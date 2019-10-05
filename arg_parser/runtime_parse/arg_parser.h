#include <iostream> // Debug only
#include <typeinfo> 

#include <vector>
#include <string>
#include <memory>
#include <map>

class argument_base {

  // NON COPYABLE
  argument_base(const argument_base&);
  argument_base& operator=(const argument_base&);

  bool set;
  const std::string desc;
  const bool required;

public:

  argument_base(std::string&& d, bool r): set(false), desc(std::move(d)), required(r) { }
  
};

template<typename T>
struct arg_ret { typedef const T type; };

template<>
struct arg_ret<std::string> { typedef const std::string& type; };

template<typename T>
class argument : public argument_base {

  // NON COPYABLE
  argument(const argument&);
  argument& operator=(const argument&);

  T value_;

public:

  argument(std::string&& d, bool r): argument_base(std::move(d), r) { }

  typename arg_ret<T>::type value() const { return value_; }

};

class Parser {

  // NON COPYABLE
  Parser(const Parser&);
  Parser& operator=(const Parser&); 

  static std::vector<std::unique_ptr<argument_base>> possible_args_; // Holds list of possible args & values if set
  const std::string desc_; // description of parser
  static std::map<std::string, size_t> name_to_pos_; // map from name given in add_argument to position in possible_args_

public:

  explicit Parser() = default;
  explicit Parser(const std::string& s): desc_(s) { }

  template<typename T>
  void add_argument(const std::string& id, const std::string& name, std::string&& desc, bool required = false) {
    possible_args_.emplace_back(std::make_unique<argument<T>>(std::forward<std::string>(desc), required));
  }

  template<typename T>
  typename arg_ret<T>::type access(size_t idx) {
    argument<T>* tmp = static_cast<argument<T>*>(possible_args_[idx].get());
    std::unique_ptr<argument<T>> derived_ptr;
    possible_args_[idx].release();
    derived_ptr.reset(tmp);
    typename arg_ret<T>::type val = derived_ptr->value();
    return val;
  }

};
/*
int main() {

  Parser parser("This is a parser");

  parser.add_argument<int>("-n", "--num", "This is my favorite number");
  parser.add_argument<float>("-f", "--float", "This is my favorite float number");
  parser.add_argument<std::string>("-w", "--word", "This is my favorite word");

  std::cout << typeid(parser.access<int>(0)).name() << " ";
  std::cout << typeid(parser.access<float>(1)).name() << " ";
  std::cout << typeid(parser.access<std::string>(2)).name() << "\n";

  return 0;
}*/
