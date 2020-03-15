#ifndef HARMONY_ARG_PARSER_H
#define HARMONY_ARG_PARSER_H

#include <string>
#include <map>
#include <memory> // shared_ptr & dynamic_pointer_cast

#include <boost/lexical_cast.hpp>
#include <stdexcept> // runtime_error

// TO DO : Think about constructor name convention, '-id' & '--name'
// TO DO : Interface for -h? flag to show help_string after fully loaded?
// TO DO : Check for illegitamite flags?
// TO DO : Namespacing


// Implimentation Things ( Helpers )

// Return Type of argument's value (specialized for strings)
template<typename T>
struct arg_ret { typedef const T type; };
template<>
struct arg_ret<std::string> { typedef const std::string& type; };

// Base class pointer used for argument ( allowing template value inside map )
class argument_base {
public:
  argument_base() = default;
  argument_base(const argument_base&) = delete;
  argument_base& operator=(const argument_base&) = delete;
  virtual ~argument_base() = default;
};

// Class holding value of argument of type T
template<typename T>
class argument : public argument_base {
  T value_;
public:
  explicit argument(T val): value_(val) { }
  ~argument() override = default;
  typename arg_ret<T>::type value() const { return value_; }
};


// Interface Class

// Command line argument parser modelled after arg_parse in python
class Parser {
  // Maps name of argument (given in add_argument) to argument value ( by ptr to base class )
  std::map<std::string, std::shared_ptr<argument_base>> name_to_arg_;

  // String used to diplay help information : called when error thrown
  std::string help_string_;
  inline static const std::string help_str_spaces = "           "; // Formatting spaces

  // Pre-parsed args <id/name, value> ( done at construction before add_arguments have been called )
  // TO DO : Think of way to clear this after done loading name_to_arg_
  std::map<std::string, std::string> pre_parsed_;

public:

  Parser(int argc, char** argv, const std::string& desc = "");
  Parser(const Parser&) = delete;
  Parser& operator=(const Parser&) = delete; 

  // Appends argument to name_to_args_ if it was pre-parsed ( & requirement handling )
  // params : <T> is arg type, id (-id), name (--name), description, arg required ( default false )
  template<typename T>
  void add_argument(const std::string&, const std::string&, const std::string&, bool = false);

  // If 'name' has corresponding add_argument entry & was a passed in argument
  bool has(const std::string& name) { return name_to_arg_.count(name); }

  // Get argument value corresponding to passed name
  template<typename T>
  typename arg_ret<T>::type get(const std::string& name);
};

template<typename T>
void Parser::add_argument(const std::string& id, const std::string& name,
                          const std::string& desc, bool required) {
  // TO DO : Show type info ( make names readable )
  help_string_ += help_str_spaces + id + "   " + name + "   " + desc +
                 "  " + (required ? "(required)\n" : "(not required)\n");
  // TO DO : Error if pre_parsed id & name passed
  if(pre_parsed_.count(id)) {
    T val = boost::lexical_cast<T>(pre_parsed_[id]);
    name_to_arg_.emplace(name.substr(2), std::make_shared<argument<T>>(val));
  } else if(pre_parsed_.count(name)) {
    T val = boost::lexical_cast<T>(pre_parsed_[name]);
    name_to_arg_.emplace(name.substr(2), std::make_shared<argument<T>>(val));
  } else if(required) {
    help_string_ += help_str_spaces + "Required argument '" + name.substr(2) + "' not given!\n";
    throw std::runtime_error(help_string_); 
  } 
}

template<typename T>
typename arg_ret<T>::type Parser::get(const std::string& name) {
  if(name_to_arg_.count(name)) {
    // Downcast Base pointer
    auto derived_ptr = std::dynamic_pointer_cast<argument<T>>(name_to_arg_[name]);
    // TO DO : if(derived_ptr) ?
    return derived_ptr->value();
  } else {
    help_string_ += help_str_spaces + "Indexed argument '" + name + "' not given!\n";
    throw std::runtime_error(help_string_);
  }
}

#endif
