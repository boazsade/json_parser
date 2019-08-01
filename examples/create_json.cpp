#include "json_ostream.h"
#include "json_writer.h"
#include "jsonfwrd.h"
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

    //json::ostream::proptree_type root;
    //json::ostream parser(root);
    json::output_stream root;
    auto parser = root ^ json::open;
    auto s = parser ^ "start"_s;
    s ^ f ^ json::_end;
    auto s2 = parser ^ "second"_s;
    s2 ^ json::_end;
    auto msg = root ^ json::str_cast;
    std::cout<<"message:\n"<<msg<<std::endl;
}
