#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <fstream>
#include <chrono>

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

int main()
{

Timer t;
std::string stringFromStream;
std::ifstream in;
in.open("generated.json", std::ifstream::in);
if (in.is_open()) {
    std::string line;
    while (getline(in, line)) {
        stringFromStream.append(line + "\n");
    }
    in.close();
}

Document d;
d.Parse<0>(stringFromStream.c_str());
//d.ParseInsitu((char*)stringFromStream.c_str());

if (d.HasParseError()) {
	std::cout<<"parsing error"<< std::endl;
}

//assert(d["hello"].IsString()); 

StringBuffer buffer;
Writer<StringBuffer> writer(buffer);
d.Accept(writer);

std::cout<<"Time taken: " << t.elapsed() << " seconds\n";
 
std::cout << buffer.GetString() << std::endl;
return 0;
}