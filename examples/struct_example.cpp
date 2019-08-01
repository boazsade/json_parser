// This exaple show an easy way of creating json from C++ strucutres
// we are using iterations on the structres members so that we do
// not need to write a code to do it manually - in esence we are 
// using the structure as array
#include "../include/json_ostream.h"
#include "../include/json_utils.h"
#include <string>
#include <vector>

struct foo
{
    int a = 1;
    int b = 2;
    short c = 3;
    std::string s = "hello";

    foo() = default; 
};

BOOST_FUSION_ADAPT_STRUCT(foo, (int, a)(int, b)(short, c)(std::string, s));

json::ostream& operator ^ (json::ostream& os, const foo& f)
{
    static const char* LABELS[] = {
        "A", "B", "C", "S"
    };

    return json::util::serialized(os, f, &LABELS[0]);
}

struct bar 
{
    std::vector<int> vals = {1, 2, 3, 4, 5};
    std::vector<double> dvals = { 1.1, 2.2, 3.3, 4.4};

    bar() = default;
};

BOOST_FUSION_ADAPT_STRUCT(bar, (std::vector<int>, vals)(std::vector<double>, dvals));
json::ostream& operator ^ (json::ostream& os, const bar& f)
{
    static const char* LABELS[] = {
        "int values", "double values"
    };

    return json::util::serialized(os, f, &LABELS[0]);
}

struct baz 
{
    foo f;
    bar b;
};

BOOST_FUSION_ADAPT_STRUCT(baz, (foo, f)(bar, b));
json::ostream& operator ^ (json::ostream& os, const baz& f)
{
    static const char* LABELS[] = {
        "foo", "baz"
    };

    return json::util::build_entry(os, f, &LABELS[0]);
}


int main()
{
    using namespace json::literals;

    baz b;
    json::output_stream root;
    auto parser{root ^ json::open};
    auto out{parser ^ b ^ json::_end};
    std::cout<<"baz as json\n"<<(root ^ json::str_cast)<<"\n";
}

