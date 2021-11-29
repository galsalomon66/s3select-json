#include <iostream>
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"
#include <chrono>
#include <fstream>
#include <vector>
#include <any>

using namespace rapidjson;

std::vector < std::pair < std::string, std::string>>  mymap;
/*
std::ostream &operator<<(std::ostream &os, const std::any &m) {
    if (m.type() == typeid(int)) {
        os << std::any_cast<int>(m);
        return os;
    }
    else if (m.type() == typeid(double)) {
        os << std::any_cast<double>(m);
        return os;
    }
    else if (m.type() == typeid(char)) {
        os << std::any_cast<char>(m);
        return os;
    }
    else if (m.type() == typeid(bool)) {
        os << std::boolalpha << std::any_cast<bool>(m);
        return os;
    }
    else if (m.type() == typeid(std::string)) {
        os << std::boolalpha << std::any_cast<std::string>(m);
        return os;
    }
    else  {
        os <<std::any_cast<const char*>(m);
        return os;
    }
    return os;
}
*/

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
    SearchCriterion oSearchCriterion;
    std::string keyname;
    std::string keystart;
    std::string keypath;
    bool Null() { return true; }
    bool Bool(bool b) { return true; }
    bool Int(int i) { return true; }
    bool Uint(unsigned u) { return true; }
    bool Int64(int64_t i) { return true; }
    bool Uint64(uint64_t u) { return true; }
    bool Double(double d) { return true; }
    bool String(const char* str, SizeType length, bool copy) {
        oSearchCriterion.SetValue(str);  
        mymap.push_back(std::make_pair(keypath, str));
        return true;
    }
    bool StartObject() { 
      keystart = keyname;
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

int main() 
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

    for (auto &a : mymap) {
        std::cout << a.first << ": " << a.second << "\n";
    }

    std::cout<<"Time taken: " << t.elapsed() << " seconds\n";
}