#include <string>
#include <map>
#include <vector> // for as_vector() function
#include <array> // for bracer_pair's
#include <utility> // pair

#include <fstream> // istream
#include <sstream> // istringstream
#include <stdexcept> // runtime_error

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

// TO DO : Think about ';' for termination
// TO DO : Way to comment out parts of config
// TO DO : Think string_view instead of string

// Stores string a key's value from a config file/string
// Impliments as_* function to obtain string as different types
class Parameter {

  const std::string value;

public:

  explicit Parameter(const std::string& s): value(s) { } 
  Parameter() = default;

  // as_* functions to get parameter value
  int as_int() const { return std::stoi(value); }                        // stoi
  int as_int(int base) const { return std::stoi(value, nullptr, base); } // stoi with base
  double as_double() const { return std::stod(value); }                  // stod
  float as_float() const { return std::stof(value); }                    // stof
  const std::string& as_string() const { return value; }                 // string
  template <typename T> const T as() const { return boost::lexical_cast<T>(value); } // template

  // Return vector of parameters if value of form : val1, val2, ...
  const std::vector<Parameter> as_vector() const;
  // Return map of parameters if value of form : key1 : val1, key2 : val2, ...
  std::map<std::string, Parameter> as_map() const;
};

const std::vector<Parameter> Parameter::as_vector() const {
  std::string curr_val;
  std::vector<Parameter> ret;
  std::istringstream reader(value);
  while(getline(reader, curr_val, ',')) {
    boost::trim(curr_val);
    ret.emplace_back(curr_val);
  }
  return ret;
}

std::map<std::string, Parameter> Parameter::as_map() const {
  std::string curr_val;
  std::map<std::string, Parameter> ret;
  std::istringstream reader(value);
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

// Object containing nested Parameters & Config_Readers ( sections contained in { } after a key ) 
// Also the overall file given in constructor is it's own Config_Reader
class Config_Reader {

  // Key to Config_Reader map ( top level config readers inside this config reader )
  std::map<std::string, Config_Reader> configs_;
  // Key to Parameter map ( of form : 'key : value\n' in the containing Config_Reader )
  std::map<std::string, Parameter> params_;

  // Config_Reader from a string ( for nested Config_Readers )
  Config_Reader(const std::string& str, const std::string& filename);
  // Parse a stream type for Config_Readers & Parameters ( used to construct a Config_Reader )
  void parse_config_stream(std::istream& s, const std::string& filename); 

public:

  // Create Config_Reader from a file w/ path 'filename'
  explicit Config_Reader(const std::string& filename);

  // Returns whether this Config_Reader contains Config_Reader w/ key 'name'
  // TO DO : Allow checking nested Config_Readers like Parameters ?
  bool has_config(const std::string& name) const { return configs_.count(name); }
  // Index configs_ map for Config_Reader
  // TO DO : Is this ever really needed ?
  const Config_Reader& config(const std::string& name) const {
    if(configs_.count(name)) {
      return configs_.at(name);
    } else {
      throw std::runtime_error("Config Reader : " + name + " not included!");
    }
  }

  // nested syntax : "conf_1::conf_2::conf_3::...::param_name"
  //   this indexing configs_ w/ key conf_1 => indexed Config_Reader's configs_ w/ key conf_2 => ...
  //   till last param_name which indexes last Config_Reader's params_ w/ key param_name

  // Returns whether there is a Parameter w/ key 'name' ( using nested syntax )
  bool has_param(const std::string& name) const {
    // Uses functionality of param(name) to determine if accessable w/ nested syntax
    try { (void) param(name); } catch(...) { return false; }
    return true;
  }
  // Index Parameter w/ nested syntax
  const Parameter& param(const std::string& param_path) const;
  const Parameter& operator[] (const std::string& param_path) const { return param(param_path); }
};

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

