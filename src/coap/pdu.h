// Copyleft 2013 tho@autistici.org

#ifndef COAP_PDU_H_
#define COAP_PDU_H_

#include <arpa/inet.h>
#include <err.h>

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <stdexcept>

#include "utils/log.h"
#include "coap/proto.h"
#include "coap/options.h"

namespace coap {

class PDU {
 public:
  // Construct a confirmable empty message by default
  PDU()
    : version_(Version::v1)
    , type_(Type::CON)
    , token_length_(0)
    , code_(Code::Empty)
    , message_id_(0)
    , token_()
    , options_()
  { }

  // Construct from binary
  explicit PDU(const std::vector<uint8_t>& bin)
    : raw_(bin)
  { }

  ~PDU() { }

  // Header fields getter's
  Version version() const { return version_; }
  Type type() const { return type_; }
  Code code() const { return code_; }
  uint8_t token_length() const { return token_.size(); }
  uint16_t message_id() const { return message_id_; }
  Options options() const { return options_; }
  std::vector<uint8_t> token() const { return token_; }

  // Header fields setter's
  void set_version(Version v) { version_ = v; }
  void set_type(Type v) { type_ = v; }
  void set_message_id(uint16_t v) { message_id_ = v; }
  void set_code(Code v) { code_ = v; }
  void set_token(const std::vector<uint8_t>& token) {
    // Copy at most 8 bytes.
    std::copy_n(token.begin(),
        std::min(8UL, token.size()),
        std::back_inserter(token_));
  }

  // Serialise header to the end of the given unsigned char buffer
  // (Also add Token which is not strictly header.)
  bool EncodeHeader(std::vector<uint8_t>& buf) const;

  // Parse header from the given unsigned char vector source starting
  // from offset.  On success the offset indicator is updated to point
  // one past the last decoded byte.
  bool DecodeHeader(const std::vector<uint8_t>& buf, size_t& offset);

  friend std::ostream& operator<< (std::ostream&, const PDU&);

 private:
  std::vector<uint8_t> raw_;

  // Fixed Header fields
  Version version_;
  Type type_;
  uint8_t token_length_;
  Code code_;
  uint16_t message_id_;

  // Optional token
  std::vector<uint8_t> token_;

  Options options_;
};

}   // namespace coap

#endif  // COAP_PDU_H_
