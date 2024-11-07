#pragma once

#include "json_ostream.h"
#include "json_istream.h"
#include <boost/fusion/adapted/struct.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/phoenix/phoenix.hpp>
#include <boost/mpl/size.hpp>
#include <span>
#include <cassert>
#include <optional>
#include <type_traits>
#include <iostream>

namespace json
{

namespace util
{

// This two function allow us to convert a given strucure into JSON structure
// You to
//  1. Adopt your structure into iterable strucure using BOOST_FUSION_ADAPT_STRUCT
//  2. make sure that have all your member supporting operator ^ to serialized the json or from json
//  3. call the bellow function by passing an array of labels for each strucure tag
//  for example:
//  given we have struture such as 
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

template<typename T>
struct is_opt_specialization : std::false_type {

};

template<typename T>
struct is_opt_specialization<std::optional<T>> {
    static constexpr bool value = is_specialization<T, std::vector>::value ||
        is_specialization<T, std::list>::value || 
        is_specialization<T, std::set>::value || 
        is_specialization<T, std::unordered_set>::value;
};

template<typename T> inline 
ostream& insert_list(ostream& to, const T& what, const char* label)
{
    auto i = to ^ _start(label);
    i ^ what ^ _end;
    return to;
}

template<typename T> inline 
ostream& insert_opt_list(ostream& to, const std::optional<T>& what, const char* label) {
    static const T empty_list = {};
    auto i = to ^ _start(label);
    if (what) {
        i ^ what ^ _end;
    } else {
        i ^ empty_list ^ _end;
    }
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
istream& extract_op_list(istream& with, std::optional<T>& to, const char* label) {
    with.set_op(true);
    auto er = with ^ json::_child(with, _name(label));
    T target;
    er ^ json::start_arr ^ target ^ json::end_arr;
    if (!target.empty()) {
        to = std::move(target);
    } else {
        to = std::nullopt;    // mark that we don't have it here
    }
    return with;
}

template<typename T> inline 
istream& extract_simple(istream& with, T& to, const char* label)
{
    with ^ _name(label) ^ to;
    return with;
}

// Generic operation to read the data in a given level of the JSON document,
// we are doing pattern matching on whether this is list or no list,
// so we can create the JSON entry correctly
template<typename T> inline
ostream& insert_to(ostream& to, const T& what, const char* label)
{
    using actual_type = T;
    if constexpr (is_specialization<actual_type, std::vector>::value) { 
        return insert_list(to, what, label);
    } else if constexpr (is_specialization<actual_type, std::list>::value) {
        return insert_list(to, what, label);
    } else if constexpr (is_specialization<actual_type, std::set>::value) {
        return insert_list(to, what, label);
    } else if constexpr (is_specialization<actual_type, std::unordered_set>::value) {
        return insert_list(to, what, label);
    } else {
        if constexpr (is_opt_specialization<actual_type>::value) {
        // this last case is for an optional value that holds a list.
        // We have an issue here, since we need to either save the contained value
        // or just create empty list!
            return insert_opt_list(to, what, label);
        }
    }
    return insert_simple(to, what, label);
}

// A generic data extraction from a JSON document.
// we are pattern matching on the type that need to be extract
// from the JSON document. WE care whether this is a list or not
// since this will detected on how to extract the data from JSON
// and into the type that we are filling.
template<typename T> inline
istream& extract_from(istream& with, T& to, const char* label)
{
    using actual_type = T;
    if constexpr (is_specialization<actual_type, std::vector>::value) { 
        return extract_list(with, to, label);
    } else if constexpr (is_specialization<actual_type, std::list>::value) {
        return extract_list(with, to, label);
    } else if constexpr (is_specialization<actual_type, std::set>::value) {
        return extract_list(with, to, label);
    } else if constexpr (is_specialization<actual_type, std::unordered_set>::value) {
        return extract_list(with, to, label);
    } else {
        if constexpr (is_opt_specialization<actual_type>::value) {
        // this is a little more subtle, we have a targe type that is an optional, but it holds
        // a container inside it that need to be fill.
        // we need to try and fill this container, but since this is optional, it may not exists
        // in the document as well..
            return extract_op_list(with, to, label);
        }
    }
    return extract_simple(with, to, label);
}


}   // end of namespace private_

template<typename T>
inline auto build_entry(ostream& js, const T& from, std::span<const char*> labels) -> ostream& {
    using boost::phoenix::arg_names::arg1;
    using namespace json::literals;

    assert(labels.size() == boost::mpl::size<T>::type::value);

    boost::fusion::for_each(from, [&js, start = labels.begin(), end = labels.end()](auto&& arg1) mutable {
            assert(start != end);      
            auto rr = js ^ _start(*start);
            rr ^ arg1 ^ _end;
            ++start;
        }
    );
    return js;
}

template<typename T>
inline auto read_entry(istream& js, T& to, std::span<const char*> labels) -> istream& {
    using boost::phoenix::arg_names::arg1;
    using namespace json::literals;

    assert(labels.size() == boost::mpl::size<T>::type::value);

    boost::fusion::for_each(to, [&js, start = labels.begin(), end = labels.end()] (auto&& arg1) mutable {
            assert(start != end);
            auto rr = js ^ _child(js, _name(*start));
            rr ^ arg1;
            ++start;
        }
    );
    return js;
}

template<typename T>
inline auto serialized(ostream& os, const T& from, std::span<const char*> labels) -> ostream& {
    using boost::phoenix::arg_names::arg1;
    using namespace json::literals;

    assert(labels.size() == boost::mpl::size<T>::type::value);

    boost::fusion::for_each(from, [&os, start = labels.begin(), end = labels.end()](auto&& arg1) mutable {
            assert(start != end);
            private_::insert_to(os, arg1, *start);
            ++start;
            return os;
        }
    );
    return os;
}

template<typename T>
inline auto deserialized(istream& os, T& from, std::span<const char*> labels) -> istream& {
    using boost::phoenix::arg_names::arg1;
    using namespace json::literals;

    assert(labels.size() == boost::mpl::size<T>::type::value);

    boost::fusion::for_each(from, [&os, start = labels.begin(), end = labels.end()](auto&& arg1) mutable {
            assert(start != end);
            private_::extract_from(os, arg1, *start);
            ++start;
            return os;
        }
    );
    return os;
}

// note that this requires that you would have `operator ^` implemented for T!
template<typename T>
inline auto into(const std::string& jstr) -> T {
    istream_root ir;
    ir ^ jstr;
    auto ip{ir ^ _root};
    T res;
    ip ^ res;
    return res;
}

// note that this requires that you would have `operator ^` implemented for T!
template<typename T>
inline auto to_string(const T& obj) -> std::string {
    output_stream root;
    auto parser{root ^ open};
    auto out{parser ^ obj ^ _end};
    return (root ^ json::str_cast);
}

// for those who like Python, this is an alias to the same loved Python API
template<typename T>
inline auto dumps(const T& obj) -> std::string {
    return to_string(obj);
}

template<typename T>
inline auto loads(const std::string& from) -> T {
    return into<T>(from);
}

}   // end of namespace util

}   // end of namespace json

