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

  private:

    Type _type;
    double _double;
    std::string _string;
    bool _bool;
    int64_t _num;;
    std::nullptr_t _null;

  public:

    Valuesax(): _type(Decimal), _double(0.0), _bool(false),_num(0) {}

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

};

class ChunksStreamer : public rapidjson::MemoryStream {

  //purpose: adding a method `resetBuffer` that enables to parse chunk after chunk
  //per each new chunk it reset internal data members
  public:

    std::string internal_buffer;

    ChunksStreamer():rapidjson::MemoryStream(0,0){}

    ChunksStreamer(const Ch *src, size_t size) : rapidjson::MemoryStream(src,size){}

    void resetBuffer(char* buff, size_t size)
    {
      //in case bytes left to "move" it to left
      std::string tmp;
      size_t copy_left_sz = 0;
      if(getBytesLeft())
      {//TODO effieciency (src_,end_ should switch between buffers, to override ::take method, this would save the buffers copy each time)
	copy_left_sz = getBytesLeft();
	tmp.assign(src_,copy_left_sz);
	internal_buffer.assign(tmp.data(),copy_left_sz);
      }

      internal_buffer.append(buff,size);

      begin_ = internal_buffer.data();//buff;
      src_ = internal_buffer.data();//buff;
      size_ = size + copy_left_sz;
      end_= src_ + size_;
    }

    size_t getBytesLeft() { return end_ - src_; }

};

class MyHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, MyHandler> {
  public:
  std::vector < std::pair < std::string, Valuesax>> mymap;
  Valuesax value;
  std::vector <char> vect;
  std::string keyname{};
  std::string keyvalue;
  bool valuep{};
  size_t start_counter{};
  std::vector<std::string> mystack;
  std::unordered_map<int, std::string> key_stack;
  ChunksStreamer stream_buffer;
  bool init_buffer_stream;

  rapidjson::Reader reader;

  MyHandler() : valuep{false}, start_counter{0},init_buffer_stream(false)
  {}

  std::string createKey()
  {
    //loop on key stack
    //pop last element
    return std::string("");
  }

  void emptyhandler() {
    mymap.clear();
  }

  std::vector < std::pair < std::string, Valuesax>> get_mykeyvalue() {
    return mymap;
  }

  bool Null() {
    mymap.push_back(std::make_pair(keyvalue, value.Parse(nullptr)));
    valuep = true; 
    return true; }

  bool Bool(bool b) {
    mymap.push_back(std::make_pair(keyvalue, value.Parse(b)));
    valuep = true;
    return true; }

  bool Int(int i) { 
    mymap.push_back(std::make_pair(keyvalue, value.Parse(i)));
    return true; }

  bool Uint(unsigned u) {
    mymap.push_back(std::make_pair(keyvalue, value.Parse(u)));
    valuep = true;
    return true; }

  bool Int64(int64_t i) { 
    mymap.push_back(std::make_pair(keyvalue, value.Parse(i)));
    return true; }

  bool Uint64(uint64_t u) { 
    mymap.push_back(std::make_pair(keyvalue, value.Parse(u)));
    return true; }

  bool Double(double d) { 
    mymap.push_back(std::make_pair(keyvalue, value.Parse(d)));
    valuep = true;
    return true; }

  bool String(const char* str, rapidjson::SizeType length, bool copy) {
    mymap.push_back(std::make_pair(keyvalue, value.Parse(str)));
    valuep = true;
    return true;
  }

  bool StartObject() {
    if(!valuep) {
      if (mystack.size() == 0 || mystack.front() != keyname) {
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
    std::vector<std::string> stack{mystack};
    stack.push_back(str);
    valuep = false;
    keyvalue = "";

    for (const auto& i: stack) {
      if (i != "") {
	keyvalue += i + '/';
      }
    }
    keyname = str;
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

  void process_rgw_buffer(char* rgw_buffer,size_t rgw_buffer_sz, bool end_of_stream=false)
  {//RGW keeps calling with buffers, this method is not aware of object size

    std::stringstream result;

    if(!init_buffer_stream)
    {
      //set the memoryStreamer
      reader.IterativeParseInit();
      init_buffer_stream = true;
    }

    //the non-processed bytes plus the next chunk are copy into main processing buffer 
    if(!end_of_stream)
	stream_buffer.resetBuffer(rgw_buffer, rgw_buffer_sz);

    while (!reader.IterativeParseComplete()) {
      reader.IterativeParseNext<rapidjson::kParseDefaultFlags>(stream_buffer, *this);

      //IterativeParseNext returns per each parsing completion(on lexical level)
      result.str("");
      for (const auto& i : this->get_mykeyvalue()) {

	/// pushing the key-value into s3select object. that s3seelct-object should filter according to from-clause and projection defintions
	//  this object could remain empty (no key-value matches the search-pattern)
	switch(i.second.type()) {
	  case Valuesax::Decimal: result << i.first << " : " << i.second.asInt() << "\n"; break;
	  case Valuesax::Double: result << i.first << " : " << i.second.asDouble() << "\n"; break;
	  case Valuesax::String: result << i.first << " : " << i.second.asString() << "\n"; break;
	  case Valuesax::Bool: result << i.first << " : " << std::boolalpha << i.second.asBool() << "\n"; break;
	  case Valuesax::Null: result << i.first << " : " << "null" << "\n"; break;
	  default: break;
	}
      }

      //print result (actually its calling to s3select for processing. the s3slect-object may contain zero matching key-values)
      std::cout << result.str() << std::endl;

      //once all key-values move into s3select(for further filtering and processing), it should be cleared
      this->emptyhandler();

      if (!end_of_stream && stream_buffer.getBytesLeft() < 100)//TODO this condition could be replaced. it also define the amount of data that should be copy
      {//the non processed bytes will be processed on next fetched chunk
	return;
      }

      // error message
      if(reader.HasParseError())  {
	rapidjson::ParseErrorCode c = reader.GetParseErrorCode();
	size_t o = reader.GetErrorOffset();
	std::cout << "PARSE ERROR " << c << " " << o << std::endl;
	break;
      }
    }//while reader.IterativeParseComplete
  }  

};

#endif
