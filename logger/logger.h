#include <fstream>
#include <stdexcept>
#include <cstdarg> // va_start & va_end

#include <string>

// TO DO : KILL to kill the program on encounter ( -1 )
// TO DO : Stream Constructor ( so one can use cout and stuff )
// TO DO : Allow appending to outfile as well as reading
// TO DO : Think about buf size in printf functions
// TO DO : Think about displaying level printed at
// TO DO : Check if you can use Log_Level::WARN (example) syntax
// TO DO : Change copy construct streamable object template?

enum Log_Level { FATAL = 0, WARN = 1, INFO = 2, DEBUG = 3 }; 

class Logger {

  Log_Level level_;        // All messages at or below this level will be outputed to outfile_
  Log_Level stream_level_; // Level that all messages coming as << will be streamed at
  std::ofstream outfile_;

public:

  // Construct a Logger with outfile as filename and levels ll
  Logger(const std::string& filename, Log_Level ll = INFO):
    level_(ll),
    stream_level_(level_),
    outfile_(filename) {
    if(!outfile_.is_open()) {
      throw std::runtime_error("Logger:: No file " + filename + " found!");
    }
  }

  // Prints up to a buffer of size 4196
  void printf(Log_Level level, const char * format, ...) {
    if(level <= level_) {
      char buf[4196]; // TO DO : Think about this number
      va_list arg;
      va_start(arg, format);
      int len = vsprintf(buf, format, arg);
      va_end(arg);
      outfile_ << buf << '\n';
    }
  }

  // Strings
  void printf(Log_Level level, std::string s) {
    if(level <= level_) {
      outfile_ << s << '\n';
    }
  }

  // Same as printf
  void sprintf(Log_Level level, const char * format, ...) {
    if(level <= level_) {
      char buf[4196]; // TO DO : Think about this number
      va_list arg;
      va_start(arg, format);
      int len = vsprintf(buf, format, arg);
      va_end(arg);
      outfile_ << buf << '\n';
    }
  }

  // Strings
  void sprintf(Log_Level level, std::string s) {
    if(level <= level_) {
      outfile_ << s << '\n';
    }
  }

  void snprintf(Log_Level level, const int len, const char * format, ...) {
    if(level <= level_) {
      char buf[len];
      va_list arg;
      va_start(arg, format);
      vsnprintf(buf, len, format, arg);
      va_end(arg);
      outfile_ << buf << '\n';
    }
  }

  // stream operators

  // template
  template <typename T>
  Logger& operator<<(T s) {
    if(stream_level_ <= level_) {
      outfile_ << s;
    }
    return *this;
  }

  // string overload to prevent copying
  Logger& operator<< (const std::string& s) {
    if(stream_level_ <= level_) {
      outfile_ << s;
    }
    return *this;
  }

  // change stream_level_ to passed level
  Logger& operator<<(Log_Level level) {
    stream_level_ = level;
    return *this;
  }

};
