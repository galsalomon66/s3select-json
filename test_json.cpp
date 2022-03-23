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


void RGW_send_data(const char* object_name)
{//purpose: simulate RGW streaming an object into s3select

  std::ifstream input_file_stream;
  MyHandler handler;
  size_t buff_sz{4096};
  char* buff = (char*)malloc(buff_sz);

  try {
    input_file_stream = std::ifstream(object_name, std::ios::in | std::ios::binary);
  }
  catch( ... ){
    std::cout << "failed to open file " << std::endl;  
    exit(-1);
  }

  //read first chunk;
  auto read_size = input_file_stream.readsome(buff, buff_sz);
  while(read_size)
  {
    //the handler is processing any buffer size
    handler.process_rgw_buffer(buff, read_size);
    
    //read next chunk
    read_size = input_file_stream.readsome(buff, buff_sz);
  }
  handler.process_rgw_buffer(0, 0, true);

}

int main(int argc, char* argv[])
{
  RGW_send_data(argv[1]);

  return 0;
}
