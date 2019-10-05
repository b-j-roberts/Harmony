#include "arg_parser.h"

int main() {

  Parser parser("This is a parser");

  parser.add_argument<int>("-n", "--num", "This is my favorite number");
  parser.add_argument<float>("-f", "--float", "This is my favorite float number");
  parser.add_argument<std::string>("-w", "--word", "This is my favorite word");

  std::cout << typeid(parser.access<int>(0)).name() << " ";
  std::cout << typeid(parser.access<float>(1)).name() << " ";
  std::cout << typeid(parser.access<std::string>(2)).name() << "\n";

  return 0;
}
