// Copyleft 2013 tho@autistici.org

#include <cassert>
#include <iostream>
#include "coap/optstore.h"

using namespace coap;

void test_print() {
  // A fancy header...
  std::cout << "No ; C ; U; N; R; Name; Format; Length; Default;\n";

  for (auto it = OptStore.begin(); it != OptStore.end(); ++it) {
    // just a bit self-assertive :-)
    assert(OptStore.find(it->first) != OptStore.end());

    // print option properties
    auto prop = it->second;
    std::cout << prop.code() << "; "
              << prop.critical() << "; "
              << prop.unsafe() << "; "
              << prop.no_cache_key() << "; "
              << prop.repeatable() << "; "
              << prop.name() << "; "
              << static_cast<unsigned int>(prop.format()) << "; "
              << prop.min_length() << "-" << prop.max_length() << "; "
              << ((prop.default_value() != nullptr)
                  ? prop.default_value()
                  : "(none)")
              << ";\n";
  }
}

int main() {
  test_print();
}
