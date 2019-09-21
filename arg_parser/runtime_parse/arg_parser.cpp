#include <iostream> // Debug only

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
class argument : public argument_base {

  // NON COPYABLE
  argument(const argument&);
  argument& operator=(const argument&);

  T value;

public:

  argument(std::string&& d, bool r): argument_base(d, r) { }

};

class Parser {

  // NON COPYABLE
  Parser(const Parser&);
  Parser& operator=(const Parser&); 

  std::vector<std::unique_ptr<argument_base>> possible_args_; // Holds list of possible args & values if set
  std::string desc_; // description of parser
  std::map<std::string, size_t> name_to_pos_; // map from name given in add_argument to position in possible_args_

public:

  Parser() = default;
  Parser(std::string s): desc_(s) { }

  template<typename T>
  void add_argument(const std::string& id, const std::string& name, std::string&& desc, bool required = false) {
    possible_args_.emplace_back(std::make_unique<argument<T>>(std::forward<std::string>(desc), required));
  }

  void access(size_t idx) {
    auto& test = static_cast<argument>(possible_args_.at(idx))->value;
  }

};
