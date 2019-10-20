#include "arg_parser.h"
#include <iostream>

int main(int argc, char** argv) {

  Parser parser(argc, argv, "This is a parser");

  parser.add_argument<int>("-n", "--num", "This is my favorite number", true);
  parser.add_argument<double>("-f", "--float", "This is my favorite float number");
  parser.add_argument<std::string>("-w", "--word", "This is my favorite word");

  parser.parse();

  int arg_first = parser.get<int>("num");
  double arg_second = parser.get<double>("float");
  std::string arg_last = parser.get<std::string>("word");

  std::cout << arg_first << "\n";
  std::cout << arg_second * 1000 << "\n";
  std::cout << arg_last << "\n";

  return 0;
}
