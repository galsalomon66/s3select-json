#include <iostream>
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"
#include <cassert>
#include <sstream>
#include <chrono>
#include <fstream>
#include <vector>
#include <iterator>
#include <cstddef>

using namespace rapidjson;

class Value {
public:
    enum Type {
        Decimal,
        Double,
        String,
        Bool,
        Null
    };
    
    static Value Parse(std::string const& s);
    static Value Parse(const double& s);
    static Value Parse(bool& s);
    static Value Parse(const unsigned& s);
    static Value Parse(const std::nullptr_t& s);

    Value(): _type(Decimal), _num(0), _double(0.0), _bool(false) {}

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

Value Value::Parse(std::string const& s) {
    Value result;
    result._type = Value::String;

    result._string = s;
    return result;
}

Value Value::Parse(const double& s) {
    Value result;
    result._type = Value::Double;

    result._double = s;
    return result;
}

Value Value::Parse(bool& s) {
    Value result;
    result._type = Value::Bool;

    result._bool = s;
    return result;
}

Value Value::Parse(const unsigned& s) {
    Value result;
    result._type = Value::Decimal;

    result._num = s;
    return result;
}

Value Value::Parse(const std::nullptr_t& s) {
    Value result;
    result._type = Value::Null;

    result._null = s;
    return result;
}

class MyHandler : public BaseReaderHandler<UTF8<>, MyHandler> {
    public:
    std::vector < std::pair < std::string, Value>>  mymap;
    std::vector <char> vect;
    std::string keyname;
    std::string keyvalue;
    bool valuep;
    int start_counter{0};
    std::vector<std::string> mystack;
    bool Null() {
      mymap.push_back(std::make_pair(keyvalue, Value::Parse(nullptr)));
      valuep = true; 
      return true; }
    bool Bool(bool b) {
      mymap.push_back(std::make_pair(keyvalue, Value::Parse(b)));
      valuep = true;
      return true; }
    bool Int(int i) { 
      return true; }
    bool Uint(unsigned u) {
      mymap.push_back(std::make_pair(keyvalue, Value::Parse(u)));
      valuep = true;
      return true; }
    bool Int64(int64_t i) { 
      return true; }
    bool Uint64(uint64_t u) { 
      return true; }
    bool Double(double d) { 
      mymap.push_back(std::make_pair(keyvalue, Value::Parse(d)));
      valuep = true;
      return true; }
    bool String(const char* str, SizeType length, bool copy) {
        mymap.push_back(std::make_pair(keyvalue, Value::Parse(str)));
        valuep = true;
        return true;
    }
    
  bool StartObject() {
    if(!valuep) {
      if (mystack.size() == 0 || mystack[mystack.size() - 1] != keyname) {
        mystack.push_back(keyname);
        start_counter = vect.size();
      }
    }
    vect.push_back('{');
    return true; 
    }

  bool Key(const char* str, SizeType length, bool copy) {
    std::vector<std::string> stack{mystack};
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
    if (vect.size()  == start_counter) {
    mystack.pop_back();
    --start_counter;
  }
    return true;
    }

  bool StartArray() {
    if(!valuep) {
    mystack.push_back(keyname);
    start_counter = vect.size();
  }
    vect.push_back('[');
    return true; 
  }

  bool EndArray(SizeType elementCount) { 
    vect.pop_back();
    if (vect.size()  == start_counter) {
    mystack.pop_back();
    --start_counter;
  }
    return true; 
  }
};

int main(int argc, char* argv[])
{
  std::string stringFromStream;
  std::ifstream in;
  const char *jsonfile = argv[1];
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

  std::cout<<"Key-value pairs are: \n";

  for (auto const& i: handler.mymap) {
    std::cout<<i.first<<": ";
    switch((i.second).type()) {
      case Value::Decimal: std::cout << (i.second).asInt() << "\n"; break;
      case Value::Double: std::cout << (i.second).asDouble() << "\n"; break;
      case Value::String: std::cout << (i.second).asString() << "\n"; break;
      case Value::Bool: std::cout << std::boolalpha << (i.second).asBool() << "\n"; break;
      case Value::Null: std::cout << "null" << "\n"; break;
      default: break;
    }
  }
}
