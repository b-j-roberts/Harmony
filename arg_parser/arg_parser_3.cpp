#include <iostream> // Debug only

#include <tuple>
#include <string>

template<size_t I>
class Argument {

  const size_t idx_;
  const std::string_view id_, name_, desc_;
  const bool req_;

public:

  constexpr Argument<I>(const std::string_view& id, const std::string_view& name, const std::string_view& desc, bool required):
    idx_(I),
    id_(id),
    name_(name),
    desc_(desc),
    req_(required) { }

};

class ParserImpl {

  static size_t constexpr num_args_{0};

public:

  constexpr ParserImpl() { }

  template<typename T>
  void constexpr add_argument(const std::string_view& id, const std::string_view& name, const std::string_view& desc, bool required = false) {
    ++num_args_;
    Argument<num_args_> arg(id, name, desc, required);
  }

  void size() {
    std::cout << num_args_ << '\n';
  }

};

int main() {

  ParserImpl parser;
  parser.add_argument<std::string>("-f", "--file", "this is a file");

  parser.size();

};
