// Copyleft 2013 tho@autistici.org

#include <cassert>
#include <vector>
#include "utils/log.h"

void test_xxx(utils::Log* L) {
  std::vector<int> priorities = {
    LOG_DEBUG,
    LOG_INFO,
    LOG_NOTICE,
    LOG_WARNING,
    LOG_ERR,
    LOG_CRIT,
    LOG_ALERT,
    LOG_EMERG
  };

  for (auto priority : priorities)
    L->Write(priority, "test %s: %d", "me", priority);
}

int main() {
  utils::Log* L = utils::Log::Instance();

  const char* ident = "log_unittest";
  int logopt = LOG_PERROR | LOG_NDELAY;
  int facility = LOG_LOCAL0;

  L->Open(ident, logopt, facility);

  test_xxx(L);

  L->Close();
}
