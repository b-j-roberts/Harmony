#include <string>
#include <fstream>
#include <stdexcept>
#include <stdarg.h>

#include <iostream> // TO DO

enum Log_Level { FATAL = 0, WARN = 1, INFO = 2, DEBUG = 3 }; // TO DO : KILL to kill program

class Logger {

  Log_Level level_;
  Log_Level stream_level_;
  std::ofstream outfile_;

public:

  Logger(const std::string& filename, Log_Level ll = INFO):
    level_(ll),
    stream_level_(level_),
    outfile_(filename) {
    // open filename for streaming
    if(!outfile_.is_open()) { // TO DO :  appending?
      throw std::runtime_error("Logger:: No file " + filename + " found!");
    }
  }

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

  Logger& operator<< (const std::string& s) {
    if(stream_level_ <= level_) {
      outfile_ << s;
    }
    return *this;
  }

  Logger& operator<<(Log_Level level) {
    stream_level_ = level;
    return *this;
  }

};

int main() {

 int test = -123;

 std::string s(" os gpps");
 Log_Level lev = DEBUG;

 Logger logger("log.log");
 logger.sprintf(INFO, "Info number:%d", 1); 
 logger.snprintf(WARN, 300, "WARN number:%d %s", 1, "cats"); 
 logger.printf(FATAL, "FATAL number:%d", 3); 
 logger.sprintf(FATAL, "FATAL number: " + s + " os gpps"); 

 logger << s << s << "\n";
 logger << 12 << 14.551123 << "\n";
 logger << -14 << 'c' << "\n";
 logger << 1 << lev << "\n" << WARN << "thisone";

}
