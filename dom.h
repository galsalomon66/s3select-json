#ifndef DOM_H
#define DOM_H

#include "rapidjson/document.h"
#include <sstream>

using namespace rapidjson;

class dom_traverse_v2
{
  public: 
    std::stringstream ss;
    void print(const Value &v, std::string);
    void traverse(Document &d);
    void traverse_object(const Value &v,std::string path);
    void traverse_array(const Value &v,std::string path);
};

#endif