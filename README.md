# json_parser
This project would enable the use of json format messages to be parsed directly into C/C++ data structures
This is based on (boost property tree)[https://www.boost.org/doc/libs/1_85_0/doc/html/property_tree.html], so you would need to have get boost on the host where you would use this library.
## Design Overview
This is not intended for JSON manipulation through C++, i.e. if you would like to load JSON into memory then access elements from the parsed JSON, this is not what it does.
This however should make it simple to read JSON directly into C++ objects and to convert C++ objects back into JSON match like [Python](https://docs.python.org/3/library/json.html) or [Rust serde](https://docs.rs/serde_json/latest/serde_json/).
Unlike Python or Rust, where reflection exists, C++ which luck it, require that we will write a conversion operator from and to JSON.
To this end, you would need to implement `operator ^` for both reading (istream) and writing (ostream). This flow to same pattern as the iostream operators in C++. Please note that order to make it simpler to implement and take away the burden of manually writing these operators, this uses [Boost Fusion](https://www.boost.org/doc/libs/1_85_0/libs/fusion/doc/html/fusion/adapted/adapt_struct.html) to mimic the reflection operation that we have in other languages, but still we don't have good macro support as in Rust to bypass auto reflection into and out of JSON. So in it simpler form you would still need to inform what are the JSON labels as well as manually calling the serializing/deserializing functions.
### Note of Performance
The  (boost property tree)[https://www.boost.org/doc/libs/1_85_0/doc/html/property_tree.html] is very bad in term of performance as well as in memory management, please don't use this where need to need to handle large JSON inputs or outputs or/and where you handle large volume of data in term of throughput.

### Note about stl
This library tries to support build in stl data types such as `vector`, `set`, `list`, `map`. Were all but `map` will be converted to JSON array, and map into JSON dictionary. Please note that the conversion is automatic, so you don't need to implement it for your vectors, but if the vector do contain you own data types, you would need to implement `operator ^` them just like `operator <<` and ` operator >>` in C++.

## Build
This library is using [conan](https://docs.conan.io/2/index.html) and [cmake](https://cmake.org/) for its builds.
You can build this without `conan` using just cmake, but in this case, make sure to install (boost)[https://www.boost.org/] on you build machine.
> Note that you depends on the existing matching releases on conan repository, so the first time installation can be long if there are no exact matching to the packages of boost on conan repository.
### Linux build
When building with `conan` on `Linux`, you have a script that automate the installation dependencies:
```bash
./instal_conan.sh -i -a
```
For first time installed, and whenever you are removing the `build` directory, you can run
```bash
./instal_conan.sh  -a
```
> note that this for all major three build type options: `debug`, `release` and `debug with release info`.
To run the actual build from the command line:
```bash
cmake --preset conan-release
cmake --preset conan-debug
cmake --preset conan-relwithdebInfo
```
To generate the cmake "project" files - in this case `Makefile`s under the `build` directory.
and then:
```bash
cmake build --preset conan-release
cmake build --preset conan-debug
cmake build --preset conan-relwithdebInfo
```
To build each of the build type setting.
> note that you can skip any one of the three above, depending on the build you would like to create.
### Building on Windows
Run the commands:
```bash
 conan install -s build_type=Debug .  --build=missing
 conan install -s build_type=Release .  --build=missing
 conan install -s build_type=RelWithDebInfo .  --build=missing
```
To generate the missing dependencies.
> note that this for all major three build type options: `debug`, `release` and `debug with release info`.
Then to do the actual build:
```bash
cmake --preset conan-default
cmake --build --preset conan-debug
cmake --build --preset conan-release
```


## Basic Usage
Say you have this data type:

```cpp
struct MyDataType {
  int a;
  double b;
  std::string s;
};
```
In order to convert this into JSON you would need to implement the out operation:
```cpp
json::ostream& operator ^ (json::ostream& s, const MyDataType& mdt) {
  return s ^ json::_name("a") ^ mdt.a ^ json::_name("b") ^ mdt.b ^ json::_name("a string") ^ mdt.s;
}
```
And the same is true for insertions - only in this case we would be using json::istream objects.

Note
-------------------
This library uses boost as a back end for the JSON parsing. In order to successfully work with the boost implementation you have to 
replace your a file that is found under boost_fix with an instructions of what needs to be done

The following is an example on how to create  JSON out of C++ data type

## From C++ to JSON
----------------------
A More elaborate example of converting form C++ into JSON:
> Note see bellow that we can make it that much simpler!

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
> note that the "second" entry is missing because we did not added anything under it
> note how this automatically handle pointers of different types

a more elaborated example include an internal list of compound elements (vector of struct)

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
> note that since the list of elements is of compound type you must create sub tree by doing:
```cpp
auto l = js ^ "struct_array"_s;
l ^ f.list ^ json::_end;
```
which tells it to create sub tree for the "struct_array" and then when done with it json::_end it!!
> note that std::vector, std::list, std::set std::array and normal C array are handle automatically (you don`t need to iterate over them)
> note that the "_n" and "_s" are literals that tells it that the literal preceding the "_n" for the key name (and not a value) and "_s" is the start of new sub tree in JSON

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

## From JSON to C++
This code has a function that handle the reading of JSON from stream (in this example its assuming a file, but any STL stream is a valid input). the function also handle exception - since in this case if we have missing entry in JSON or invalid type of entry or invalid JSON an exception is thrown

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
> Please note that in the case of arrays it is using an empty tag name for it.

We can make it simpler still:
As we all know and love Python, why not make it [Python like](https://docs.python.org/3/library/json.html)?
Assume we have 3 types: `foo` and `bar` and they are contained inside `baz`.
If we are to implement the `^` operators for reading and writing for all 3 of them, then our code will be:
```cpp
#include "json_parser/include/json_utils.h"

struct foo {/*lets not dwell into this path..*/};
struct bar {/* lets not dwell into it..*/};
struct baz { bar b; foo f};
static const char* LABELS[] = {
    "foo", "baz"
};

BOOST_FUSION_ADAPT_STRUCT(baz, (foo, f)(bar, b));
auto operator ^ (json::ostream& os, const baz& f) -> json::ostream& {
    return json::util::build_entry(os, f, &LABELS[0]);
}

auto operator ^ (json::istream& os, baz& f) -> json::istream& {
    return json::util::read_entry(os, f, &LABELS[0]);
}
// and then all we have to do, as the story goes
// There
const auto as_str{json::util::dumps(baz{})};
// And back
const auto b2{json::util::loads<baz>(as_str)};
```
Please take a look into the `examples` sub directory from more examples. You can build and run them as well to get feel for it.
