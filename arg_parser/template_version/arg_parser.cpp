#include "arg_parser.h"

Parser::Parser(int argc, char** argv, const std::string& desc):
  help_string_(desc + '\n') {
  // Do Pre-Parsing ( fills pre_parsed_ )
  // TO DO : Think about ways to let values start with '-'
  // TO DO : Think about if flag doesnt have value ( ie -v for verbose mode )
  for(int i = 1; i < argc - 1; ++i) {
    if(argv[i][0] == '-') {
      if(pre_parsed_.count(argv[i]) == 0) {
        pre_parsed_[argv[i]] = argv[i+1];
      } else {
        help_string_ += help_str_spaces + "Make sure not to use same flag twice!\n";
        throw std::runtime_error(help_string_);
      }
    }
  }
}
