#include <stdexcept>

#include <vector>
#include <string>
#include <memory>
#include <map>

#include <boost/lexical_cast.hpp>

// TO DO : Introduce namespaces? harmony { } & parser? { } & impl namespace ? 
// TO DO : Think about if people might want to access argument desc or help function/string
// TO DO : Think about constructor name convention, '-id' & '--name'


// Implimentation

// Return Type of argument's value (specialized for strings)
template<typename T>
struct arg_ret { typedef const T type; };

template<>
struct arg_ret<std::string> { typedef const std::string& type; };

// Base class pointer used for argument
// This is so arguement can have general template type
class argument_base {

  // NON COPYABLE
  argument_base(const argument_base&);
  argument_base& operator=(const argument_base&);

  const std::string desc;

public:

  argument_base(std::string&& d): desc(std::move(d)) { }
  
};

// Class holding value of argument of type T
template<typename T>
class argument : public argument_base {

  // NON COPYABLE
  argument(const argument&);
  argument& operator=(const argument&);

  T value_;

public:

  argument(T val, std::string&& d): argument_base(std::move(d)), value_(val) { }

  typename arg_ret<T>::type value() const { return value_; }

};
// End of implimentation items


// Command line argument parser modelled after arg_parse in python
class Parser {

  std::vector<std::unique_ptr<argument_base>> args_; // Holds list of arg values if passed
  const std::string desc_;                           // Description of parser or program
  std::map<std::string, size_t> name_to_pos_;        // Maps name in add_argument to pos in args_
  bool parsed;                                       // if parse() function called (required)

  // pre-parser struct which loads all passed params like '-id argument' or '--name argument' into
  // a map, also holds help_string for help function and if we should show help upon calling parse()
  struct Pre_Parser {
    
    std::map<std::string, std::string> pre_parsed_;
    void clear() { pre_parsed_.clear(); }

    std::string help_string;
    bool show_help;

    // constructor does pre-parsing
    //   TO DO : Think about ways to not show_help if argument starts with '-'
    //   TO DO : Think about what if name and id given in list of flags
    Pre_Parser(int argc, char** argv, const std::string& s): help_string(s + '\n'), show_help(false) {
      for(size_t i = 1; i < argc - 1; ++i) {
        if(argv[i][0] == '-') {
          if(pre_parsed_.count(argv[i]) == 0) {
            pre_parsed_[argv[i]] = argv[i+1];
          } else {
            help_string += "           Make sure not to use same flag twice!\n";
            show_help = true; 
          }
        }
      }
    }

  } pre_;


  // NON COPYABLE
  Parser(const Parser&);
  Parser& operator=(const Parser&); 

  void help() {
    throw std::runtime_error(pre_.help_string); 
  }

  // get value at passed idx in args_
  template<typename T>
  typename arg_ret<T>::type get(size_t idx) {
    argument<T>* tmp = static_cast<argument<T>*>(args_[idx].get());
    std::unique_ptr<argument<T>> derived_ptr;
    args_[idx].release();
    derived_ptr.reset(tmp);
    typename arg_ret<T>::type val = derived_ptr->value();
    return val;
  }

public:

  explicit Parser(int argc, char** argv):  pre_(argc, argv, ""), parsed(false) { }
  explicit Parser(int argc, char** argv, const std::string& s):  
    pre_(argc, argv, s), 
    desc_(s), 
    parsed(false) { }

  // Function to add an argument to list of possible arguments
  // This function also appends argument to args_ if it was pre-parsed
  template<typename T>
  void add_argument(const std::string& id, const std::string& name, 
                    std::string&& desc, bool required = false) {
    pre_.help_string += "           " + id + "  " + name + "  " + desc + 
                        "  " + (required ? "(required)\n" : "(not required)\n"); 
    if(pre_.pre_parsed_.count(id)) {
      name_to_pos_[name.substr(2)] = args_.size();
      T val = boost::lexical_cast<T>(pre_.pre_parsed_[id]);
      args_.emplace_back(std::make_unique<argument<T>>(val, std::forward<std::string>(desc)));
    } else if(pre_.pre_parsed_.count(name)) {
      name_to_pos_[name.substr(2)] = args_.size();
      T val = boost::lexical_cast<T>(pre_.pre_parsed_[name]);
      args_.emplace_back(std::make_unique<argument<T>>(val, std::forward<std::string>(desc)));
    } else {
      if(required) {
        pre_.help_string += "           Required argument : " + name.substr(2) + " not given!\n";
        pre_.show_help = true;
      }
    }
  }

  // Makes sure everything has been delt with properly and makes parser ready to work
  void parse() {
    if(pre_.show_help) help();
    pre_.clear();
    parsed = true;
  }

  // Function to get argument corresponding to passed name
  // Requires calling parse() before use
  template<typename T>
  typename arg_ret<T>::type get(const std::string& name) {
    if(!parsed) {
      pre_.help_string += "           Please make sure to call parse function before get/has!\n";
      help();
    }
    if(name_to_pos_.count(name)) {
      return get<T>(name_to_pos_[name]); // TO DO : Check for if name exists and check if parsed
    } else {
      pre_.help_string += "           Indexing argument '" + name + "' which is not given!\n";
      help(); 
    }
  }

  // Function which returns true if the argument is in args_ (ie add_argument and passed in)
  bool has(const std::string& name) {
    return name_to_pos_.count(name);
  }

};
