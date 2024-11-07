#include "json_istream.h"
#include "json_ostream.h"
#include "json_utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>

bool report_error(const std::string& err)
{
    std::cerr<<"error: "<<err<<std::endl;
    return false;
}

struct message_header 
{ 
    using reply_list = std::vector<std::string>;

    std::string field;
    std::string solution;   
    std::string name;       
    std::string uuid;
    reply_list  expected_replies;

};

std::ostream& operator << (std::ostream& os, const message_header& hdr)
{
    os<<"field: "<<hdr.field<<", solution: "<<hdr.solution<<", name: "<<hdr.name<<", uuid: "<<hdr.uuid;
    if (!hdr.expected_replies.empty()) {
        std::copy(std::begin(hdr.expected_replies), std::end(hdr.expected_replies), std::ostream_iterator<std::string>(os, ","));
    }
    return os;
}

json::istream& operator ^ (json::istream& os, message_header& hdr) 
{ 
    using namespace json::literals; 
    os ^ "solution_type"_n ^ hdr.field ^ "solution_sub_type"_n ^ hdr.solution ^ "message_type"_n ^ hdr.name ^ "uuid"_n ^ hdr.uuid; 
    auto er = os ^ json::_child(os, "reply_type"_n); 
    er ^ json::start_arr ^ hdr.expected_replies ^ json::end_arr; 
    return os; 
}

bool get_header(const std::string& input, message_header& output) 
{
    try { 
        json::istream_root root; 
        root ^ input; 
        auto base_node = root ^ json::_root; 
        // read the message "header" - note that you need to tell where the 
        // sub node is starting - the name of it 
        auto header = base_node ^ json::_child(base_node, json::_name("title")); 
        header ^ output; return true; 
    } catch (const std::runtime_error& err) { 
        return report_error(err.what()); 
    } catch (...) { 
        return report_error("failed to parse message header"); 
    }
    return true;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <input json file\n";
        return -1;
    } 
    std::cout << "reading json from " << argv[1] << std::endl;
    std::ostringstream buffer;
    std::ifstream input(argv[1]);
    if (input) {
        buffer << input.rdbuf();
        message_header hdr;
        if (get_header(buffer.str(), hdr)) {
            std::cout << "successfully read header " << hdr << " from input file" << std::endl;
        } else {
            std::cerr << "failed to successfully read header from input file\n";
            return -3;
        }
    } else {
        std::cerr << "failed to open " << argv[1] << " for reading\n";
        return -2;
    }
}

