#pragma once
namespace json
{

template<typename Ch>
struct basic_istream;

template<typename Ch>
struct basic_ostream;

template<typename Ch>
struct basic_output_stream;

template<typename Ch>
class basic_istream_root;

// aliases
using istream = basic_istream<char>;
using wistream = basic_istream<wchar_t>;
using ostream = basic_ostream<char>;
using wostream = basic_ostream<wchar_t>;
using output_stream = basic_output_stream<char>;
using woutput_stream = basic_output_stream<wchar_t>;
using istream_root = basic_istream_root<char>;
using wistream_root = basic_istream_root<wchar_t>;

}   // end of namespace json

