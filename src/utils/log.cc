// Copyleft 2013 tho@autistici.org

#include <cstdarg>
#include <syslog.h>
#include "utils/log.h"

namespace utils {

Log* Log::instance_ = nullptr;

Log* Log::Instance() {
  if (!instance_)
    instance_ = new Log;
     
  return instance_;
}

void Log::Open(const char* ident, int logopt, int facility) {
  if (!opened_) {
    openlog(ident, logopt, facility);
    opened_ = true;
  }
}

void Log::Write(int priority, const char* fmt, ...) const {
  if (opened_) {
    va_list args;
    va_start(args, fmt);
    vsyslog(priority, fmt, args);
    va_end(args);
  }
}

void Log::Debug(const char* fmt, ...) const {
  if (opened_) {
    va_list args;
    va_start(args, fmt);
    vsyslog(LOG_DEBUG, fmt, args);
    va_end(args);
  }
}

void Log::Close() const {
  if (opened_)
    closelog();
}

}   // namespace utils
