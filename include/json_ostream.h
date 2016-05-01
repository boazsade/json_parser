#pragma once
#include "json_stream.h"
#include "json_base.h"
#include <boost/array.hpp>	// this become part of c++11
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/if.hpp>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <iostream>

namespace json
{

//template<typename Ch>
//struct this_type& basic_ostream;
// allow to add new node into the tree
struct _new
{
    _new(const char* n) : name(n)
    {
    }

    void create();

private:
    const char* name;
};

static const struct __end
{

} _end = __end();

struct _start
{
    _start(const char* n) : value(n)
    {

    }

    const char* value;
};

static const struct _pushe 
{
} _pushend = _pushe();

static const _pushe _empty = _pushe();

struct _push 
{
    _push(const char* n = "") : name(n)
    {

    }

    const char* name;
};

static const _push _array = _push();

template<typename T>
struct c_array
{
	typedef T	value_type;
	typedef T*	pointer_type;
	typedef const T* const_pointer_type;

	//template<std::size_t A>
	c_array(const T* a_name, std::size_t A) : from(a_name), to(a_name+A)
	{
	}

	const_pointer_type begin() const{
		return from;
	}
	const_pointer_type end() const {
		return to;
	}

	std::size_t size() const {
		return to - from;
	}

	bool empty() const {
		return from == empty;
	}

	const_pointer_type from;
	const_pointer_type to;
};


template<typename T, std::size_t A> inline
c_array<T>	make_carray(const T (&a)[A]) {
	return c_array<T>(a, A);
}

struct null_entry {
};

template<typename Ch> inline
std::basic_ostream<Ch>& operator << (std::basic_ostream<Ch>& stream, const null_entry&)
{
	return stream<<"null";
}

namespace detail {
template<typename P, typename T>
struct handle_pointer;
}


template<typename Ch>
struct basic_ostream : json_stream
{
    typedef typename ptree_type<Ch>::proptree_type  proptree_type;
    typedef typename ptree_type<Ch>::char_type      char_type;
    typedef basic_ostream<char_type>                this_type;

    basic_ostream(proptree_type& p, this_type* prt = 0) : pt(p), parent(prt)
    {

    }


    this_type& operator ^ (const _name& n)
    {
        this->set(n);
        return *this;
    }

    // overload of the insert operation 
    // note that each call with this would place a new entry to the 
    // tree, note that noramly this would assumes a new entry 
    // as a name, value pair, unless set as array
    this_type& operator ^ (int val)
    {
        return this->insert<int>(val);
    }

    this_type& operator ^ (unsigned int val)
    {
        return this->insert<unsigned int>(val);
    }

    this_type& operator ^ (short val)
    {
        return this->insert<short>(val);
    }

    this_type& operator ^ (unsigned short val)
    {
        return this->insert<unsigned short>(val);
    }

    this_type& operator ^ (char val)
    {
        return this->insert<char>(val);
    }

    this_type& operator ^ (unsigned char val)
    {
        return this->insert<unsigned char>(val);
    }

    this_type& operator ^ (double val)
    {
        return this->insert<double>(val);
    }

    this_type& operator ^ (float val)
    {
        return this->insert<float>(val);
    }

	this_type& operator ^ (long val)
    {
        return this->insert<long>(val);
    }

	this_type& operator ^ (unsigned long val)
    {
        return this->insert<unsigned long>(val);
    }

	this_type& operator ^ (long long val)
    {
        return this->insert<long long>(val);
    }

	this_type& operator ^ (unsigned long long val)
    {
        return this->insert<unsigned long long>(val);
    }

    this_type& operator ^ (bool val)
    {
        return this->insert<bool>(val);
    }

    this_type& operator ^ (const sub_tree& st)
	{
    	return this->insert<sub_tree>(st);
	}

    this_type& operator ^ (const null_entry& ne)
	{
    	return this->insert<null_entry>(ne);
	}

    template<typename P>
    this_type& operator ^ (const P* ptr)
    {
    	typedef detail::handle_pointer<P, this_type>	handler_t;
    	return handler_t::process(ptr, *this);
    }

    template<size_t N>
    this_type& operator ^ (const char_type cstring[N])
    {
    	typedef detail::handle_pointer<char_type, this_type>	handler_t;
    	return handler_t::process(cstring, *this);
    }

    // overload for vector and list
    // please note this would not work for anything that 
    // is not overloaded here, and would only work for collection where
    // we don't have a name for the value!
    template<typename T, typename A>
    this_type& operator ^ (const std::vector<T, A>& vec)
    {
    	return this->range_add(vec.begin(), vec.end());
    }

    template<typename T, typename A>
    this_type& operator ^ (const std::list<T, A>& vec)
    {
    	return this->range_add(vec.begin(), vec.end());
    }

    template<typename T, typename C, typename A>
    this_type& operator ^ (const std::set<T, C, A>& vec)
    {
    	return this->range_add(vec.begin(), vec.end());
    }

    template<typename T>
    this_type& operator ^ (const c_array<T>&  arr)
    {
    	return this->range_add(arr.begin(), arr.end());
    }

    template<typename T, std::size_t S>
    this_type& operator ^ (const boost::array<T, S>& arr)
    {
    	return this->range_add(arr.begin(), arr.end());
    }

    template<typename T>
    this_type& operator ^ (const std::pair<T*, T*>& range)
    {
    	return this->range_add(range.first, range.second);
    }

    this_type& operator ^ (const std::string& s)
    {
        return this->insert<std::string>(s);
    }

    this_type operator ^ (const _push& a)
    {
        return sub_element(a.name, "");
    }

    this_type operator ^ (const  _start& n)
    {
        return sub_element(n.value);
    }

    this_type& operator ^ (const _pushe&)
    {
        if (this->good() && parent) {

            parent->entries().push_back(std::make_pair("", pt));
            return *parent;
        }
        return *this;
    }

    this_type& operator ^ (const __end&)
    {
        if (this->good() && parent) {
            const char* name = "";
            if (parent->element_name()) {
                name = parent->element_name();
            }

            parent->entries().add_child(name, pt);
            childs.reset((proptree_type*)0);
            return *parent;
        }
        return *this;
    }

    proptree_type& entries()
    {
        return pt;
    }

    const proptree_type& entries() const
    {
        return pt;
    }


private:

    template<typename Iter>
    this_type& range_add(Iter from, Iter to)
    {
    	if (from != to && this->good()) {
    		while (from != to) {
    			*this ^ _array  ^ _name("") ^ *from ^ _pushend;
    			++from;
    		}
    	} else {
    		 *this ^ _array ^ _pushend;
    	}
    	return *this;
    }


    template<typename T>
    this_type& insert(const T& val)
    {
        BOOST_STATIC_ASSERT(details::check_legal_value<T>::value);
        if (this->good() && this->element_name()) {
            entry<T> e(this->element_name(), val);
            e.write(pt);
        }
        this->reset();
        return *this;
    }

    this_type sub_element(const char* pname, const char* cname = 0)
    {
        childs.reset(new proptree_type);
        this_type child(*childs, this);
        if (cname) {
            child.set(cname);
        }
        this->set(pname); 
        return child;
    }

private:
    proptree_type& pt;
    boost::shared_ptr<proptree_type> childs;
    this_type* parent;
};

namespace detail {
template<typename P, typename T>
struct handle_pointer {
	static T& process(const P* ptr, T& _this)
	{
		if (ptr) {
			return _this ^ *ptr;
		} else {
			static const null_entry ne = null_entry();
			return _this ^ ne;
		}
	}
};

template<typename T>
struct handle_pointer<typename T::char_type, T> {
	static T& process(const typename T::char_type* cstr, T& _this)
	{
		if (cstr) {
			return _this ^ std::basic_string<typename T::char_type>(cstr);
		} else {
			static const null_entry ne = null_entry();
			return _this ^ ne;
		}
	}
};

}	// end of namespace detail


typedef basic_ostream<char> ostream;
typedef basic_ostream<wchar_t> wostream;

}   // end of namespace json

