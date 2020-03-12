#include "config_reader.h"

#include <array> // for bracer_pair's
#include <utility> // pair
#include <sstream> // istringstream
#include <stdexcept> // runtime_error

#include <boost/algorithm/string/trim.hpp>

std::vector<Parameter> Parameter::as_vector() const {
  std::string curr_val;
  std::vector<Parameter> ret;
  std::istringstream reader(value_);
  while(getline(reader, curr_val, ',')) {
    boost::trim(curr_val);
    ret.emplace_back(curr_val);
  } 
  return ret;
}

std::map<std::string, Parameter> Parameter::as_map() const {
  std::string curr_val;
  std::map<std::string, Parameter> ret;
  std::istringstream reader(value_);
  while(getline(reader, curr_val, ',')) {
    size_t sep = curr_val.find(':');
    std::string key = curr_val.substr(0, sep);
    boost::trim(key);
    std::string val = curr_val.substr(sep + 1);
    boost::trim(val);
    ret.emplace(key, Parameter(val));
  }
  return ret;
}

// Return bracer pair for c if it is valid, otherwise returns pair of '\001'
const std::pair<char, char>& is_valid_bracer(char c) {
  static const size_t delim_num = 6;
  static const std::array<std::pair<char, char>, delim_num> valid_bracers =
     {{ {'{', '}'}, {'[', ']'}, {'\'', '\''}, {'(', ')'}, {'\"', '\"'}, {'<', '>'} }};
  static const std::pair<char, char> invalid_pair = { '\001', '\001' };
  for(const auto& bracer : valid_bracers) if(c == bracer.first) return bracer;
  return invalid_pair;
}

void Config_Reader::parse_config_stream(std::istream& s, const std::string& filename) {
  std::string curr_line;
  while(getline(s, curr_line)) {
    // Find key/value seperator
    size_t delim_pos = curr_line.find(':');
    if(delim_pos != std::string::npos) {
      // get key
      std::string key = curr_line.substr(0, delim_pos);
      boost::trim(key);

      // get value
      std::string value;
      curr_line = curr_line.substr(delim_pos + 1);
      bool obtained = false;
      bool accumulating = false;
      int accumulator = 0;
      std::pair<char, char> bracer_pair;
      do {
        for(const auto& c : curr_line) {
          if(accumulating) {
            if(c == bracer_pair.second) --accumulator;
            if(accumulator == 0) {
              obtained = true;
              break;
            }
            if(c == bracer_pair.first) ++accumulator;
          } else {
            if(c != ' ') {
              bracer_pair = is_valid_bracer(c);
              accumulating = true;
              ++accumulator;
              if(bracer_pair.first == '\001') {
                obtained = true;
                break;
              }
            }
          }
        }
        value += curr_line + '\n';
      } while(!obtained && getline(s, curr_line));
      if(!obtained) throw std::runtime_error("Some end bracer or value is missing in : " + filename);

      if(bracer_pair.first != '\001') {
        size_t first_brace = value.find(bracer_pair.first);
        value = value.substr(first_brace + 1, value.rfind(bracer_pair.second) - first_brace - 1);
      }
      boost::trim(value);

      // Insert key/value into Config_Reader or Parameter based on bracer type
      if(bracer_pair.first == '{') {
        if(configs_.count(key)) throw std::runtime_error(key + " included more than once!");
        this->configs_.emplace(key, Config_Reader(value, filename));
      } else {
        if(params_.count(key)) throw std::runtime_error(key + " included more than once!");
        this->params_.emplace(key, Parameter(value));
      }
    }
  }
}

Config_Reader::Config_Reader(const std::string& str, const std::string& filename) {
  std::istringstream config_in(str);
  parse_config_stream(config_in, filename);
}

Config_Reader::Config_Reader(const std::string& filename) {
  std::ifstream config_in(filename);
  if(config_in.is_open()) {
    parse_config_stream(config_in, filename);
  } else {
    throw std::runtime_error("Error opening config file : " + filename);
  }
}

const Config_Reader& Config_Reader::config(const std::string& name) const {
  if(configs_.count(name)) {
    return configs_.at(name);
  } else {
    throw std::runtime_error("Config Reader : " + name + " not included!");
  }
}

bool Config_Reader::has_param(const std::string& name) const {
  // Uses functionality of param(name) to determine if accessable w/ nested syntax
  try { (void) param(name); } catch(...) { return false; }
  return true;
}

// TO DO : Think of way to access without making strings
const Parameter& Config_Reader::param(const std::string& name) const {
  if(params_.count(name)) {
    return params_.at(name);
  } else {
    size_t pos = name.find("::");
    if(pos == std::string::npos) throw std::runtime_error("Parameter : " + name + " not included!");
    if(configs_.count(name.substr(0, pos)))
      return configs_.at(name.substr(0, pos)).param(name.substr(pos + 2));
    else throw std::runtime_error("No Config matching name : " + name.substr(0, pos));
  }
}
