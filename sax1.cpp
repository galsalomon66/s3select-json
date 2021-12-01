#include <iostream>
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"
#include <boost/lexical_cast.hpp>
#include <cassert>
#include <sstream>
#include <chrono>
#include <fstream>
#include <vector>
#include <iterator>

using namespace rapidjson;

class Value {
public:
    enum Type {
        Int,
        Double,
        String,
    };
    
    static Value Parse(std::string const& s);
    static Value Parse(double const& s);

    Value(): _type(Int), _int(0), _double(0.0) {} 

    Type type() const { return _type; }

    int asInt() const {
        assert(_type == Int && "not an int");
        return _int;
    }

    double asDouble() const {
        assert(_type == Double && "not a double");
        return _double;
    }

    std::string const& asString() const {
        assert(_type == String && "not a string");
        return _string; 
    }

private:
    Type _type;
    int _int;
    double _double;
    std::string _string;
};

Value Value::Parse(std::string const& s) {
    Value result;
    result._type = Value::String;

    result._string = s;
    return result;
}

Value Value::Parse(double const& s) {
    Value result;
    result._type = Value::Double;

    result._double = s;
    return result;
}

class Timer
{
  using clock_type = std::chrono::steady_clock;
  using second_type = std::chrono::duration<double, std::ratio<1> >;

  std::chrono::time_point<clock_type> m_beg;

public:
  Timer() : m_beg { clock_type::now() }
  {
  }

  void reset()
  {
    m_beg = clock_type::now();
  }

  double elapsed() const
  {
    return std::chrono::duration_cast<second_type>(clock_type::now() - m_beg).count();
  }
};

class SearchCriterion
{
   public:
   std::vector<std::string>  value;
   std::vector <std::string> key;
   void SetValue(const std::string& in) {value.push_back(in);}
   void SetKey(const std::string& in) {key.push_back(in);}
   std::vector <std::string> getKeys(){return key;}
   std::vector <std::string> getValue(){return value;}
};

class MyHandler : public BaseReaderHandler<UTF8<>, MyHandler> {
    public:
    std::vector < std::pair < std::string, Value>>  mymap;
    SearchCriterion oSearchCriterion;
    std::string keyname;
    std::string keypath;
    bool Null() { return true; }
    bool Bool(bool b) { return true; }
    bool Int(int i) { return true; }
    bool Uint(unsigned u) { return true; }
    bool Int64(int64_t i) { return true; }
    bool Uint64(uint64_t u) { return true; }
    bool Double(double d) { 
      mymap.push_back(std::make_pair(keypath, Value::Parse(d)));
      return true; }
    bool String(const char* str, SizeType length, bool copy) {
        oSearchCriterion.SetValue(str);  
        mymap.push_back(std::make_pair(keypath, Value::Parse(str)));
        return true;
    }
    bool StartObject() { 
      return true; }
    bool Key(const char* str, SizeType length, bool copy) {
        oSearchCriterion.SetKey(str);
        keyname = str;
        keypath += '/' + keyname;
        return true;
    }
    bool EndObject(SizeType memberCount) {
      keypath = "";
      return true;
    }
    bool StartArray() { return true; }
    bool EndArray(SizeType elementCount) { return true; }
};

int main(int argc, char* argv[])
{
   std::string stringFromStream;
   std::ifstream in;
   in.open("sample9.json", std::ifstream::in);
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
    Timer t;
    reader.Parse(ss, handler);

    const std::vector<std::string> keys{handler.oSearchCriterion.getKeys()};
    for (auto i : keys) {
      std::cout<<i<<"\n";
    }

    const std::vector<std::string> values{handler.oSearchCriterion.getValue()};
    for (auto i : values) {
      std::cout<<i<<"\n";
    }

    std::cout<<"Key-value pairs are: \n";

    for (auto const& i: handler.mymap) {
        std::cout<<i.first<<": ";
        switch((i.second).type()) {
        case Value::Int: std::cout << (i.second).asInt() << "\n"; break;
        case Value::Double: std::cout << (i.second).asDouble() << "\n"; break;
        case Value::String: std::cout << (i.second).asString() << "\n"; break;
        default: break;
        }
    }

    std::cout<<"Time taken: " << t.elapsed() << " seconds\n";
}