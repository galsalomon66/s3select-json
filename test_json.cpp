#include <iostream>
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/stringbuffer.h"
#include "dom.h"
#include "sax.h"
#include <gtest/gtest.h>
#include <cassert>
#include <sstream>
#include <chrono>
#include <fstream>
#include <vector>
#include <unordered_map>


using namespace rapidjson;

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

  Document document;
  document.Parse(file_content.data());

  if (document.HasParseError()) {
    std::cout<<"parsing error"<< std::endl;
  }

  if (!document.IsObject())
  {
    std::cout << " input is not an object " << std::endl;
  }

  dom_traverse_v2 td2;

  td2.traverse( document );

  final_result = (td2.ss).str();

  return final_result;
}

std::string parse_json_sax(const char* file_name)
{
  std::string stringFromStream;
  std::ifstream in;
  std::stringstream result;
  std::string final_result;
  const char *jsonfile = file_name;;
  in.open(jsonfile, std::ifstream::in);
  if (in.is_open()) {
    std::string line;
    while (getline(in, line)) {
        stringFromStream.append(line + "\n");
    }
    in.close();
  }

  const char* data = stringFromStream.c_str();

  MyHandler handler;
  Reader reader;
  StringStream ss(data);
  reader.Parse(ss, handler);

  for (auto const& i: handler.mymap) {
    result<<i.first<<": ";
    switch((i.second).type()) {
      case Valuesax::Decimal: result << (i.second).asInt() << "\n"; break;
      case Valuesax::Double: result << (i.second).asDouble() << "\n"; break;
      case Valuesax::String: result << (i.second).asString() << "\n"; break;
      case Valuesax::Bool: result << std::boolalpha << (i.second).asBool() << "\n"; break;
      case Valuesax::Null: result << "null" << "\n"; break;
      default: break;
    }
  }
  final_result = result.str();
  return final_result;
}

TEST(Jsonparse, json)
{
  std::string sax_result = parse_json_sax("sample4.json");

  std::string dom_result = parse_json_dom("sample4.json");

  ASSERT_EQ(dom_result, sax_result);

  std::string sax_result_1 = parse_json_sax("sample2.json");

  std::string dom_result_1 = parse_json_dom("sample2.json");

  ASSERT_EQ(dom_result_1, sax_result_1);

  std::string sax_result_2 = parse_json_sax("sample3.json");

  std::string dom_result_2 = parse_json_dom("sample3.json");

  ASSERT_EQ(dom_result_2, sax_result_2);

  std::string sax_result_3 = parse_json_sax("sample8.json");

  std::string dom_result_3 = parse_json_dom("sample8.json");

  ASSERT_EQ(dom_result_3, sax_result_3);

  std::string sax_result_4 = parse_json_sax("sample9.json");

  std::string dom_result_4 = parse_json_dom("sample9.json");

  ASSERT_EQ(dom_result_4, sax_result_4);

  std::string sax_result_5 = parse_json_sax("sample13.json");

  std::string dom_result_5 = parse_json_dom("sample13.json");

  ASSERT_EQ(dom_result_5, sax_result_5);
}

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
