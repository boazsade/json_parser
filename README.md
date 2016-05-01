# json_parser
This project would enable the use of json format messages to be parsed directly into C/C++ data structures
This is basedon the the boost property tree data type (so would must have a boost installation on you host in order to compile this).
The format is 
struct MyDataType {
  int a;
  double b;
  std::string s;
};

json::ostream& operator ^ (json::ostream& s, const MyDataType& mdt) {
  return s ^ json::_name("a") ^ mdt.a ^ json::_name("b") ^ mdt.b ^ json::_name("a string") ^ mdt.s;
}
the same is true for insertions - only in this case we would be using json::istream opjects
