#pragma once
#include "common/json/json_writer.h"
#include "common/json/json_reader.h"
#include "common/json/json_ostream.h"
#include "common/json/json_istream.h"

#include <iosfwd>
#include <string>

namespace json
{
// generate a string from json
std::string write(entry_writer& ga, bool indent = false);
std::string write(array_writer& ga, bool indent = false);
std::string write(generate_array& ga, bool indent = false);
std::string write(ostream& os, bool indent = false);
void write(generate_array& ga, std::string& arg, bool indent = false);
// wide char version
std::wstring wwrite(wentry_writer& ga, bool indent = false);
std::wstring wwrite(warray_writer& ga, bool indent = false);
std::wstring wwrite(wgenerate_array& ga, bool indent = false);
std::wstring wwrite(wostream& os, bool indent = false);

// write to out stream from json
bool write(std::ostream& to, entry_writer& ga, bool indent = false);
bool write(std::ostream& to, array_writer& ga, bool indent = false);
bool write(std::ostream& to, generate_array& ga, bool indent = false);

// wide char version
bool wwrite(std::wostream& to, wentry_writer& ga, bool indent = false);
bool wwrite(std::wostream& to, warray_writer& ga, bool indent = false);
bool wwrite(std::wostream& to, wgenerate_array& ga, bool indent = false);

// read json into property tree
bool read(std::istream& from, boost::property_tree::ptree& pt);
bool read(std::wistream& from, boost::property_tree::wptree& pt);
bool read(const std::string& input, boost::property_tree::ptree& pt);
bool read(const std::wstring& input, boost::property_tree::wptree& pt);

std::string as_string(const json::istream& from);  // return the entries under the given istream objs as plain string

}   // namespace json

