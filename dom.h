#ifndef DOM_H
#define DOM_H

#include "rapidjson/document.h"
#include <sstream>

class dom_traverse_v2
{
  public: 
    std::stringstream ss;
    void print(const rapidjson::Value &v, std::string);
    void traverse(rapidjson::Document &d);
    void traverse_object(const rapidjson::Value &v,std::string path);
    void traverse_array(const rapidjson::Value &v,std::string path);
};

#endif