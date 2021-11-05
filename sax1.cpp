#include <iostream>
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"
#include <chrono>
#include <fstream>

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
   std::string type;
   std::string name;
   std::string value;
   void SetValue(const std::string& in) {value = in;}
   std::string getValue(){return value;}
};

class SelectionCriteria
{
   public:
   SelectionCriteria(){}
   ~SelectionCriteria(){}

   SearchCriterion m_searchCretiteria;
   SearchCriterion& GetSearchCriteria() { return m_searchCretiteria; }

   void SetSearchCriteria(SearchCriterion& in) { m_searchCretiteria = in; }
};

class MyHandler : public BaseReaderHandler<UTF8<>, MyHandler> {
    public:
    std::string Keyname;
    SelectionCriteria oSelectionCriteria;
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
        Keyname = str;
        return true;
    }
    bool EndObject(SizeType memberCount) {
       oSelectionCriteria.SetSearchCriteria(oSearchCriterion);
       return true;
    }
    bool StartArray() { return true; }
    bool EndArray(SizeType elementCount) { return true; }
};

int main() 
{
   std::string stringFromStream;
   std::ifstream in;
   in.open("test.json", std::ifstream::in);
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

    std::cout<<"Time taken: " << t.elapsed() << " seconds\n";
    return 0;
}