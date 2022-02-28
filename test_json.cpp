#include <iostream>
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/memorystream.h"

#include "dom.h"
#include "sax.h"
#include <gtest/gtest.h>
#include <cassert>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <filesystem>

std::string parse_json_dom(const char* file_name)
{
  std::string final_result;

  const char* dom_input_file_name = file_name;

  std::fstream dom_input_file(dom_input_file_name, std::ios::in | std::ios::binary);
  dom_input_file.seekg(0, std::ios::end);

  // get file size
  auto sz = dom_input_file.tellg();
  // place the position at the begining
  dom_input_file.seekg(0, std::ios::beg);
  //read whole file content into allocated buffer
  std::string file_content(sz, '\0');
  dom_input_file.read((char*)file_content.data(),sz);

  rapidjson::Document document;
  document.Parse(file_content.data());

  if (document.HasParseError()) {
    std::cout<<"parsing error"<< std::endl;
    return "parsing error";
  }

  if (!document.IsObject())
  {
    std::cout << " input is not an object " << std::endl;
    return "object error";
  }

  dom_traverse_v2 td2;

  td2.traverse( document );

  final_result = (td2.ss).str();

  return final_result;
}

class my_memory_stm : public rapidjson::MemoryStream {

  //purpose: adding a method `resetBuffer` that enables to parse chunk after chunk
  //per each new chunk it reset internal data members
  public:

    my_memory_stm(const Ch *src, size_t size) : rapidjson::MemoryStream(src,size){}

    void resetBuffer(char *start,size_t size)//TODO add chunk as parameter
    {
      begin_ = start;
      src_ = start;
      size_ = size;
      end_= src_ + size_;
    }

    size_t getBytesLeft() { return end_ - src_; }

};

std::string extract_key_values(char* buff,uint64_t buffer_sz, std::ifstream* input_file_stream)
{
  std::stringstream result;
  std::string final_result;

  //rapidjson::MemoryStream buffer(buff, buffer_sz);

  //read first chunk;
  auto read_size = input_file_stream->readsome(buff, buffer_sz);
  //set the memoryStreamer
  my_memory_stm buffer(buff, read_size);

  MyHandler handler;
  rapidjson::Reader reader{};

  reader.IterativeParseInit();
  while (!reader.IterativeParseComplete()) {
    reader.IterativeParseNext<rapidjson::kParseDefaultFlags>(buffer, handler);

    //calculate how much left to process
    size_t bytes_left = buffer.getBytesLeft();

    //upon true, the non-processed bytes plus the next chunk are copy into main processing buffer 
    if (bytes_left < buffer_sz/2)//TODO this condition could be replaced
    {

      // copy remain stream  
      memcpy(buff, buffer.src_, bytes_left);

      // read next chunk, appended to remain stream 
      read_size = input_file_stream->readsome(buff+bytes_left, buffer_sz - bytes_left);

      // memoryStreamer are reset per the new buffer
      buffer.resetBuffer(buff,read_size+bytes_left);
    }

    // error message
    if(reader.HasParseError())  {
      rapidjson::ParseErrorCode c = reader.GetParseErrorCode();
      size_t o = reader.GetErrorOffset();
      std::cout << "PARSE ERROR " << c << " " << o << std::endl;
      break;
    }
  }

  #if 1
  //print out all key values 
  for (const auto& i : handler.get_mykeyvalue()) {
    switch(i.second.type()) {
      case Valuesax::Decimal: result << i.first << " : " << i.second.asInt() << "\n"; break;
      case Valuesax::Double: result << i.first << " : " << i.second.asDouble() << "\n"; break;
      case Valuesax::String: result << i.first << " : " << i.second.asString() << "\n"; break;
      case Valuesax::Bool: result << i.first << " : " << std::boolalpha << i.second.asBool() << "\n"; break;
      case Valuesax::Null: result << i.first << " : " << "null" << "\n"; break;
      default: break;
    }
  }
  #endif

  final_result = result.str();
  return final_result;
}

int main(int argc, char* argv[])
{
  std::ifstream input_file_stream;

  char* file_name = argv[1];

  try {
    input_file_stream = std::ifstream(file_name, std::ios::in | std::ios::binary);
  }
  catch( ... ){
    std::cout << "failed to open file " << std::endl;  
    exit(-1);
  }


  size_t buff_sz = 4096;
  char* buff = (char*)malloc(buff_sz);


  std::cout << extract_key_values(buff, buff_sz, &input_file_stream) << std::endl;

  return 0;
}
