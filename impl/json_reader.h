#pragma once

#include "json_base.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/mpl/if.hpp> // boost::mpl::if_c
#include <boost/type_traits/is_same.hpp>
#include <sstream>
#include <iostream>
#include <boost/foreach.hpp>
#include <optional>

namespace json
{

namespace details
{

template<typename T>
struct null_string;

template<>
struct null_string<char>
{
    static const char* get()
    {
        return "";
    }
};

template<>
struct null_string<wchar_t>
{
    static const wchar_t* get()
    {
        return L"";
    }
};

template<typename T>
struct special_chars;

template<>
struct special_chars<char>
{
    static char open_square()
    {
        return '[';
    }

    static char closing_square()
    {
        return ']';
    }

    static char dots()
    {
        return ':';
    }
};

template<>
struct special_chars<wchar_t>
{
    static wchar_t open_square()
    {
        return L'[';
    }

    static wchar_t closing_square()
    {
        return L']';
    }

    static wchar_t dots()
    {
        return L':';
    }
};

}   // end of namespace details

template<typename T>
struct array_reader
{
	T operator () (const char* name, boost::property_tree::ptree::value_type& entry) const
	{
		return entry.second.get<T>(name);
	}

	T operator () (const wchar_t* name, boost::property_tree::wptree::value_type& entry) const
	{
		return entry.second.get<T>(name);
	}
};

template<typename T>
struct reader
{
	T operator () (const char* name, boost::property_tree::ptree& pt) const
	{
		return pt.get<T>(name);
	}

	T operator () (const wchar_t* name, boost::property_tree::wptree& pt) const
	{
		return pt.get<T>(name);
	}
};

template<typename T>
struct opt_reader
{
	std::optional<T> operator () (const char* name, boost::property_tree::ptree& pt) const
	{
		auto v{pt.get_optional<T>(name)};
        return v ? std::optional<T>{std::move(v.value())} : std::nullopt;
	}

	std::optional<T> operator () (const wchar_t* name, boost::property_tree::wptree& pt) const
	{
        auto v{pt.get_optional<T>(name)};
		return v ? std::optional<T>{std::move(v.value())} : std::nullopt;
	}
};

template<typename T>
struct opt_array_reader
{
	T operator () (const char* name, boost::property_tree::ptree::value_type& entry) const
	{
		return entry.second.get<T>(name);
	}

	std::optional<T> operator () (const wchar_t* name, boost::property_tree::wptree::value_type& entry) const
	{
		auto v{entry.second.get_optional<T>(name)};
        return v ? std::optional<T>{std::move(v.value())} : std::nullopt;
	}
};

template<typename T, typename CharT = char>
struct single_entry : reader<T>
{
	typedef T	                         value_type;
    typedef CharT                        char_type;
    typedef std::basic_string<char_type> string_type;

    typedef typename boost::mpl::if_c<boost::is_same<char_type, char>::value,
                                 boost::property_tree::ptree,
                                 boost::property_tree::wptree>::type proptree_type;
	
	single_entry(const char_type* n, const T& default_val = T()) : name(n), value(default_val)
	{
	}

	bool read(proptree_type& pt)
	{
		try {
			value = this->operator()(name.c_str(), pt);
			return true;
		} catch (std::exception& e) {
			return false;
		}
	}

	string_type name;
	value_type  value;
};

template<typename T, typename CharT = char>
struct ref_single_entry : reader<T>
{
	typedef T	                         value_type;
    typedef CharT                        char_type;
    typedef std::basic_string<char_type> string_type;

    typedef typename boost::mpl::if_c<boost::is_same<char_type, char>::value,
                                 boost::property_tree::ptree,
                                 boost::property_tree::wptree>::type proptree_type;
	
	ref_single_entry(const char_type* n, T& default_val) : name(n), value(default_val)
	{
	}

	bool read(proptree_type& pt)
	{
		try {
			value = this->operator()(name.c_str(), pt);
			return true;
		} catch (std::exception& ) {
			return false;
		}
	}

	string_type name;
	value_type&  value;
};

template<typename T, typename CharT = char>
struct opt_single_entry : opt_reader<T>
{
	using value_type = T;
    using char_type = CharT;
    using string_type = std::basic_string<char_type>;

    using proptree_type = typename boost::mpl::if_c<boost::is_same<char_type, char>::value,
                                 boost::property_tree::ptree,
                                 boost::property_tree::wptree>::type;
	
	opt_single_entry(const char_type* n, std::optional<T>& default_val) : name(n), value(default_val)
	{
	}

	bool read(proptree_type& pt)
	{
		try {
			value = this->operator()(name.c_str(), pt);
			return true;
		} catch (std::exception& e) {
			return false;
		}
	}

	string_type                 name;
	std::optional<value_type>&  value;
};

template<typename T, typename CharT> inline
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& os, const single_entry<T, CharT>& entry)
{
	return os<<details::special_chars<CharT>::open_square()<<entry.name<<details::special_chars<CharT>::dots()<<entry.value<<details::special_chars<CharT>::closing_square();
}

template<typename T, typename CharT = char>
struct array_entry : array_reader<T>
{
	typedef T	value_type;
    typedef CharT char_type;
    typedef std::basic_string<char_type> string_type;
    typedef typename boost::mpl::if_c<boost::is_same<char_type, char>::value,
                                 boost::property_tree::ptree,
                                 boost::property_tree::wptree>::type proptree_type;
	
	array_entry(const char_type* n, const T& default_val = T()) : name(n), value(default_val)
	{
	}

	bool read(typename proptree_type::value_type& pt)
	{
		try {
			value = this->operator()(name.c_str(), pt);
			return true;
		} catch (std::exception&) {
			return false;
		}
	}

	string_type name;
	value_type value;
};

template<typename T, typename CharT = char>
struct ref_array_entry : array_reader<T>
{
	typedef T	value_type;
    typedef CharT char_type;
    typedef std::basic_string<char_type> string_type;
    typedef typename boost::mpl::if_c<boost::is_same<char_type, char>::value,
                                 boost::property_tree::ptree,
                                 boost::property_tree::wptree>::type proptree_type;
	
	ref_array_entry(const char_type* n, T& default_val) : name(n), value(default_val)
	{
	}

	bool read(typename proptree_type::value_type& pt)
	{
		try {
			value = this->operator()(name.c_str(), pt);
			return true;
		} catch (std::exception&) {
			return false;
		}
	}

	string_type name;
	value_type& value;
};

template<typename T, typename CharT = char>
struct opt_array_entry : opt_array_reader<T>
{
	using value_type = T	;
    using char_type = CharT ;
    using string_type = std::basic_string<char_type> ;
    using proptree_type = typename boost::mpl::if_c<boost::is_same<char_type, char>::value,
                                 boost::property_tree::ptree,
                                 boost::property_tree::wptree>::type ;
	
	opt_array_entry(const char_type* n, std::optional<T>& default_val) : name(n), value(default_val)
	{
	}

	bool read(typename proptree_type::value_type& pt)
	{
		try {
			value = this->operator()(name.c_str(), pt);
			return true;
		} catch (std::exception&) {
			return false;
		}
	}

	string_type                 name;
	std::optional<value_type>&  value;
};

template<typename T, typename CharT> inline
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& os, const array_entry<T, CharT>& entry)
{
	return os << details::special_chars<CharT>::open_square() << entry.name
        << details::special_chars<CharT>::dots() << entry.value << details::special_chars<CharT>::closing_square();
}

using int_array_entry = array_entry<int>;
using str_array_entry = array_entry<std::string>;
using fp_array_entry = array_entry<double>;
using bool_array_entry = array_entry<bool>;
using int_entry = single_entry<int>;
using str_entry = single_entry<std::string>;
using fp_entry = single_entry<double>;
using bool_entry = single_entry<bool>;

using opt_int_entry = opt_single_entry<int>;
using opt_str_entry = opt_single_entry<std::string>;
using opt_fp_entry = opt_single_entry<double>;
using opt_bool_entry = opt_single_entry<bool>;
using opt_int_array_entry = opt_array_entry<int>;
using opt_str_array_entry = opt_array_entry<std::string>;
using opt_fp_array_entry = opt_array_entry<double>;
using opt_bool_array_entry = opt_array_entry<bool>;
////////////////////
// wide version
using wint_array_entry = array_entry<int, wchar_t>;
using wstr_array_entry = array_entry<std::wstring, wchar_t>;
using wfp_array_entry = array_entry<double, wchar_t>;
using wbool_array_entry = array_entry<bool, wchar_t>;
using wint_entry = single_entry<int, wchar_t>;
using wstr_entry = single_entry<std::wstring, wchar_t>;
using wfp_entry = single_entry<double, wchar_t>;
using wbool_entry = single_entry<bool, wchar_t>;

template<typename T, typename CharT = char>
struct values_list
{
private:
    using data_type = std::vector<T> ;
    using char_type = CharT;
    using string_type = std::basic_string<char_type>;
    using proptree_type = typename boost::mpl::if_c<boost::is_same<char_type, char>::value,
                                 boost::property_tree::ptree,
                                 boost::property_tree::wptree>::type ;

public:
    using const_iterator = typename data_type::const_iterator;

    using value_type = T;

    explicit values_list(const string_type& input = string_type(), const char_type* root = details::null_string<char_type>::get())
    {
        if (!input.empty()) {
            read(input, root);
        }
    }

    bool read(const string_type& input, const char_type* root = details::null_string<char_type>::get())
    {
        data.clear();

        proptree_type pt;

        if (!input.empty()) {
            if (this->read(input, pt)) {
                BOOST_FOREACH(typename proptree_type::value_type &v, pt.get_child(root)) {
                    array_entry<value_type, char_type> elem(details::null_string<char_type>::get());
                    bool ret = elem.read(v);
                    if (ret) {
                        data.push_back(elem.value);
                    }
                }
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    const T& operator [] (std::size_t index) const
    {
        return data.at(index);
    }

    const_iterator begin() const
    {
        return data.begin();
    }

    const_iterator end() const
    {
        return data.end();
    }

    bool empty() const
    {
        return data.empty();
    }

    std::size_t size() const
    {
        return data.size();
    }

private:
    bool read(const std::string& , proptree_type& pt)
    {
        std::basic_istringstream<char_type> buffer;
        try {
            read_json(buffer, pt);
            return true;
        } catch (const std::runtime_error&) {
            return false;
        }
    }

private:
    data_type   data;
};

// read json into property tree
bool read(std::istream& from, boost::property_tree::ptree& pt);
bool read(std::wistream& from, boost::property_tree::wptree& pt);
bool read(const std::string& input, boost::property_tree::ptree& pt);
bool read(const std::wstring& input, boost::property_tree::wptree& pt);

}   // end of namespace json
