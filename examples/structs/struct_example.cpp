// This example show an easy way of creating json from C++ structures
// we are using iterations on the structures members so that we do
// not need to write a code to do it manually - in essence we are 
// using the structure as array
#include "json_istream.h"
#include "json_ostream.h"
#include "json_utils.h"
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

struct foo {
    int a = 1;
    int b = 2;
    short c = 3;
    std::string s = "hello";

    foo() = default; 
};

auto operator << (std::ostream& os, const foo& f) -> std::ostream& {
    return os << "a: " << f.a << ", b: " << f.b << ", c: " << f.c << ", s:" << std::quoted(f.s);
}

BOOST_FUSION_ADAPT_STRUCT(foo, (int, a)(int, b)(short, c)(std::string, s));

auto operator ^ (json::ostream& os, const foo& f) -> json::ostream& {
    static const char* LABELS[] = {
        "A", "B", "C", "S"
    };

    return json::util::serialized(os, f, &LABELS[0]);
}

auto operator ^ (json::istream& os, foo& f) -> json::istream& {
    static const char* LABELS[] = {
        "A", "B", "C", "S"
    };

    return json::util::deserialized(os, f, &LABELS[0]);
}

struct bar {
    std::vector<int> vals = {1, 2, 3, 4, 5};
    std::vector<double> dvals = { 1.1, 2.2, 3.3, 4.4};

    bar() = default;
};

auto operator << (std::ostream& os, const bar& b) -> std::ostream& {
    os << "vals: ";
    std::copy(std::begin(b.vals), std::end(b.vals), std::ostream_iterator<int>(os, ", "));
    os << "\ndvals: ";
    std::copy(std::begin(b.dvals), std::end(b.dvals), std::ostream_iterator<double>(os, ", "));
    return os;
}

BOOST_FUSION_ADAPT_STRUCT(bar, (std::vector<int>, vals)(std::vector<double>, dvals));
auto operator ^ (json::ostream& os, const bar& f) -> json::ostream& {
    static const char* LABELS[] = {
        "int values", "double values"
    };

    return json::util::serialized(os, f, &LABELS[0]);
}

auto operator ^ (json::istream& os, bar& f) -> json::istream& {
    static const char* LABELS[] = {
        "int values", "double values"
    };

    return json::util::deserialized(os, f, &LABELS[0]);
}

struct baz {
    foo f;
    bar b;
};

auto operator << (std::ostream& os, const baz& b) -> std::ostream& {
    return os << "foo: " << b.f << "\nbar: " << b.b;
}

BOOST_FUSION_ADAPT_STRUCT(baz, (foo, f)(bar, b));
auto operator ^ (json::ostream& os, const baz& f) -> json::ostream& {
    static const char* LABELS[] = {
        "foo", "baz"
    };

    return json::util::build_entry(os, f, &LABELS[0]);
}

auto operator ^ (json::istream& os, baz& f) -> json::istream& {
    static const char* LABELS[] = {
        "foo", "baz"
    };

    return json::util::read_entry(os, f, &LABELS[0]);
}




auto main() -> int {
    using namespace json::literals;
    try {
        baz b;
        json::output_stream root;
        auto parser{root ^ json::open};
        auto out{parser ^ b ^ json::_end};
        const auto js{(root ^ json::str_cast)};
        std::cout<<"baz as json\n" << js  << "\n";
        std::cout << "reading into C++: ";
        json::istream_root ir;      // the input stream instance that we will be using for populate from JSON into C++
        ir ^ js;                    // read from the string into our json stream
        auto ip{ir ^ json::_root};  // we need to have the root node in order to parse
        baz fj;                     // this instance will be filled with the stream
        ip ^ fj;                    // convert the json struct into our baz C++ object.
        std::cout << " baz is " << fj << "\n";  // and now we are ready

        std::cout << "Lef's make it even simpler!!\n";
        std::cout << "convert this to a string with JSON format\n";
        const auto as_str{json::util::to_string(baz{})};
        std::cout << "we have baz as json:\n" << std::quoted(as_str) << "\n";
        std::cout << "and we can read this back very easily:\n";
        const auto b2{json::util::into<baz>(as_str)};
        std::cout << "we successfully created baz from json as: " << b2 << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "failed to complete the task: got an error: " << e.what() << "\n";
        return -1;
    }
    return 0;
}

