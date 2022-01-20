#ifndef SAX_H
#define SAX_H

#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"
#include <cassert>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>

class Valuesax {
public:
    enum Type {
        Decimal,
        Double,
        String,
        Bool,
        Null
    };

    Valuesax Parse(std::string const& s) {
    	Valuesax result;
    	result._type = Valuesax::String;

    	result._string = s;
    	return result;
	  }

	  Valuesax Parse(const double& s) {
    	Valuesax result;
    	result._type = Valuesax::Double;

    	result._double = s;
    	return result;
	  }

	  Valuesax Parse(bool& s) {
    	Valuesax result;
    	result._type = Valuesax::Bool;

    	result._bool = s;
    	return result;
	  }

    Valuesax Parse(const int& s) {
    	Valuesax result;
    	result._type = Valuesax::Decimal;

    	result._num = s;
    	return result;
	  }

	  Valuesax Parse(const unsigned& s) {
    	Valuesax result;
    	result._type = Valuesax::Decimal;

    	result._num = s;
    	return result;
    }

    Valuesax Parse(const int64_t& s) {
    	Valuesax result;
    	result._type = Valuesax::Decimal;

    	result._num = s;
    	return result;
	  }

    Valuesax Parse(const uint64_t& s) {
    	Valuesax result;
    	result._type = Valuesax::Decimal;

    	result._num = s;
    	return result;
	  }

	  Valuesax Parse(const std::nullptr_t& s) {
    	Valuesax result;
    	result._type = Valuesax::Null;

    	result._null = s;
    	return result;
	  }

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

class MyHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, MyHandler> {
    Valuesax value;
    std::vector <Valuesax> myvalue;
    std::string my_prev_key{};
    std::string search_key;
    std::string search_prev_key{};
    std::vector <char> vect;
    std::string keyname{};
    std::string keyvalue;
    std::string prev_key{};
    bool valuep{};
    int start_counter{};
    std::vector<std::string> mystack;
    std::unordered_map<int, std::string> key_stack;
    public:
    MyHandler() : valuep{false}, start_counter{0} {}
    void set_search_key(std::string key) {
      search_key = key;
    }
    void set_search_prev_key(std::string key) {
      search_prev_key = key;
    }
    std::string get_my_prev_key()
    {
      return my_prev_key;
    }
    std::vector<Valuesax> get_myvalue() {
      return myvalue;
    }
    bool Null() {
      if (keyvalue == search_key) {
          myvalue.push_back(value.Parse(nullptr));
        }
      valuep = true; 
      return true; }
    bool Bool(bool b) {
      if (keyvalue == search_key) {
          myvalue.push_back(value.Parse(b));
        }
      valuep = true;
      return true; }
    bool Int(int i) { 
      if (keyvalue == search_key) {
          myvalue.push_back(value.Parse(i));
        }
      return true; }
    bool Uint(unsigned u) {
      if (keyvalue == search_key) {
          myvalue.push_back(value.Parse(u));
        }
      valuep = true;
      return true; }
    bool Int64(int64_t i) { 
      if (keyvalue == search_key) {
          myvalue.push_back(value.Parse(i));
        }
      return true; }
    bool Uint64(uint64_t u) { 
      if (keyvalue == search_key) {
          myvalue.push_back(value.Parse(u));
        }
      return true; }
    bool Double(double d) { 
      if (keyvalue == search_key) {
          myvalue.push_back(value.Parse(d));
        }
      valuep = true;
      return true; }
    bool String(const char* str, rapidjson::SizeType length, bool copy) {
        if (keyvalue == search_key) {
          myvalue.push_back(value.Parse(str));
        }
        valuep = true;
        return true;
    }
    
  bool StartObject() {
    if(!valuep) {
      if (mystack.size() == 0 || mystack[mystack.size() - 1] != keyname) {
        if (keyname.length()) {
        mystack.push_back(keyname);
        start_counter = vect.size();
        key_stack[start_counter] = keyname;
      }
      }
    }
    vect.push_back('{');
    return true; 
    }

  bool Key(const char* str, rapidjson::SizeType length, bool copy) {
    prev_key = keyvalue;
    std::vector<std::string> stack{mystack};
    stack.push_back(str);
    valuep = false;
    keyvalue = "";
    
    for (const auto& i: stack) {
      keyvalue += i + '/';
    }
    keyname = str;
    if (search_prev_key == prev_key) {
      my_prev_key = keyvalue;
    }
    return true;
    }

  bool EndObject(rapidjson::SizeType memberCount) {
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

  bool EndArray(rapidjson::SizeType elementCount) { 
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

#endif