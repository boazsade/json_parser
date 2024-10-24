// ----------------------------------------------------------------------------
// Copyright (C) 2002-2006 Marcin Kalicinski
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see www.boost.org
// ----------------------------------------------------------------------------
#ifndef BOOST_PROPERTY_TREE_DETAIL_JSON_PARSER_WRITE_HPP_INCLUDED
#define BOOST_PROPERTY_TREE_DETAIL_JSON_PARSER_WRITE_HPP_INCLUDED

#include <boost/property_tree/ptree.hpp>
#include <boost/next_prior.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <cctype>   // for isxdigit
#include <cwctype>   // for iswxdigit
#include <string>
#include <ostream>
#include <iomanip>

namespace boost { namespace property_tree { namespace json_parser
{
    
    template<typename Ch>
    struct ch_overload
    {
        static bool test(Ch cha)
        {
            return (cha == 0x20 || cha == 0x21 || (cha >= 0x23 && cha <= 0x2E) ||
                (cha >= 0x30 && cha <= 0x5B) || (cha >= 0x5D));
        }
    };

    template<>
    struct ch_overload<char>
    {
        static bool test(char cha)
        {
            return (cha == 0x20 || cha == 0x21 || (cha >= 0x23 && cha <= 0x2E) ||
                (cha >= 0x30 && cha <= 0x5B) || (cha >= 0x5D));
        }
    };

    template<typename Ch> inline
    void escape_special(std::basic_string<Ch>& result, Ch cha)
    {
        result += Ch('\\');
        result += cha;
    }

    // default case to use for char and others - we assume that this wold work
    template<typename T>
    struct check_unicode_escape
    {
        template<typename It>
        static bool apply(It from, It to, T first_char)
        {
            while (from != to && std::isxdigit(*from)) {
                from++;
            }
            return from == to || *from == first_char;
        }
    };

    // spcial case for wchar_t
    template<>
    struct check_unicode_escape<wchar_t>
    {

        template<typename It>
        static bool apply(It from, It to, wchar_t first_char) 
        {
            while (from != to && std::iswxdigit(*from)) {
                from++;
            }
            return from == to || *from == first_char;
        }

    };

    template<typename Ch, typename It> inline
    void apply_backslash(It from, It to, Ch first_char, std::basic_string<Ch>& result)
    {
        // we have the u after backslash that can stands for unicode escape!
        if (*(from + sizeof(Ch)) != Ch('u')) {
            escape_special<Ch>(result, '\\');
        } else {
            ++from; // to pass the '\'
            if (check_unicode_escape<Ch>::apply(++from /* to pass the u */, to, first_char)) {
                result += Ch('\\'); // just leave single backslash, we don't need escape
            } else {
                escape_special<Ch>(result, '\\');
            }
        }
    }

    template<typename Ch> inline
    bool create_escapes_from(const std::basic_string<Ch>& s, std::basic_string<Ch>& result)
    {
        typedef ch_overload<Ch>                        range_check;
        typename std::basic_string<Ch>::const_iterator b = s.begin();
        typename std::basic_string<Ch>::const_iterator e = s.end();
        Ch st = *(s.begin());
	    size_t last = s.size() - 1;
	    size_t pos = 0;

        while (b != e)
        {
            // This assumes an ASCII superset. But so does everything in PTree.
            // We escape everything outside ASCII, because this code can't
            // handle high unicode characters.
            if (range_check::test(*b)) {
                result += *b;
            } else {
                switch (*b) {
                case Ch('\b'):
                    escape_special<Ch>(result, 'b');
                    break;
                case Ch('\f'):
                    escape_special<Ch>(result, 'f');
                    break;
                case Ch('\n'):
                    escape_special<Ch>(result, 'n');
                    break;
                case Ch('\r'):
                    escape_special<Ch>(result, 'r');
                    break;
                case Ch('\\'):
                    apply_backslash<Ch>(b, e, st, result);
                    break;
                /*case Ch('/'):
                    escape_special<Ch>(result, '/');
                    break;*/
                case Ch('\t'):
                    escape_special<Ch>(result, 't');
                    break;
                case Ch('"'):
                    if (pos != 0 && pos != last) {
                        result += Ch('\\');
                    }
                    result += Ch('"');
                    break;
                default:
					result += *b;
                    // this code use to try with unicode in a way that didn't really worked (see comment above)
                    // since we are using unicode correctly, this is not required any more - Boaz Sade 05/10/2014
                    {
#if 0
                        const char *hexdigits = "0123456789ABCDEF";
                        typedef typename make_unsigned<Ch>::type UCh;
                        unsigned long u = (std::min)(static_cast<unsigned long>(
                                                         static_cast<UCh>(*b)),
                                                     0xFFFFul);
                        int d1 = u / 4096; 
                        u -= d1 * 4096;
                        int d2 = u / 256; 
                        u -= d2 * 256;
                        int d3 = u / 16; 
                        u -= d3 * 16;
                        int d4 = u;
                        result += Ch('\\'); 
                        result += Ch('u');
                        result += Ch(hexdigits[d1]); 
                        result += Ch(hexdigits[d2]);
                        result += Ch(hexdigits[d3]); 
                        result += Ch(hexdigits[d4]);
#endif
                    }
                    break;
                }
            }
            
            ++b;
            ++pos;
        }
        return !result.empty();
    }

    // Create necessary escape sequences from illegal characters
    template<class Ch> inline
    std::basic_string<Ch> create_escapes(const std::basic_string<Ch> &s)
    {

        std::basic_string<Ch> result;
        if (!s.empty()) {
            create_escapes_from(s, result);
        }
        return result;
    }

    template<class Ptree>
    void write_json_helper(std::basic_ostream<typename Ptree::key_type::value_type> &stream, 
                           const Ptree &pt,
                           int indent, bool pretty)
    {

        typedef typename Ptree::key_type::value_type Ch;
        typedef typename std::basic_string<Ch> Str;

        // Value or object or array
        if (indent > 0 && pt.empty())
        {
            // Write value
            Str data = create_escapes(pt.template get_value<Str>());
            //stream << Ch('"') << data << Ch('"');
            stream << data;

        }
        else if (/*indent > 0 && */pt.count(Str()) == pt.size())
        {
            // Write array
            stream << Ch('[');
            if (pretty) stream << Ch('\n');
            typename Ptree::const_iterator it = pt.begin();
            for (; it != pt.end(); ++it)
            {
                if (pretty) stream << Str(4 * (indent + 1), Ch(' '));
                write_json_helper(stream, it->second, indent + 1, pretty);
                if (boost::next(it) != pt.end())
                    stream << Ch(',');
                if (pretty) stream << Ch('\n');
            }
            if (pretty) {
		stream << Str(4 * indent, Ch(' '));
            }
            stream << Ch(']');

        }
        else
        {
            // Write object
            stream << Ch('{');
            if (pretty) stream << Ch('\n');
            typename Ptree::const_iterator it = pt.begin();
            for (; it != pt.end(); ++it)
            {
                if (pretty) stream << Str(4 * (indent + 1), Ch(' '));
                stream << Ch('"') << create_escapes(it->first) << Ch('"') << Ch(':');
                if (pretty) {
                    stream << Ch(' ');
                }
                write_json_helper(stream, it->second, indent + 1, pretty);
                if (boost::next(it) != pt.end()) {
                    stream << Ch(',');
                }
                if (pretty) {
                   stream << Ch('\n');
                }
            }
            if (pretty) stream << Str(4 * indent, Ch(' '));
            stream << Ch('}');
        }

    }

    // Verify if ptree does not contain information that cannot be written to json
    template<class Ptree>
    bool verify_json(const Ptree &pt, int depth)
    {

        typedef typename Ptree::key_type::value_type Ch;
        typedef typename std::basic_string<Ch> Str;

        // Root ptree cannot have data
        if (depth == 0 && !pt.template get_value<Str>().empty())
            return false;
        
        // Ptree cannot have both children and data
        if (!pt.template get_value<Str>().empty() && !pt.empty())
            return false;

        // Check children
        typename Ptree::const_iterator it = pt.begin();
        for (; it != pt.end(); ++it)
            if (!verify_json(it->second, depth + 1))
                return false;

        // Success
        return true;

    }
    
    // Write ptree to json stream
    template<class Ptree>
    void write_json_internal(std::basic_ostream<typename Ptree::key_type::value_type> &stream, 
                             const Ptree &pt,
                             const std::string &filename,
                             bool pretty)
    {
        if (!verify_json(pt, 0))
            BOOST_PROPERTY_TREE_THROW(json_parser_error("ptree contains data that cannot be represented in JSON format", filename, 0));
        write_json_helper(stream, pt, 0, pretty);
        stream << std::endl;
        if (!stream.good())
            BOOST_PROPERTY_TREE_THROW(json_parser_error("write error", filename, 0));
    }

} } }

#endif
