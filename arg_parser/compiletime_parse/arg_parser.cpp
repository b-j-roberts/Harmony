#include <iostream> // Debug only

#include <tuple>
#include <string>
#include <map>
#include <cassert>
/*
template <size_t I>
struct get_impl {
  template <typename T>
  static decltype(auto) visit(T& tup, size_t idx) {
      if (idx == I - 1) return std::get<I - 1>(tup);
      else get_impl<I - 1>::visit(tup, idx);
  }
};

template<>
struct get_impl<0> {
  template<typename T>
  static decltype(auto) visit(T& tup, size_t idx) { assert(false); }
};

template<typename... Ts>
decltype(auto) get_at(std::tuple<Ts...>& tup, size_t idx) {
  return get_impl<sizeof...(Ts)>::visit(tup, idx);
};


template<typename T, size_t I>
struct get_impl{

  typedef T type;

  template<typename>
  static type obtain() {
    return std::get<I>(
  }

};
*/
class Parser {

  template<typename T>
  struct argument {
    const T value;
    bool set;
    std::string desc;
    bool required;
    argument(std::string&& d, bool r): value(), set(false), desc(std::move(d)), required(r) { }
  };

  std::tuple<> possible_args_; // Holds list of possible args & values if set
  std::string_view desc_; // description of parser
  std::map<std::string_view, size_t> name_to_pos_; // map from name given in add_argument to position in possible_args_
  std::map<std::string_view, size_t> id_to_pos_; // map from id given in add_argument to position in possible_args_

  // NO COPY OR ASSIGNMENT OPERATION
  Parser(const Parser&);
  Parser& operator=(const Parser&);

public:

  // Default const, no description
  constexpr Parser();
  // const, with desciption
  constexpr Parser(std::string_view s): desc_(s) { }
/*
  //add possible argument to the parser
  //  ex: Parser.add_argument<std::string>("-f", "--file", "this is the input file string location")
  template<typename T>
  void add_argument(const std::string& id, const std::string& name, std::string&& desc, bool required = false) {
    argument<T> temp_arg(std::forward<std::string>(desc), required);
    name_to_pos_.insert({std::move(name), std::tuple_size<decltype(possible_args_)>::value});
    id_to_pos_.insert({std::move(id), std::tuple_size<decltype(possible_args_)>::value});
    possible_args_ = std::tuple_cat(possible_args_, std::make_tuple(temp_arg));
  }

  template<typename T>
  void add_argument(const std::string& id, const std::string& name, bool required = false) {
    add_argument<T>(id, name, std::string(), required);
  }

  //Parse given arguments
  //  check for required
  //  set if set 

  //gets value with corresponding name
  decltype(auto) get(const std::string& name) {
    if(name_to_pos_.count(name) == 0) {
      throw std::runtime_error("Argument " + name + " was never added, please use add_argument function to add this option!");
    }
    auto& arg = get_at(possible_args_, name_to_pos_[name]);
    if(!arg.set) {
      throw std::runtime_error("Argument " + name + " was not set!");
    }
    return arg.value;
  }
*/
};

namespace Parse {
  const bool Required = true;
}
