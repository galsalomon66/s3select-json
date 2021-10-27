#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <fstream>

using namespace rapidjson;
using namespace std;

int main()
{

string stringFromStream;
ifstream in;
in.open("sample3.json", ifstream::in);
if (in.is_open()) {
    string line;
    while (getline(in, line)) {
        stringFromStream.append(line + "\n");
    }
    in.close();
}

Document d;
d.Parse<0>(stringFromStream.c_str());
//d.ParseInsitu((char*)stringFromStream.c_str());

if (d.HasParseError()) {
	cout<<"parsing error"<< endl;
}

assert(d["hello"].IsString()); 

StringBuffer buffer;
Writer<StringBuffer> writer(buffer);
d.Accept(writer);
 
cout << buffer.GetString() << endl;
return 0;
}