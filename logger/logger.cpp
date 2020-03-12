#include "logger.h"

#include <cstdarg> // va_start & va_end

void Logger::printf(Log_Level level, const char * format, ...) {
  if(level <= level_) {
    char buf[4196]; // TO DO : Think about this number
    va_list arg;
    va_start(arg, format);
    (void)vsprintf(buf, format, arg);
    va_end(arg);
    stream_ << buf << '\n';
  }
}
void Logger::printf(Log_Level level, const std::string& s) {
  if(level <= level_) {
    stream_ << s << '\n';
  }
}

void Logger::sprintf(Log_Level level, const char * format, ...) {
  if(level <= level_) {
    char buf[4196]; // TO DO : Think about this number
    va_list arg;
    va_start(arg, format);
    (void)vsprintf(buf, format, arg);
    va_end(arg);
    stream_ << buf << '\n';
  }
}
void Logger::sprintf(Log_Level level, const std::string& s) {
  if(level <= level_) {
    stream_ << s << '\n';
  }
}

void Logger::snprintf(Log_Level level, size_t len, const char * format, ...) {
  if(level <= level_) {
    char* buf = new char[len];
    va_list arg;
    va_start(arg, format);
    vsnprintf(buf, len, format, arg);
    va_end(arg);
    stream_ << buf << '\n';
    delete[] buf;
  }
}
