// Copyleft 2013 tho@autistici.org

#include <iomanip>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cassert>
#include "utils/log.h"
#include "coap/options.h"

using namespace coap;

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
    Option opt;
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

  Option opt;
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

  Option opt;
  size_t opt_base = 0;
  size_t offset = 0;

  assert(opt.Decode(opt_base, buf, offset));
  assert(opt.IsPayloadMarker());
  assert(offset == 1);
  assert(opt_base == 0);
}

void test_ok_codec() {
  Option opt1;
  std::vector<uint8_t> etag1{0xAB, 0xCD}; // NOLINT
  opt1.set_num(If_Match);
  opt1.set_format(OptionFormat::opaque);
  opt1.set_value(etag1);

  std::vector<uint8_t> bin;
  size_t opt_base = 0;
  assert(opt1.Encode(opt_base, bin));
  assert(opt_base == 1);

  opt_base = 0; // reset

  size_t offset = 0;
  Option opt2;
  assert(opt2.Decode(opt_base, bin, offset));
  assert(opt2.format() == OptionFormat::opaque);
  assert(opt2.num() == If_Match);
  std::vector<uint8_t> etag2;
  assert(opt2.value_opaque(etag2));
  assert(etag1 == etag2);
}

void test_ko_decode_bad_payload_marker() {

  for (uint8_t ui = 0xF0; ui <= 0xFE; ++ui) {
    Option opt;
    std::vector<uint8_t> buf = { ui };
    size_t offset = 0;
    size_t opt_base = 0;

    assert(!opt.Decode(opt_base, buf, offset));
  }
}

void test_ko_decode_bad_length() {
  std::vector<uint8_t> buf {
    0x12,   // delta=1, length=2
    0xFF    // value=0xFF
  };

  Option opt;
  size_t opt_base = 0;
  size_t offset = 0;

  assert(!opt.Decode(opt_base, buf, offset));
}

void test_ko_decode_unknown_option() {
  std::vector<uint8_t> buf {
    0x22,   // delta=2, length=2
    0xFF, 0xFF
  };

  Option opt;
  size_t opt_base = 0;
  size_t offset = 0;

  assert(!opt.Decode(opt_base, buf, offset));
}

void test_ko_decode_out_of_range_length() {
  // Uri-Port with a length of 3
  std::vector<uint8_t> buf {
    0x73,   // delta=7, length=3
    0xFF, 0xFF, 0xFF
  };

  Option opt;
  size_t opt_base = 0;
  size_t offset = 0;

  assert(!opt.Decode(opt_base, buf, offset));
}

void test_ok_codec_multi() {
  Options opts1;
  std::vector<uint8_t> etag { 'A', 'B', 'C', 'D' };

  assert(opts1.AddIfMatch(etag));

  // Encode "coap://s.example.org:4567/dir/file?q=val"
  // (The Uri-Host string is exactly 13 bytes, which encodes an
  // extra lenght byte to 0x00.)
  assert(opts1.AddUriHost("s.example.org"));
  assert(opts1.AddUriPort(4567));
  assert(opts1.AddUriPath("dir"));
  assert(opts1.AddUriPath("file"));
  assert(opts1.AddUriQuery("q=val"));

  std::vector<uint8_t> buf;
  assert(opts1.Encode(buf));

  // Decode
  Options opts2;
  size_t offset = 0;
  assert(opts2.Decode(buf, offset));
  assert(opts2.count() == 6);

  // TODO get options and check values.
//  std::cout << opts1.count() << '\n';
//  for (auto it = opts1.begin(); it != opts1.end(); ++it)
//    std::cout << *it << '\n';

  std::vector<Option> optv;

  assert(opts2.LookUp(If_Match, optv));
  assert(optv.size() == 1);
  std::vector<uint8_t> etag2;
  optv[0].value_opaque(etag2);
  assert(etag == etag2);

  assert(opts2.LookUp(Uri_Host, optv));
  assert(optv.size() == 1);
  std::string s;
  optv[0].value_string(s);
  assert(s == "s.example.org");
  std::cout << s << '\n';

  assert(opts2.LookUp(Uri_Path, optv));
  assert(optv.size() == 2);
  for (auto it : optv) {
    it.value_string(s);
    assert(s == "dir" || s == "file");
    std::cout << s << '\n';
  }
}

void test_ok_add_multi_repeatable() {
  Options opts;
  assert(opts.AddUriPath("d1"));
  assert(opts.AddUriPath("d2"));
  assert(opts.AddUriPath("d3"));
  assert(opts.AddUriPath("res"));
}

void test_ko_add_multi_non_repeatable() {
  Options opts;
  assert(opts.AddUriHost("a.example.org"));
  assert(!opts.AddUriHost("b.example.org"));
  assert(!opts.AddUriHost("c.example.org"));
}

void test_ko_add_out_of_range_size() {
  Options opts;
  assert(!opts.AddAccept(UINT64_MAX));
}

int main() {
  init_log();

  test_ok_set_integer();
  test_ok_simple();
  test_ok_payload_marker();
  test_ok_codec();
  test_ok_codec_multi();
  test_ok_add_multi_repeatable();

  test_ko_decode_bad_length();
  test_ko_decode_bad_payload_marker();
  test_ko_decode_unknown_option();
  test_ko_decode_out_of_range_length();
  test_ko_add_multi_non_repeatable();
  test_ko_add_out_of_range_size();
}
