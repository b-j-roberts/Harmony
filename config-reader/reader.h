#include <iostream> // DEBUG ONLY

#include <sting>
#include <vector>
#include <map>
#include <ifstream>
#include <stdexcept>

#include <boost/algorithm/string/trim.hpp>

// TO DO : Namespaces

// Class to detect beginning and ending parenthesis in string
class Parenthesis_Detect {

  size_t count;

public:

  Parentheis_Detect(): count(0) { }

}

// Requires { } , ( ), [ ] if on more than one line
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
  // TO DO : As vector
  //         As Map ?
  //         As queue ?

};

class Config_Reader {

  std::map<std::string, Config_Reader> params_; // Map from names of Config Readers to Config Readers in top level of file

  // TO DO : NON-COPYABLE
  //

  // Config_Reader from a string
  Config_Reader(const std::string& str, bool private_mask) { }

public:

  // Config_Reader from a file
  Config_Reader(const std::string& filename) { 
    std::ifstream config_in(filename);
    if(config_in.is_open()) {
      std::string curr_line;
      while(getline(config_in, curr_line)) {
        size_t delim_pos = curr_line.find(':');
        if(delim_pos != std::string::npos) {
          std::string key = curr_line.substr(0, delim_pos);
          boost::trim(key);
          // get value using parenthesis detect
          // insert into map
        }
      }
    } else {
      throw std::runtime_error("Error opening config file : " + filename);
    }
  }

  // TO DO : Has function to tell if file has parameter of passed name

};
