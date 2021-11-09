#include <iostream>
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"
#include <chrono>
#include <fstream>
#include <vector>

using namespace rapidjson;

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
    bool Null() { return true; }
    bool Bool(bool b) { return true; }
    bool Int(int i) { return true; }
    bool Uint(unsigned u) { return true; }
    bool Int64(int64_t i) { return true; }
    bool Uint64(uint64_t u) { return true; }
    bool Double(double d) { return true; }
    bool String(const char* str, SizeType length, bool copy) {
        oSearchCriterion.SetValue(str);  
        return true;
    }
    bool StartObject() { return true; }
    bool Key(const char* str, SizeType length, bool copy) {
        oSearchCriterion.SetKey(str);
        return true;
    }
    bool EndObject(SizeType memberCount) {
       return true;
    }
    bool StartArray() { return true; }
    bool EndArray(SizeType elementCount) { return true; }
};

int main() 
{
    const char* data = "{\"selectionCriteria\": {\"maxEntries\": \"3\",\"searchCriteria\": {\"criterion\": {\"type\": \"Attribute\",\"name\": \"root\",\"value\": \"Yes\"}}}}";

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
    std::cout<<"Time taken: " << t.elapsed() << " seconds\n";
    }
