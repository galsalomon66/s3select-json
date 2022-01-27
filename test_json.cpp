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

std::string get_value_sax(const char* file_name, const char* key)
{
  std::string stringFromStream;
  std::ifstream in;
  std::stringstream result;
  std::string final_result;

  rapidjson::MemoryStream buffer(file_name, 1024);

  MyHandler handler;
  handler.set_search_key(key);
  rapidjson::Reader reader{};

  reader.IterativeParseInit();
  while (!reader.IterativeParseComplete()) {
    reader.IterativeParseNext<rapidjson::kParseDefaultFlags>(buffer, handler);
    if(reader.HasParseError())  {
      rapidjson::ParseErrorCode c = reader.GetParseErrorCode();
      size_t o = reader.GetErrorOffset();
      std::cout << "PARSE ERROR " << c << " " << o << std::endl;
      break;
      }
    }
  
  for (const auto& i : handler.get_myvalue()) {
    switch(i.type()) {
      case Valuesax::Decimal: result << i.asInt() << "\n"; break;
      case Valuesax::Double: result << i.asDouble() << "\n"; break;
      case Valuesax::String: result << i.asString() << "\n"; break;
      case Valuesax::Bool: result << std::boolalpha << i.asBool() << "\n"; break;
      case Valuesax::Null: result << "null" << "\n"; break;
      default: break;
    }
  }
  final_result = result.str();
  return final_result;
}

std::string get_next_key_sax(const char* file_name, const char* key)
{
  std::string stringFromStream;
  std::ifstream in;
  std::stringstream result;
  std::string final_result;
  
  rapidjson::MemoryStream buffer(file_name, 1024);

  MyHandler handler;
  handler.set_search_prev_key(key);
  rapidjson::Reader reader{};

  reader.IterativeParseInit();
  while (!reader.IterativeParseComplete()) {
    reader.IterativeParseNext<rapidjson::kParseDefaultFlags>(buffer, handler);
    if(reader.HasParseError())  {
      rapidjson::ParseErrorCode c = reader.GetParseErrorCode();
      size_t o = reader.GetErrorOffset();
      std::cout << "PARSE ERROR " << c << " " << o << std::endl;
      break;
      }
    }
  
  final_result = handler.get_my_prev_key();
  return final_result;
}
/*
TEST(Jsonparse, json)
{
  

  //std::string dom_result = parse_json_dom("sample4.json");

  //ASSERT_EQ(dom_result, sax_result);
}

TEST(Jsonparse, json1)
{
  std::string sax_result_1 = parse_json_sax("sample2.json");

  std::string dom_result_1 = parse_json_dom("sample2.json");

  ASSERT_EQ(dom_result_1, sax_result_1);
}

TEST(Jsonparse, json2)
{
  std::string sax_result_2 = parse_json_sax("sample3.json");

  std::string dom_result_2 = parse_json_dom("sample3.json");

  ASSERT_EQ(dom_result_2, sax_result_2);
}

TEST(Jsonparse, json3)
{
  std::string sax_result_3 = parse_json_sax("sample8.json");

  std::string dom_result_3 = parse_json_dom("sample8.json");

  ASSERT_EQ(dom_result_3, sax_result_3);
}

TEST(Jsonparse, json4)
{
  std::string sax_result_4 = parse_json_sax("sample9.json");

  std::string dom_result_4 = parse_json_dom("sample9.json");

  ASSERT_EQ(dom_result_4, sax_result_4);
}

TEST(Jsonparse, json5)
{
  std::string sax_result_5 = parse_json_sax("sample13.json");

  std::string dom_result_5 = parse_json_dom("sample13.json");

  ASSERT_EQ(dom_result_5, sax_result_5);
}*/

int main(int argc, char* argv[])
{/*
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();*/

  const char* json2 = "{ \
  \"firstName\": \"Joe\", \
  \"lastName\": \"Jackson\", \
  \"gender\": \"male\", \
  \"age\": \"twenty\", \
  \"address\": { \
  \"streetAddress\": \"101\", \
  \"city\": \"San Diego\", \
  \"state\": \"CA\" \
  }, \
  \"phoneNumbers\": [ \
  { \"type\": \"home1\", \"number\": \"7349282_1\" }, \
  { \"type\": \"home2\", \"number\": \"7349282_2\" }, \
  { \"type\": \"home3\", \"number\": \"734928_3\" }, \
  { \"type\": \"home4\", \"number\": \"734928_4\" }, \
  { \"type\": \"home5\", \"number\": \"734928_5\" }, \
  { \"type\": \"home6\", \"number\": \"734928_6\" }, \
  { \"type\": \"home7\", \"number\": \"734928_7\" } \
  ] \
  }";

  const char* key = "address/streetAddress/";

  std::string sax_result = get_value_sax(json2, key);

  std::cout<<sax_result;

  std::string sax_next_key = get_next_key_sax(json2, key);

  std::cout<<sax_next_key;
}
