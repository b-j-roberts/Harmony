#ifndef HARMONY_CONFIG_READER_H
#define HARMONY_CONFIG_READER_H

#include <string>
#include <map>
#include <vector> // for as_vector() function

#include <fstream> // istream

#include <boost/lexical_cast.hpp>

// TO DO : Think about ';' for termination
// TO DO : Way to comment out parts of config
// TO DO : Think string_view instead of string

// Stores string a key's value from a config file/string
// Impliments as_* function to obtain string as different types
class Parameter {

  const std::string value_;

public:

  explicit Parameter(const std::string& s): value_(s) { } 
  Parameter() = default;

  // as_* functions to get parameter value_
  int as_int() const { return std::stoi(value_); }                        // stoi
  int as_int(int base) const { return std::stoi(value_, nullptr, base); } // stoi with base
  double as_double() const { return std::stod(value_); }                  // stod
  float as_float() const { return std::stof(value_); }                    // stof
  const std::string& as_string() const { return value_; }                 // string
  template <typename T> const T as() const { return boost::lexical_cast<T>(value_); } // template

  // Return vector of parameters if value_ of form : val1, val2, ...
  std::vector<Parameter> as_vector() const;
  // Return map of parameters if value_ of form : key1 : val1, key2 : val2, ...
  std::map<std::string, Parameter> as_map() const;
};

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
  const Config_Reader& config(const std::string& name) const;
  // Return const& to configs_ ( for looping through )
  const std::map<std::string, Config_Reader>& configs() const;

  // nested syntax : "conf_1::conf_2::conf_3::...::param_name"
  //   this indexing configs_ w/ key conf_1 => indexed Config_Reader's configs_ w/ key conf_2 => ...
  //   till last param_name which indexes last Config_Reader's params_ w/ key param_name

  // Returns whether there is a Parameter w/ key 'name' ( using nested syntax )
  bool has_param(const std::string& name) const;
  // Index Parameter w/ nested syntax
  const Parameter& param(const std::string& param_path) const;
  const Parameter& operator[] (const std::string& param_path) const { return param(param_path); }
};

#endif
