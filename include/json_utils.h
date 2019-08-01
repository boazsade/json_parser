#pragma once
// Functions that can be used to help altivate some boilerplate code
#include "json_ostream.h"
#include "json_istream.h"
#include <boost/fusion/adapted/struct.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/phoenix/phoenix.hpp>
#include <type_traits>

namespace json
{

namespace util
{

// This two function allow us to convert a given strucure into JSON strucutre
// You to
//  1. Adopt your structure into iterable strcure using BOOST_FUSION_ADAPT_STRUCT
//  2. make sure that have all your member supporting operator ^ to serialized the json or from json
//  3. call the bellow function by passing an array of labels for each strucure tag
//  for example:
//  given we have strcture such as 
//  struct foo {
//      int a;
//      double d;
//      std::string s;
//  };
//  json::ostream& operator ^ (json::ostream& , const foo&);
//  json::istream& operator ^ (json::istream& , foo&);
//
//  struct bar {
//      short a;
//      float b;
//      std::vector<int> l;
//  };
//  json::ostream& operator ^ (json::ostream& , const bar&);
//  json::istream& operator ^ (json::istream& , bar&);
//
//  struct baz {
//      foo f;
//      bar b;
//  };
//  json::ostream& operator ^ (json::ostream& , const baz&);
//  json::istream& operator ^ (json::istream& , baz&);
//
// --> at the global namespace we then 
// BOOST_FUSION_ADAPT_STRUCT(baz, (foo, f)(bar, b));
// --> then we would have a list of tags somewhere
// const char* NAMES[] = {
//  "json-foo", "json-bar"
//  };
//
//  json::ostream& operator ^ (json::ostream& js, const baz& b) {
//      return json::util::to_json(js, b, &NAMES[0]);
//  }
//
//  json::istream& operator ^ (json::istream& js, baz& b) {
//      return json::util::from_json(js, b, &NAMES[0]);
//  }
//

namespace private_
{

template<typename Test, template<typename...> class Ref>
struct is_specialization : std::false_type {};

template<template<typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref>: std::true_type {};

template<typename T> inline 
ostream& insert_list(ostream& to, const T& what, const char* label)
{
    auto i = to ^ _start(label);
    i ^ what ^ _end;
    return to;
}

template<typename T> inline 
ostream& insert_simple(ostream& to, const T& what, const char* label)
{
    to ^ _name(label) ^ what;
    return to;
}

template<typename T> inline 
istream& extract_list(istream& with, T& to, const char* label)
{
    auto er = with ^ json::_child(with, _name(label)); 
    er ^ json::start_arr ^ to ^ json::end_arr; 
    return with;
}

template<typename T> inline 
istream& extract_simple(istream& with, T& to, const char* label)
{
    with ^ _name(label) ^ to;
    return with;
}

template<typename T> inline
ostream& insert_to(ostream& to, const T& what, const char* label)
{
    using actual_type = T;
    if constexpr (is_specialization<actual_type, std::vector>::value) { 
        return insert_list(to, what, label);
    } else {
        if constexpr (is_specialization<actual_type, std::list>::value) {
            return insert_list(to, what, label);
        } else {
            if constexpr (is_specialization<actual_type, std::set>::value) {
                return insert_list(to, what, label);
            }
        }
    }
            
    return insert_simple(to, what, label);
}

template<typename T> inline
istream& extract_from(istream& with, T& to, const char* label)
{
    using actual_type = T;
    if constexpr (is_specialization<actual_type, std::vector>::value) { 
        return extract_list(with, to, label);
    } else {
        if constexpr (is_specialization<actual_type, std::list>::value) {
            return extract_list(with, to, label);
        } else {
            if constexpr (is_specialization<actual_type, std::set>::value) {
                return extract_list(with, to, label);
            }
        }
    }
            
    return extract_simple(with, to, label);
}


}   // end of namespace private_

template<typename T>
inline ostream& build_entry(ostream& js, const T& from, const char** labels)
{
    using boost::phoenix::arg_names::arg1;
    using namespace json::literals;

    boost::fusion::for_each(from, [&js, &labels](auto&& arg1) {
            auto rr = js ^ _start(*labels);
            rr ^ arg1 ^ _end;
            ++labels;
        }
    );
    return js;
}

template<typename T>
inline istream& read_entry(istream& js, T& to, const char** labels)
{
    using boost::phoenix::arg_names::arg1;
    using namespace json::literals;

    boost::fusion::for_each(to, [&js, &labels](auto&& arg1) {
           auto rr = js ^ _child(js, _name(*labels));
            rr ^ arg1;
            ++labels;
        }
    );
    return js;
}

template<typename T>
inline ostream& serialized(ostream& os, const T& from, const char** labels)
{
    using boost::phoenix::arg_names::arg1;
    using namespace json::literals;

    boost::fusion::for_each(from, [&os, &labels](auto&& arg1) {
                private_::insert_to(os, arg1, *labels);
                ++labels;
                return os;
            }
    );
    return os;
}

template<typename T>
inline istream& deserialized(istream& os, T& from, const char** labels)
{
    using boost::phoenix::arg_names::arg1;
    using namespace json::literals;

    boost::fusion::for_each(from, [&os, &labels](auto&& arg1) {
                private_::extract_from(os, arg1, *labels);
                ++labels;
                return os;
            }
    );
    return os;
}

}   // end of namespace util

}   // end of namespace json

