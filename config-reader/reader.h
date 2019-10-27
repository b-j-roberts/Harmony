#include <iostream> // DEBUG ONLY

#include <sting>
#include <vector>
#include <map>
#include <ifstream>
#include <stdexcept>
#include <sstream>
#include <pair>

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

  // TO DO : copy Constructor
  Parameter(const std::string& s): value(s) { } 

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


// Returns end bracer of passed char, if invalid returns '\001'
std::pair<char, char> is_valid_bracer(char c) {
  static const size_t delim_num{6};
  static char valid_inner_start[delim_num] = { '{', '[', ''', '(', '"', '<' };
  static char valid_inner_end[delim_num] = { '}', ']', ''', ')', '"', '>' };
  for(size_t i = 0; i < delim_num; ++i) {
    if(c == valid_inner_start[i]) 
      return std::make_pair(valid_inner_start[i], valid_inner_end[i]);
  }
  return std::make_pair('\001', '\001');
}

class Config_Reader {

  std::map<std::string, Config_Reader> params_; // Map from names of Config Readers to Config Readers in top level of file
  std::map<std::string, Parameter> values_; 
  // TO DO : NON-COPYABLE
  //

  // Config_Reader from a string
  Config_Reader(const std::string& str, bool private_mask) { 
    std::istringstream config_in(str);
    std::string curr_line;
    while(getline(config_in, curr_line)) {
      size_t delim_pos = curr_line.find(':');
      if(delim_pos != std::string::npos) {
        std::string key = curr_line.substr(0, delim_pos);
        boost::trim(key);
        // get value using parenthesis detect

        bool obtained = false;
        size_t pos = delim_pos + 1;
        bool begin_accumulate = false;
        size_t accumulator = 0;
        std::pair<char, char> end_bracer;
        std::string value;
        do {
          size_t line_len = curr_line.length();
          for(;pos < line_len; ++pos) {
            if(!begin_accumulate && curr_line[pos] != ' ') {
              end_bracer = is_valid_bracer(curr_line[pos]);
              begin_accumulate = true;
              accumulator = 1;
              if(end_bracer == '\001') { // it is line, no bracers
                value = curr_line.substr(pos);
                boost::trim(value);
                obtained = true;
              } else +++pos; // get inside
            }
            if(begin_accumulate) {
              if(curr_line[pos] == end_bracer.first) ++accumulator;
              else if(curr_line[pos] == end_bracer.second) --accumulator; // TO DO : rename end_bracer
              if(accumulator == 0) {
                obtained = true;
                value += curr_line(0, pos);
              }
            }
          }
          pos = 0;
          if(!obtained) value += curr_line;
        } while(!obtained && getline(config_in, curr_line));
        // TO DO: If not obtained and break, ie inside bracer but no end
        //
        // insert into map
        
        if(end_bracer == '\001') {
          // this is a value that needed to be kept
          this->values_.emplace(key, Parameter(value));
        } else {
          // this is another config reader that needs to be constructed from string constructor
          this->params_.emplace(key, Config_Reader(value, true));
        }
      }
    }

  }

public:

  // Config_Reader from a file
  explicit Config_Reader(const std::string& filename) { 
    std::ifstream config_in(filename);
    if(config_in.is_open()) {
      std::string curr_line;
      while(getline(config_in, curr_line)) {
        size_t delim_pos = curr_line.find(':');
        if(delim_pos != std::string::npos) {
          std::string key = curr_line.substr(0, delim_pos);
          boost::trim(key);
          // get value using parenthesis detect

          bool obtained = false;
          size_t pos = delim_pos + 1;
          bool begin_accumulate = false;
          size_t accumulator = 0;
          std::pair<char, char> end_bracer;
          std::string value;
          do {
            size_t line_len = curr_line.length();
            for(;pos < line_len; ++pos) {
              if(!begin_accumulate && curr_line[pos] != ' ') {
                end_bracer = is_valid_bracer(curr_line[pos]);
                begin_accumulate = true;
                accumulator = 1;
                if(end_bracer == '\001') { // it is line, no bracers
                  value = curr_line.substr(pos);
                  boost::trim(value);
                  obtained = true;
                } else +++pos; // get inside
              }
              if(begin_accumulate) {
                if(curr_line[pos] == end_bracer.first) ++accumulator;
                else if(curr_line[pos] == end_bracer.second) --accumulator; // TO DO : rename end_bracer
                if(accumulator == 0) {
                  obtained = true;
                  value += curr_line(0, pos);
              }
            }
            pos = 0;
            if(!obtained) value += curr_line;
          } while(!obtained && getline(config_in, curr_line));
          // TO DO: If not obtained and break, ie inside bracer but no end
          //
          // insert into map
          
          if(end_bracer == '\001') {
            // this is a value that needed to be kept
            this->values_.emplace(key, Parameter(value));
          } else {
            // this is another config reader that needs to be constructed from string constructor
            this->params_.emplace(key, Config_Reader(value, true));
          }

        }
      }
    } else {
      throw std::runtime_error("Error opening config file : " + filename);
    }
  }

  // indexing values
  Parameter &operator[] (const std::string& param) {
    if(values_.count(param)) {
      return values_[param];
    } else {
      throw std::out_of_range("Parameter : " + param + " not included!");
    }
  }

  // indexing nested config readers
  Config_Reader config(const std::string& name) {
    if(params_.count(name)) {
      return params_[name];
    } else {
      throw std::out_of_range("Config Reader : " + name + " not included!");
    }
  }

  // TO DO : Has function to tell if file has parameter of passed name

};
