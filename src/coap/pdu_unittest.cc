// Copyleft 2013 tho@autistici.org

#include <cassert>
#include "coap/pdu.h"

void init_log() {
  utils::Log::Instance()->Open("options_unittest",
                               LOG_PERROR | LOG_NDELAY,
                               LOG_LOCAL0);
}

void test_ok_codec() {
  coap::PDU pdu1;

  std::vector<uint8_t> token{ 'a', 'b', 'c', 'd' };   // NOLINT
  pdu1.set_token(token);

  std::vector<uint8_t> bin;
  pdu1.EncodeHeader(bin);

  coap::PDU pdu2;
  size_t offset = 0;

  assert(pdu2.DecodeHeader(bin, offset));
  assert(pdu1.token() == pdu2.token());
  assert(token == pdu2.token());
  assert(offset == 4 + token.size());
}

void test_ko_unsupported_version() {
  std::vector<std::vector<uint8_t>> bins { 
    { 0x00 }, // 0
    { 0x80 }, // 2
    { 0xC0 }  // 3
  };

  for (auto bin : bins) {
    coap::PDU pdu;
    size_t offset = 0;
    assert(!pdu.DecodeHeader(bin, offset));
  }
}

void test_ko_unknown_code() {
  std::vector<std::vector<uint8_t>> bins { };

  for (auto bin : bins) {
    ;
  }
}

int main() {
  init_log();

  test_ok_codec();

  test_ko_unsupported_version();
  test_ko_unknown_code();
}
