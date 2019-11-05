#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <utility>

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

// TO DO : Namespaces
// TO DO : Split into cpp file, Look into static library creation
// TO DO : Think about returning config map and param map options
//         (so one can for loop over all configs/params)
// TO DO : Defaultable parameter
// TO DO : Think about ';' for termination

// Stores actual parameter values, and has as_* functions to obtain this value
class Parameter {

  const std::string value;

public:

  // Copy constructors and assignment operators taken as default

  explicit Parameter(const std::string& s): value(s) { } 
  Parameter() = default;

  // as_* functions to get parameter value
  const int as_int() const { return std::stoi(value); }                        // stoi
  const int as_int(int base) const { return std::stoi(value, nullptr, base); } // stoi with base
  const double as_double() const { return std::stod(value); }                  // stod
  const float as_float() const { return std::stof(value); }                    // stof
  const std::string& as_string() const { return value; }                       // string

  template <typename T> const T as() const { return boost::lexical_cast<T>(value); } // template

  // if parameter is in format item1, item2, ...
  const std::vector<Parameter> as_vector() const {
    std::string curr_val;
    std::vector<Parameter> ret;
    std::istringstream reader(value);
    while(getline(reader, curr_val, ',')) {
      boost::trim(curr_val);
      ret.emplace_back(curr_val);
    }
    return ret;
  }

  // if parameter is in format item1 : this, item2 : that, ...
  std::map<std::string, Parameter> as_map() const {
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

};


// Returns end bracer of passed char, if invalid returns '\001'
std::pair<char, char> is_valid_bracer(char c) {
  static const size_t delim_num{6};
  static char valid_inner_start[delim_num] = { '{', '[', '\'', '(', '\"', '<' };
  static char valid_inner_end[delim_num] = { '}', ']', '\'', ')', '\"', '>' };
  for(size_t i = 0; i < delim_num; ++i) {
    if(c == valid_inner_start[i]) 
      return std::make_pair(valid_inner_start[i], valid_inner_end[i]);
  }
  return std::make_pair('\001', '\001');
}

// Main object that parses config files and stores file's params
class Config_Reader {

  std::map<std::string, Config_Reader> configs_; // Map from names of Config Readers to 
                                                 // Config Readers in top level of this
  std::map<std::string, Parameter> params_;      // Map from parameter name to values in this 

  // Config_Reader from a string
  Config_Reader(const std::string& str, const std::string& filename) { 
    std::istringstream config_in(str);
    std::string curr_line;
    while(getline(config_in, curr_line)) {
      size_t delim_pos = curr_line.find(':');
      if(delim_pos != std::string::npos) {
        std::string key = curr_line.substr(0, delim_pos);
        boost::trim(key);

        // get value using parenthesis detection
        bool obtained = false;
        size_t pos = delim_pos + 1;
        bool begin_accumulate = false;
        size_t first_acc = 0;
        size_t accumulator = 0;
        std::pair<char, char> bracer_pair;
        std::string value;
        do {
          size_t line_len = curr_line.length();
          for(;pos < line_len; ++pos) {
            if(!begin_accumulate && curr_line[pos] != ' ') {
              bracer_pair = is_valid_bracer(curr_line[pos]);
              begin_accumulate = true;
              accumulator = 1;
              if(bracer_pair.first == '\001') { // it is line, no bracers
                value = curr_line.substr(pos);
                boost::trim(value);
                obtained = true;
                break;
              } else {
                ++pos; // get inside
                first_acc = pos;
                if(pos >= line_len) continue;
              }
            }
            if(begin_accumulate) {
              if(curr_line[pos] == bracer_pair.second) --accumulator;
              if(accumulator == 0) {
                obtained = true;
                value += curr_line.substr(first_acc, pos - first_acc);
                first_acc = 0;
              }
              if(curr_line[pos] == bracer_pair.first) ++accumulator;
            }
          }
          pos = 0;
          if(first_acc != 0) {
            value += curr_line.substr(first_acc) + '\n';
            first_acc = 0;
          } else if(!obtained) value += curr_line + '\n';
        } while(!obtained && getline(config_in, curr_line));

        if(!obtained) throw std::runtime_error("Some end bracket is missing in : " + filename);

        // insert into map
        boost::trim(value);
        if(bracer_pair.first == '{') {
          // this is a value that needed to be kept
          if(configs_.count(key)) throw std::runtime_error(key + " included more than once!");
          this->configs_.emplace(key, Config_Reader(value, filename));
        } else {
          // this is another config reader that needs to be constructed from string constructor
          if(params_.count(key)) throw std::runtime_error(key + " included more than once!");
          this->params_.emplace(key, Parameter(value));
        }
      }
    }
  } // end of constructor

public:

  //Using default copy const and assignment operator

  Config_Reader() = default;

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

          // get value using parenthesis detection
          bool obtained = false;
          size_t pos = delim_pos + 1;
          bool begin_accumulate = false;
          size_t first_acc = 0;
          size_t accumulator = 0;
          std::pair<char, char> bracer_pair;
          std::string value;
          do {
            size_t line_len = curr_line.length();
            for(;pos < line_len; ++pos) {
              if(!begin_accumulate && curr_line[pos] != ' ') {
                bracer_pair = is_valid_bracer(curr_line[pos]);
                begin_accumulate = true;
                accumulator = 1;
                if(bracer_pair.first == '\001') { // it is line, no bracers
                  value = curr_line.substr(pos);
                  boost::trim(value);
                  obtained = true;
                  break;
                } else {
                  ++pos; // get inside
                  first_acc = pos;
                  if(pos >= line_len) continue;
                }
              }
              if(begin_accumulate) {
                if(curr_line[pos] == bracer_pair.second) --accumulator;
                if(accumulator == 0) {
                  obtained = true;
                  value += curr_line.substr(first_acc, pos - first_acc);
                  first_acc = 0;
                }
                if(curr_line[pos] == bracer_pair.first) ++accumulator;
              }
            }
            pos = 0;
            if(first_acc != 0) {
              value += curr_line.substr(first_acc) + '\n';
              first_acc = 0;
            } else if(!obtained) value += curr_line + '\n';
          } while(!obtained && getline(config_in, curr_line));
         
          if(!obtained) throw std::runtime_error("Some end bracket is missing in : " + filename);

          // insert into map
          boost::trim(value);
          if(bracer_pair.first == '{') {
            // this is a value that needed to be kept
            if(configs_.count(key)) throw std::runtime_error(key + " included more than once!");
            this->configs_.emplace(key, Config_Reader(value, filename));
          } else {
            // this is another config reader that needs to be constructed from string constructor
            if(params_.count(key)) throw std::runtime_error(key + " included more than once!");
            this->params_.emplace(key, Parameter(value));
          }
        }
      }
    } else {
      throw std::runtime_error("Error opening config file : " + filename);
    }
  } // end of main constructor

  // Access param using param name or nested :: stucture name
  const Parameter& param(const std::string& param) const {
    if(params_.count(param)) {
      return params_.find(param)->second;
    } else {
      size_t pos = param.find("::");
      std::vector<std::string> param_list;
      if(pos != std::string::npos) param_list.push_back(param.substr(0, pos));
      while(pos != std::string::npos) {
        size_t new_pos = param.find("::", pos + 2);
        param_list.push_back(param.substr(pos + 2, new_pos - pos - 2));
        pos = new_pos;
      }
      const Config_Reader* nester = this;
      for(size_t i = 0, i_end = param_list.size(); i < i_end; ++i) {
        if(i != i_end - 1) nester = &nester->config(param_list[i]);
        else return nester->param(param_list[i]);
      }
      throw std::out_of_range("Parameter : " + param + " not included!");
    }
  }

  // indexing params
  const Parameter& operator[] (const std::string& param_) const {
    return param(param_);
  }

  // indexing nested config readers
  const Config_Reader& config(const std::string& name) const {
    if(configs_.count(name)) {
      return configs_.find(name)->second;
    } else {
      throw std::out_of_range("Config Reader : " + name + " not included!");
    }
  }

  // Detects if param_ is accessable param object
  bool has_param(const std::string& param_) const {
    try {
      auto test = param(param_);
    }
    catch(std::out_of_range) {
      return false;
    }
    return true;
  }

  bool has_config(const std::string& name) const {
    return configs_.count(name);
  }

};
