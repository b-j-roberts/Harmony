#include <iostream> // DEBUG ONLY

#include <sting>
#include <vector>

class Parameter {

  // TO DO : Think about name index of this param? maybe use a map?

  std::string value;

  // TO DO : Non-copyable?

public:

  // TO DO : Constructor

  int as_int() { return std::stoi(value); }                        // stoi
  int as_int(int base) { return std::stoi(value, nullptr, base); } // stoi with base
  double as_double() { return std::stod(value); }                  // stod
  float as_float() { return std::stof(value); }                    // stof
  std::string as_string() { return value; }                        // string
  // TO DO : Make template version as using lexical cast?

};

class Config_Reader {

  std::vector<Parameter> params_; // All parameters in top level of given config file

  // TO DO : NON-COPYABLE

public:

  Config_Reader(const std::string& filename) { /* TO DO : load file, parse parameters */ }

  // TO DO : Has function to tell if file has parameter of passed name

};
