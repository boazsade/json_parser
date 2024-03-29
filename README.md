# json_parser
This project would enable the use of json format messages to be parsed directly into C/C++ data structures
This is based on the the boost property tree data type (so would must have a boost installation on you host in order to compile this).
The format is 

```cpp
struct MyDataType {
  int a;
  double b;
  std::string s;
};

json::ostream& operator ^ (json::ostream& s, const MyDataType& mdt) {
  return s ^ json::_name("a") ^ mdt.a ^ json::_name("b") ^ mdt.b ^ json::_name("a string") ^ mdt.s;
}
```
the same is true for insertions - only in this case we would be using json::istream opjects

Note
-------------------
This library uses boost as a back end for the JSON parsing. In order to successfully work with the boost implementation you have to 
replace your a file that is found under boost_fix with an instructions of what needs to be done

The following is an example on how to create  JOSN out of C++ data type

Create JSON
----------------------

```cpp
#include "json_parser/json_ostream.h"
#include "json_parser/json_writer.h"
#include "json_parser/jsonfwrd.h"
#include <string>
#include <iostream>

const char* empty = "";

struct foo {
    int a = 0;
    int b = 0;
    int c = 0;
    std::string s;
    double* dp = nullptr;
    const char* cp =empty;
};

std::ostream& operator << (std::ostream& os, const foo& f) {
    return os <<"a = "<<f.a<<", b = "<<f.b<<", c = "<<f.c<<" s = '"<<f.s<<"'"<<" dp = "<<(f.dp ? *f.dp : 0.0);
}

json::ostream& operator ^ (json::ostream& js, const foo& f) {
    using namespace json::literals;
    js ^ "a"_n ^ f.a ^ "b"_n ^ f.b ^ "c"_n ^ f.c ^ "s"_n ^ f.s ^ "dp"_n ^ f.dp ^ "cp"_n ^ f.cp;
    return js;
}

int main() {
    using namespace json::literals;

    foo f;

    
    json::output_stream root;
    auto parser = root ^ json::open;
    auto s = parser ^ "start"_s;
    s ^ f ^ json::_end;
    auto s2 = parser ^ "second"_s;
    s2 ^ json::_end;
    auto msg = root ^ json::str_cast;
    std::cout<<"message:\n"<<msg<<std::endl;
}
```
And the output in this case would be 
~~~~
{
	"start": {
		"a": 0,
		"b": 0,
		"c": 0,
		"s": "",
		"dp": null,
		"cp": ""
	}
}
~~~~
note that the "second" entry is missing because we did not added anything under it
note how this automatically handle pointers of different types

a more elaborated example include an internal list of compound elements (vector of structs)

```cpp
#include "libs/json_parser/json_ostream.h"
#include "libs/json_parser/json_writer.h"
#include "libs/json_parser/jsonfwrd.h"
#include <vector>
#include <string>

#include <iostream>
const char* empty = "";

struct bar {
    int tt = 12;
    std::string baz;

};

json::ostream& operator ^ (json::ostream& js, const bar& b) {
    using namespace json::literals;
    js ^ "int_arg"_n ^ b.tt ^ "string_arg"_n ^ b.baz;
    return js;
}

struct foo {
    int a = 0;
    int b = 0;
    int c = 0;
    std::string s;
    double* dp = nullptr;
    const char* cp =empty;
    std::vector<bar> list;
};

std::ostream& operator << (std::ostream& os, const foo& f) {
    return os <<"a = "<<f.a<<", b = "<<f.b<<", c = "<<f.c<<" s = '"<<f.s<<"'"<<" dp = "<<(f.dp ? *f.dp : 0.0);
}

json::ostream& operator ^ (json::ostream& js, const foo& f) {
    using namespace json::literals;
    js ^ "a"_n ^ f.a ^ "b"_n ^ f.b ^ "c"_n ^ f.c ^ "s"_n ^ f.s ^ "dp"_n ^ f.dp ^ "cp"_n ^ f.cp;
   auto l = js ^ "struct_array"_s;
    l ^ f.list ^ json::_end;
    return js;
}

int main() {
    using namespace json::literals;

    foo f;
    const bar b[] = {
        {234, std::string("jjj")},
        {987, std::string("iuywieru")}
    };
    f.list.insert(f.list.begin(), std::begin(b), std::end(b));

    
    json::output_stream root;
    auto parser = root ^ json::open;
    auto s = parser ^ "start"_s;
    s ^ f ^ json::_end;
    auto s2 = parser ^ "second"_s;
    s2 ^ json::_end;
    auto msg = root ^ json::str_cast;
    std::cout<<"message:\n"<<msg<<std::endl;
}
```
and the output is:
~~~~
{
	"start": {
		"a": 0,
		"b": 0,
		"c": 0,
		"s": "",
		"dp": null,
		"cp": "",
		"struct_array": [{
			"int_arg": 234,
			"string_arg": "jjj"
		}, {
			"int_arg": 987,
			"string_arg": "iuywieru"
		}]
	}
}
~~~~
note that since the list of elements is of compund type you must create sub tree by doing:
auto l = js ^ "struct_array"_s;
l ^ f.list ^ json::_end;
which tells it to create sub tree for the "struct_array" and then when done with it json::_end it!!
note that std::vector, std::list, std::set std::array and normal C array are handle automatically (you dont need to iterate over them)
note that the "_n" and "_s" are literals that tells it that the literal preceding the "_n" for the key name (and not a value) and "_s" is the start of new sub tree in JSON

the last "write" example is with a simple array - again array is sub element so you need to create it then pass to the streaming operation - only in this case you have a direct transaction without the need to write manual operator

```cpp
#include "libs/json_parser/json_ostream.h"
#include "libs/json_parser/json_writer.h"
#include "libs/json_parser/jsonfwrd.h"
#include <vector>
#include <string>

#include <iostream>
const char* empty = "";

struct bar {
    int tt = 12;
    std::string baz;

};

json::ostream& operator ^ (json::ostream& js, const bar& b) {
    using namespace json::literals;
    js ^ "int_arg"_n ^ b.tt ^ "string_arg"_n ^ b.baz;
    return js;
}

struct foo {
    int a = 0;
    int b = 0;
    int c = 0;
    std::string s;
    double* dp = nullptr;
    const char* cp =empty;
    std::vector<bar> list;
    std::list<int> simple_list;
};

std::ostream& operator << (std::ostream& os, const foo& f) {
    return os <<"a = "<<f.a<<", b = "<<f.b<<", c = "<<f.c<<" s = '"<<f.s<<"'"<<" dp = "<<(f.dp ? *f.dp : 0.0);
}

json::ostream& operator ^ (json::ostream& js, const foo& f) {
    using namespace json::literals;
    js ^ "a"_n ^ f.a ^ "b"_n ^ f.b ^ "c"_n ^ f.c ^ "s"_n ^ f.s ^ "dp"_n ^ f.dp ^ "cp"_n ^ f.cp;
    auto sl = js ^ "simle_list"_s;
    sl ^ f.simple_list ^ json::_end;
   auto l = js ^ "struct_array"_s;
    l ^ f.list ^ json::_end;
    return js;
}

int main() {
    using namespace json::literals;

    foo f;
    const bar b[] = {
        {234, std::string("jjj")},
        {987, std::string("iuywieru")}
    };
    f.list.insert(f.list.begin(), std::begin(b), std::end(b));
    f.simple_list.push_back(1);
    f.simple_list.push_back(17);

    
    json::output_stream root;
    auto parser = root ^ json::open;
    auto s = parser ^ "start"_s;
    s ^ f ^ json::_end;
    auto s2 = parser ^ "second"_s;
    s2 ^ json::_end;
    auto msg = root ^ json::str_cast;
    std::cout<<"message:\n"<<msg<<std::endl;
}
```
and the output is 
~~~~
{
	"start": {
		"a": 0,
		"b": 0,
		"c": 0,
		"s": "",
		"dp": null,
		"cp": "",
		"simle_list": [1, 17],
		"struct_array": [{
			"int_arg": 234,
			"string_arg": "jjj"
		}, {
			"int_arg": 987,
			"string_arg": "iuywieru"
		}]
	}
}

~~~~
READING JSON
This code has a function that handle the reading of JSON from stream (in this example its assuming a file, but any STL stream is a valid input). the function also handle examption - since in this case if we have missing entry in JSON or invalid type of enty or invalid JSON an exception is thrown

```cpp
#include "libs/json_parser/json_istream.h"
#include <boost/filesystem.hpp>
#include <string>

struct message_header
{
    using reply_list = std::vector<std::string>;

    std::string field;
    std::string solution;   
    std::string name;       
    std::string uuid;
    reply_list  expected_replies;
};

json::istream& operator ^ (json::istream& os, message_header& hdr)
{
    using namespace json::literals;
    os ^ "solution_type"_n ^ hdr.field ^ "solution_sub_type"_n ^
        hdr.solution ^ "message_type"_n ^ hdr.name ^ "uuid"_n ^ hdr.uuid;
    auto er = os ^ json::_child(os, "reply_type"_n);
    er ^ json::start_arr ^ hdr.expected_replies ^ json::end_arr;
    return os;
}


bool json_message::get_header(const std::string& input, message_header& output) {
  try {
        json::istream_root root;
        root ^ input;
        auto base_node = root ^ json::_root;
        // read the message "header" - note that you need to tell where the 
        // sub node is starting - the name of it
        auto header = base_node ^ json::_child(base_node, json::_name("title"));
        header ^ output;
        return true;
    } catch (const std::runtime_error& err) {
        return report_error(err.what());
    } catch (...) {
        return report_error("failed to parse message header");
    }    
}
```
the json that is an input to the function is
~~~~
{
    "title": {
        "solution_type": "solution name",
        "solution_sub_type": "sub type solution",
        "message_type": "message type",
        "uuid": "9817234"
    }
}
note that the list of "reply_type" is optional and so we are not failing if we are not reading it!
~~~~
```cpp
// Yet another option to read from json with less explicit code
#include <iostream>
#include "json_parser/include/json_ostream.h"
#include "json_parser/include/json_utils.h"
#include <string>
#include <vector>
#include <map>

struct foo {
	int a{1};
	double b{1.1};
	std::string s{"hello"};
};

BOOST_FUSION_ADAPT_STRUCT(foo, (int, a)(double, b)(std::string, s));
json::istream& operator ^ (json::istream& os,  foo& f)
{
    static const char* LABELS[] = {
        "a", "b", "s"
    };

    return json::util::deserialized(os, f, &LABELS[0]);
}
std::ostream& operator << (std::ostream& os, const foo& f) {
	return os<<"{a: "<<f.a<<", b: "<<f.b<<", s: "<<f.s<<"}";
}
struct bar {
	std::vector<foo> f;
};
BOOST_FUSION_ADAPT_STRUCT(bar, (std::vector<foo>, f));
json::istream& operator ^ (json::istream& with,  bar& b)
{
    static const char* LABELS[] = {
        ""
    };

    return json::util::deserialized(with, b, &LABELS[0]);
}
std::ostream& operator << (std::ostream& os, const bar& f) {
        os<<" foo: [\n";
        for (const auto& i: f.f) {
                os<<"\t"<<i<<",\n";
        }
        return os<<"]";
}
template<typename Stream>
auto from_json(Stream input) -> bar {
	bar b;
        json::istream_root root;
        root ^ input;
	std::cout<<"successfully parsed the input "<<std::endl;
        auto base_node = root ^ json::_root;
	std::cout<<"successfully read the base input"<<std::endl;
	auto header = base_node ^ json::_child(base_node, json::_name("title"));
	std::cout<<"successfully read the title"<<std::endl;
	header ^ b;
	return b; 
}

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr<<"usage: <input file>\n";
		return -1;
	}
	std::ifstream input{argv[1]};
	if (input) {
		std::cout<<"successfully read input file"<<std::endl;
		std::ostringstream buffer;
		buffer << input.rdbuf();
		try {
			const auto b{from_json(std::move(buffer.str()))};
			std::cout<<"successfully read from json "<<b<<std::endl;
		} catch (const std::exception& e) {
			std::cerr<<"failed to process: "<<e.what()<<std::endl;
		}
	} else {
		std::cerr<<"failed to open "<<argv[1]<<" for reading\n";
		return -1;
	}
}
```
Use the this json input to test this code
```json
{
	"title": [{
		"a": 1,
		"b": 1.1111,
		"s": "hello"
	},
	{
		"a": 1444,
		"b": 121.23411,
		"s": "world"
	}]
}
```
Please note that in the case of arrays it is using an empty tag name for it.
