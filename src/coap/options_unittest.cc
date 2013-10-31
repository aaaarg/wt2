// Copyleft 2013 tho@autistici.org

#include <cassert>
#include "utils/log.h"
#include "coap/options.h"

void init_log() {
  utils::Log::Instance()->Open("options_unittest",
                               LOG_PERROR | LOG_NDELAY,
                               LOG_LOCAL0);
}

void test_ok_set_integer() {

  std::vector<uint64_t> uis {
    0, 1, 2, 10, 100,
    (1ULL <<  8) - 1,
    (1ULL <<  8),
    (1ULL <<  8) + 1,
    (1ULL << 16) - 1,
    (1ULL << 16),
    (1ULL << 16) + 1,
    (1ULL << 24) - 1,
    (1ULL << 24),
    (1ULL << 24) + 1,
    (1ULL << 32) - 1,
    (1ULL << 32),
    (1ULL << 32) + 1,
    (1ULL << 40) - 1,
    (1ULL << 40),
    (1ULL << 40) + 1,
    (1ULL << 48) - 1,
    (1ULL << 48),
    (1ULL << 48) + 1,
    (1ULL << 56) - 1,
    (1ULL << 56),
    (1ULL << 56) + 1,
    UINT64_MAX - 1,
    UINT64_MAX
  };

  for (auto ui : uis) {
    coap::Option opt;
    uint64_t ui_set = ui;
    opt.set_value(ui_set);

    uint64_t ui_get;
    assert(opt.value_uint(ui_get));
    assert(ui_get == ui_set);
  }
}

void test_ok_simple() {
  std::vector<uint8_t> buf { 
    0x11,   // delta=1, length=1
    0xFF    // value=0xFF
  };

  coap::Option opt;
  size_t opt_base = 0;
  size_t offset = 0;

  assert(opt.Decode(opt_base, buf, offset));

  std::vector<uint8_t> val;
  opt.value(val);
  assert(val.size() == 1);
  assert(val[0] == 0xFF);
  
  assert(opt_base == 1);
}

void test_ok_payload_marker() {
  std::vector<uint8_t> buf { 
    0xFF    // payload marker
  };

  coap::Option opt;
  size_t opt_base = 0;
  size_t offset = 0;

  assert(opt.Decode(opt_base, buf, offset));
  assert(opt.IsPayloadMarker());
  assert(offset == 1);
  assert(opt_base == 0);
}

void test_ok_codec() {
  coap::Option opt;
  // TODO
}

void test_ko_bad_payload_marker() {

  for (uint8_t ui = 0xF0; ui <= 0xFE; ++ui) {
    coap::Option opt;
    std::vector<uint8_t> buf = { ui };
    size_t offset = 0;
    size_t opt_base = 0;

    assert(!opt.Decode(opt_base, buf, offset));
  }
}

void test_ko_bad_length() {
  std::vector<uint8_t> buf { 
    0x12,   // delta=1, length=2
    0xFF    // value=0xFF
  };

  coap::Option opt;
  size_t opt_base = 0;
  size_t offset = 0;

  assert(!opt.Decode(opt_base, buf, offset));
}

void test_ko_unknown_option() {
  std::vector<uint8_t> buf { 
    0x22,   // delta=2, length=2
    0xFF, 0xFF
  };

  coap::Option opt;
  size_t opt_base = 0;
  size_t offset = 0;

  assert(!opt.Decode(opt_base, buf, offset));
}

void test_ko_out_of_range_length() {
  // Uri-Port with a length of 3
  std::vector<uint8_t> buf { 
    0x73,   // delta=7, length=3
    0xFF, 0xFF, 0xFF
  };

  coap::Option opt;
  size_t opt_base = 0;
  size_t offset = 0;

  assert(!opt.Decode(opt_base, buf, offset));
}

int main() {
  init_log();

  test_ok_set_integer();
  test_ok_simple();
  test_ok_payload_marker();

  test_ko_bad_length();
  test_ko_bad_payload_marker();
  test_ko_unknown_option();
  test_ko_out_of_range_length();
}
