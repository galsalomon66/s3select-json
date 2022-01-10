#include <iostream>
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/stringbuffer.h"
#include <gtest/gtest.h>
#include <cassert>
#include <sstream>
#include <chrono>
#include <fstream>
#include <vector>
#include <unordered_map>

using namespace rapidjson;

class Valuesax {
  public:
    enum Type {
      Decimal,
      Double,
      String,
      Bool,
      Null
    };

    static Valuesax Parse(std::string const& s);
    static Valuesax Parse(const double& s);
    static Valuesax Parse(bool& s);
    static Valuesax Parse(const unsigned& s);
    static Valuesax Parse(const std::nullptr_t& s);

    Valuesax(): _type(Decimal), _num(0), _double(0.0), _bool(false) {}

    Type type() const { return _type; }

    int asInt() const {
      assert(_type == Decimal && "not an int");
      return _num;
    }

    double asDouble() const {
      assert(_type == Double && "not a double");
      return _double;
    }

    std::string const& asString() const {
      assert(_type == String && "not a string");
      return _string;
    }

    bool asBool() const {
      assert(_type == Bool && "not a bool");
      return _bool;
    }

  private:
    Type _type;
    double _double;
    std::string _string;
    bool _bool;
    int64_t _num;;
    std::nullptr_t _null;
};

Valuesax Valuesax::Parse(std::string const& s) {
  Valuesax result;
  result._type = Valuesax::String;

  result._string = s;
  return result;
}

Valuesax Valuesax::Parse(const double& s) {
  Valuesax result;
  result._type = Valuesax::Double;

  result._double = s;
  return result;
}

Valuesax Valuesax::Parse(bool& s) {
  Valuesax result;
  result._type = Valuesax::Bool;

  result._bool = s;
  return result;
}

Valuesax Valuesax::Parse(const unsigned& s) {
  Valuesax result;
  result._type = Valuesax::Decimal;

  result._num = s;
  return result;
}

Valuesax Valuesax::Parse(const std::nullptr_t& s) {
  Valuesax result;
  result._type = Valuesax::Null;

  result._null = s;
  return result;
}

class MyHandler : public BaseReaderHandler<UTF8<>, MyHandler> {
  public:
    std::vector < std::pair < std::string, Valuesax>>  mymap;
    std::vector <char> vect;
    std::string keyname{"first"};
    std::string keyvalue;
    bool valuep{false};
    int start_counter{0};
    std::vector<std::string> mystack;
    std::unordered_map<int, std::string> key_stack;//TODO why not using an array?

    bool Null() {
      mymap.push_back(std::make_pair(keyvalue, Valuesax::Parse(nullptr)));
      valuep = true; 
      return true; }

    bool Bool(bool b) {
      mymap.push_back(std::make_pair(keyvalue, Valuesax::Parse(b)));
      valuep = true;
      return true; }

    bool Int(int i) {//TODO to complete 
      return true; }

    bool Uint(unsigned u) {
      mymap.push_back(std::make_pair(keyvalue, Valuesax::Parse(u)));
      valuep = true;
      return true; }

    bool Int64(int64_t i) {//TODO to complete  
      return true; }

    bool Uint64(uint64_t u) {//TODO to complete  
      return true; }

    bool Double(double d) { 
      mymap.push_back(std::make_pair(keyvalue, Valuesax::Parse(d)));
      valuep = true;
      return true; }

    bool String(const char* str, SizeType length, bool copy) {
      mymap.push_back(std::make_pair(keyvalue, Valuesax::Parse(str)));
      valuep = true;
      return true;
    }

    bool StartObject() {
      if(!valuep) {
	if (mystack.size() == 0 || mystack[mystack.size() - 1] != keyname) {
	  if (keyname != "first") {//TODO "first" could be real key-name, it could not be used as a state
	    mystack.push_back(keyname);
	    start_counter = vect.size();
	    key_stack[start_counter] = keyname;
	  }
	}
      }
      vect.push_back('{');
      return true; 
    }

    bool Key(const char* str, SizeType length, bool copy) {
      std::vector<std::string> stack{mystack}; //TODO on each call it construct stack with mystack
      stack.push_back(str);
      valuep = false;
      keyvalue = "";

      for (const auto& i: stack) {
	keyvalue += i + '/';
      }
      keyname = str;

      return true;
    }

    bool EndObject(SizeType memberCount) {
      vect.pop_back();
      if (mystack.size() > 0 && vect.size()  == start_counter) {
	if(key_stack[vect.size()] == mystack.back()) {
	  mystack.pop_back();
	}
	--start_counter;
      }
      return true;
    }

    bool StartArray() {
      if(!valuep) {
	if (mystack.size() == 0 || mystack[mystack.size() - 1] != keyname) {
	  mystack.push_back(keyname);
	  start_counter = vect.size();
	  key_stack[start_counter] = keyname;
	}
      }
      vect.push_back('[');
      return true; 
    }

    bool EndArray(SizeType elementCount) { 
      vect.pop_back();
      if (mystack.size() > 0 && vect.size()  == start_counter) {
	if(key_stack[vect.size()] == mystack.back()) {
	  mystack.pop_back();
	}
	--start_counter;
      }
      return true; 
    }
};

std::string parse_json_sax(const char* file_name)
{// the parser is actually load the whole input (several GB .... )
  std::string stringFromStream;
  std::ifstream in;
  std::stringstream result;
  std::string final_result;
  const char *jsonfile = file_name;;
  in.open(jsonfile, std::ifstream::in);
  if (in.is_open()) {
    std::string line;
    while (getline(in, line)) {
      stringFromStream.append(line + "\n"); //the entire file is loaded into stringFromStream
    }
    in.close();
  }

  const char* data = stringFromStream.c_str();

  MyHandler handler;
  Reader reader;
  StringStream ss(data);
  reader.Parse(ss, handler);

  for (auto const& i: handler.mymap) { //mymap contains all keys/vause, could be huge amount of data
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

class dom_traverse_v2
{
  public: 
    std::stringstream ss;
    void print(const Value &v, std::string);
    void traverse(Document &d);
    void traverse_object(const Value &v,std::string path);
    void traverse_array(const Value &v,std::string path);
};

void dom_traverse_v2::print(const Value &v, std::string key_name)
{
  ss << key_name << ": ";

  if(v.IsString())
  {
    ss << v.GetString() << std::endl;
  }
  else
    if(v.IsInt())
    {
      ss << v.GetInt() << std::endl;
    }
    else
      if(v.IsBool())
      {
	ss << (v.GetBool() ? "true" : "false" ) << std::endl;
      }
      else
	if(v.IsNull())
	{
	  ss << "null" << std::endl;
	}
	else
	  if(v.IsDouble())
	  {
	    ss << v.GetDouble() << std::endl;
	  }
	  else
	  {
	    ss << "value not exist" << std::endl;
	  }
}

void dom_traverse_v2::traverse(Document &d)
{
  std::string path="";

  for (Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr)
  {
    const Value &v = itr->value;

    if(v.IsArray())
    {
      std::string path="";
      path.append( itr->name.GetString() );
      path.append( "/" );

      traverse_array(v, path);
    }
    else if (v.IsObject())
    {
      std::string path="";
      path.append( itr->name.GetString() );
      path.append( "/" );

      traverse_object(v, path);
    }
    else
    {
      std::string tmp = path;
      path.append( itr->name.GetString() );
      path.append( "/" );
      print(v, path);
      path = tmp;
    }

  }
}

void dom_traverse_v2::traverse_array(const Value &v,std::string path)
{
  std::string object_key = path;

  for (Value::ConstValueIterator itr = v.Begin(); itr != v.End(); ++itr)
  {
    const Value& array_item = *itr;
    if(array_item.IsArray())
    {
      traverse_array(array_item,object_key);
    }
    else if(array_item.IsObject())
    {
      traverse_object(array_item,object_key);
    }
    else
    {
      print(array_item, object_key);
    }
  }
}

void dom_traverse_v2::traverse_object(const Value &v,std::string path)
{
  std::string object_key = path;

  for (Value::ConstMemberIterator itr = v.MemberBegin(); itr != v.MemberEnd(); ++itr)
  {
    const Value& v_itr = itr->value;

    if (itr->value.IsObject())
    {
      std::string tmp = object_key;
      object_key.append( itr->name.GetString() );
      object_key.append("/");
      traverse_object(v_itr,object_key);
      object_key = tmp;
    }
    else
      if (itr->value.IsArray())
      {
	object_key.append( itr->name.GetString() );
	object_key.append("/");
	traverse_array(v_itr,object_key);
      }
      else
      {
	std::string tmp = object_key;
	object_key.append( itr->name.GetString() );
	object_key.append("/");
	print(v_itr, object_key);
	object_key = tmp;
      }

  }
}

std::string parse_json_dom(const char* file_name)
{
  std::string final_result;

  const char * dom_input_file_name = file_name;

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
}

int main(int argc, char* argv[])
{

  testing::InitGoogleTest(&argc, argv);

  //return RUN_ALL_TESTS();

  std::cout << parse_json_sax( argv[1] ) ;
}


