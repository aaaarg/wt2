// Copyleft 2013 tho@autistici.org

#ifndef UTILS_LOG_H_
#define UTILS_LOG_H_

#include <syslog.h>
#include <cstdarg>

namespace utils {

class Log {

 public:
  Log(Log const&) = delete;
  Log& operator= (Log const&) = delete;

 public:
  static Log* Instance();

 public:
  void Open(const char* ident, int logopt, int facility);
  void Write(int priority, const char* fmt, ...) const;
  void Debug(const char* fmt, ...) const;
  void Close() const;

 private:
  Log() : opened_(false) { }

 private:
  static Log* instance_;
  bool opened_;
};

}   // namespace utils

#endif  // UTILS_LOG_H_
