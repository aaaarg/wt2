// Copyleft 2013 tho@autistici.org

#include <cassert>
#include "coap/pdu.h"

using namespace coap;

void init_log() {
  utils::Log::Instance()->Open("options_unittest",
                               LOG_PERROR | LOG_NDELAY,
                               LOG_LOCAL0);
}

void test_ok_codec() {
  PDU pdu1;

  std::vector<uint8_t> token{ 'a', 'b', 'c', 'd' };   // NOLINT
  pdu1.set_token(token);

  Options opts1;
  opts1.AddUriHost("s.example.org");
  pdu1.set_options(opts1);

  std::vector<uint8_t> pload { 'p', 'l', 'o', 'a', 'd' };
  pdu1.set_payload(pload);

  std::vector<uint8_t> pkt;
  pdu1.Encode(pkt);

  PDU pdu2;

  assert(pdu2.Decode(pkt));
  assert(pdu1.token() == pdu2.token());
  assert(token == pdu2.token());
  std::vector<Option> res_set;
  assert(pdu2.options().LookUp(Uri_Host, res_set));
  assert(res_set.size() == 1);
  std::string uri_host;
  assert(res_set[0].value_string(uri_host));
  assert(uri_host == "s.example.org");
  assert(pdu2.payload() == pload);

  //assert(offset == 4 + token.size());
}

void test_ko_unsupported_version() {
  std::vector<std::vector<uint8_t>> bins {
    { 0x00 }, // 0
    { 0x80 }, // 2
    { 0xC0 }  // 3
  };

  for (auto bin : bins) {
    PDU pdu;
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
