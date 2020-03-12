#ifndef HARMONY_LOGGER_H
#define HARMONY_LOGGER_H

#include <string>
#include <ostream>

// TO DO : KILL to kill the program on encounter ( -1 )
// TO DO : Add verbose mode
// TO DO : Think about displaying level printed at
// TO DO : Printf vs. Sprintf?
// TO DO : Template prints & prevent w/o '<<' op
// TO DO : Think about '\n' auto printing

enum Log_Level { FATAL = 0, WARN = 1, INFO = 2, DEBUG = 3 }; 

// Logger which only writes to ostream object if its Log_Level is at or above that of the message
class Logger {

  // Log_Level such that all messages at or below this level will be outputed
  Log_Level level_;
  // Log_Level that incoming stream messages will be at ( << ), set with <<
  Log_Level stream_level_;
  // Output stream
  std::ostream& stream_;

public:

  // Logger from a ostream with level_ specified
  Logger(std::ostream& stream, Log_Level ll = INFO):
    level_(ll),
    stream_level_(level_),
    stream_(stream) { }

  // PRINT FUNCTIONS 

  // Prints up to a buffer of size 4196
  void printf(Log_Level level, const char * format, ...);
  // Strings
  void printf(Log_Level level, const std::string& s);
  // Same as printf
  void sprintf(Log_Level level, const char * format, ...);
  // Strings
  void sprintf(Log_Level level, const std::string& s);
  // Only print up to len char in format
  void snprintf(Log_Level level, size_t len, const char * format, ...);

  // STREAMING

  // Template
  template <typename T>
  Logger& operator<<(const T& s) {
    if(stream_level_ <= level_) {
      stream_ << s;
    }
    return *this;
  }
  // change stream_level_ to passed level
  Logger& operator<<(Log_Level level) {
    stream_level_ = level;
    return *this;
  }
};

#endif
