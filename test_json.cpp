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

    void resetBuffer(char* buff, size_t size)//TODO add chunk as parameter
    {
      begin_ = buff;
      src_ = buff;
      size_ = size;
      end_= src_ + size_;
    }

    void copy_stream(char* buff, size_t bytes_left)
    {
      memcpy(buff, src_, bytes_left);
    }

    size_t getBytesLeft() { return end_ - src_; }

};

void extract_key_values(char* buff,uint64_t buffer_sz, my_memory_stm &buffer, rapidjson::Reader& reader, MyHandler& handler)
{
  while (!reader.IterativeParseComplete()) {
    reader.IterativeParseNext<rapidjson::kParseDefaultFlags>(buffer, handler);

      //upon true, the non-processed bytes plus the next chunk are copy into main processing buffer 
    if (buffer.getBytesLeft() < buffer_sz/2)//TODO this condition could be replaced
     {
        buffer.copy_stream(buff, buffer.getBytesLeft());
        return;
      }
      // error message
    if(reader.HasParseError())  {
      rapidjson::ParseErrorCode c = reader.GetParseErrorCode();
      size_t o = reader.GetErrorOffset();
      std::cout << "PARSE ERROR " << c << " " << o << std::endl;
      break;
    }
  }
}

TEST(Jsonparse, json)
{
  std::ifstream input_file_stream;
  std::string sax_result;
  std::stringstream result;
  rapidjson::Reader reader{};
  MyHandler handler;

  try {
    input_file_stream = std::ifstream("sample4.json", std::ios::in | std::ios::binary);
  }
  catch( ... ){
    std::cout << "failed to open file " << std::endl;  
    exit(-1);
  }
  size_t buff_sz{4096};
  char* buff = (char*)malloc(buff_sz);
  int counter{};

  //read first chunk;
  auto read_size = input_file_stream.readsome(buff, buff_sz);

  //set the memoryStreamer
  my_memory_stm buffer(buff, read_size);
  reader.IterativeParseInit();

  while(1) {
    if(!read_size || input_file_stream.eof()){
      break;
    }

    if (counter) {
      auto read_size = input_file_stream.readsome(buff + buffer.getBytesLeft(), buff_sz - buffer.getBytesLeft());
      // memoryStreamer are reset per the new buffer
      buffer.resetBuffer(buff, buffer.getBytesLeft() + read_size);
      if (reader.IterativeParseComplete()) {
        break;
      }
    }

  extract_key_values(buff, buff_sz, buffer, reader, handler);
  ++counter;
  }

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

  sax_result = result.str();

  std::string dom_result = parse_json_dom("sample4.json");

  ASSERT_EQ(dom_result, sax_result);
}
/*
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
}*/

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();

  return 0;
}
