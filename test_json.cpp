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

    void resetBuffer(char* buff, size_t bytes_left, std::ifstream& input_file_stream)//TODO add chunk as parameter
    {
      // copy remain stream
      size_t buffer_sz{4096};
      memcpy(buff, src_, bytes_left);
      auto read_size = input_file_stream.readsome(buff + bytes_left, buffer_sz - bytes_left);
      begin_ = buff;
      src_ = buff;
      size_ = read_size + bytes_left;
      end_= src_ + size_;
    }

    void PushDataToProcess()
    {

    }

    size_t getBytesLeft() { return end_ - src_; }

};

std::string extract_key_values(char* buff,uint64_t buffer_sz, const char* file_name)
{
  std::ifstream input_file_stream;
  try {
    input_file_stream = std::ifstream(file_name, std::ios::in | std::ios::binary);
  }
  catch( ... ){
    std::cout << "failed to open file " << std::endl;  
    exit(-1);
  }

  std::stringstream result;
  std::string final_result;

  //rapidjson::MemoryStream buffer(buff, buffer_sz);

  //read first chunk;
  auto read_size = input_file_stream.readsome(buff, buffer_sz);
  //set the memoryStreamer
  my_memory_stm buffer(buff, read_size);

  MyHandler handler;
  rapidjson::Reader reader{};

  reader.IterativeParseInit();
  while (!reader.IterativeParseComplete()) {
    reader.IterativeParseNext<rapidjson::kParseDefaultFlags>(buffer, handler);
    buffer.PushDataToProcess();

    //calculate how much left to process
    size_t bytes_left = buffer.getBytesLeft();

    //upon true, the non-processed bytes plus the next chunk are copy into main processing buffer 
    if (bytes_left < buffer_sz/2)//TODO this condition could be replaced
    {
    // memoryStreamer are reset per the new buffer
      buffer.resetBuffer(buff, bytes_left, input_file_stream);
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

TEST(Jsonparse, json)
{
  size_t buff_sz = 4096;
  char* buff = (char*)malloc(buff_sz);

  std::string sax_result = extract_key_values(buff, buff_sz, "sample4.json");

  std::string dom_result = parse_json_dom("sample4.json");

  ASSERT_EQ(dom_result, sax_result);
}

TEST(Jsonparse, json1)
{
  size_t buff_sz = 4096;
  char* buff = (char*)malloc(buff_sz);

  std::string sax_result_1 = extract_key_values(buff, buff_sz, "sample2.json");

  std::string dom_result_1 = parse_json_dom("sample2.json");

  ASSERT_EQ(dom_result_1, sax_result_1);
}

TEST(Jsonparse, json2)
{
  size_t buff_sz = 4096;
  char* buff = (char*)malloc(buff_sz);

  std::string sax_result_2 = extract_key_values(buff, buff_sz, "sample3.json");

  std::string dom_result_2 = parse_json_dom("sample3.json");

  ASSERT_EQ(dom_result_2, sax_result_2);
}

TEST(Jsonparse, json3)
{
  size_t buff_sz = 4096;
  char* buff = (char*)malloc(buff_sz);

  std::string sax_result_3 = extract_key_values(buff, buff_sz, "sample8.json");

  std::string dom_result_3 = parse_json_dom("sample8.json");

  ASSERT_EQ(dom_result_3, sax_result_3);
}

TEST(Jsonparse, json4)
{
  size_t buff_sz = 4096;
  char* buff = (char*)malloc(buff_sz);

  std::string sax_result_4 = extract_key_values(buff, buff_sz, "sample9.json");

  std::string dom_result_4 = parse_json_dom("sample9.json");

  ASSERT_EQ(dom_result_4, sax_result_4);
}

TEST(Jsonparse, json5)
{
  size_t buff_sz = 4096;
  char* buff = (char*)malloc(buff_sz);

  std::string sax_result_5 = extract_key_values(buff, buff_sz, "sample13.json");

  std::string dom_result_5 = parse_json_dom("sample13.json");

  ASSERT_EQ(dom_result_5, sax_result_5);
}

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();

  return 0;
}
