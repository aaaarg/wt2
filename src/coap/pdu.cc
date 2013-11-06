// Copyleft 2013 tho@autistici.org

#include <cassert>

#include "utils/log.h"
#include "coap/proto.h"
#include "coap/options.h"
#include "coap/pdu.h"

namespace coap {

bool PDU::PayloadFits(size_t heading_size, size_t payload_size) const {
  if (heading_size >= max_message_size_)
    return false;

  return payload_size < (max_message_size_ - heading_size);
}

bool PDU::Encode(std::vector<uint8_t>& buf) const {
  utils::Log* L = utils::Log::Instance();

  // Mandatory header
  if (!EncodeHeader(buf))
    return false;

  // Optional options
  if (options_.count() > 0 && !options_.Encode(buf))
    return false;

  // Optional payload
  if (payload_.size() > 0) {
    // Must fit the currently set message limit.
    if (PayloadFits(buf.size(), payload_.size())) {
      // Add payload marker followed by payload bytes.
      buf.push_back(0xFF);
      std::copy(payload_.begin(), payload_.end(), std::back_inserter(buf));
    } else {
      L->Debug("message limits (%zu) overrun", max_message_size_);
      return false;
    }
  }

  return true;
}

bool PDU::Decode(const std::vector<uint8_t>& buf) {
  size_t offset = 0;

  if (!DecodeHeader(buf, offset))
    return false;

  if (offset >= buf.size()) {
    assert(offset == buf.size());
    return true;
  }

  if (!options_.Decode(buf, offset))
    return false;

  if (offset >= buf.size()) {
    assert(offset == buf.size());
    return true;
  }

  // Copy-in the payload (i.e. everything starting from the current offset
  // up to the end of the PDU buffer.
  std::copy(buf.begin() + offset, buf.end(), std::back_inserter(payload_));

  return true;
}

// Serialise header to the end of the given unsigned char buffer
// (Also add Token which is not strictly header.)
bool PDU::EncodeHeader(std::vector<uint8_t>& buf) const {
  //  0 1 2 3 4 5 6 7
  // +-+-+-+-+-+-+-+-+
  // | V | T |  TKL  |
  // +-+-+-+-+-+-+-+-+
  buf.push_back(
    ((static_cast<uint8_t>(version_) & 0x03) << 6) |
    ((static_cast<uint8_t>(code_) & 0x03) << 4) |
    (token_.size() & 0x0F));

  //  8 9 0 1 2 3 4 5
  // +-+-+-+-+-+-+-+-+
  // |     Code      |
  // +-+-+-+-+-+-+-+-+
  buf.push_back(static_cast<uint8_t>(code_));

  //  6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  // |          Message ID           |
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  buf.push_back((htons(message_id_) & 0xFF00) >> 8);
  buf.push_back((htons(message_id_) & 0x00FF));

  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  // |   Token (if any, TKL bytes) ...
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  if (token_.size() > 0)
    std::copy(token_.begin(), token_.end(), std::back_inserter(buf));

  return true;
}

// Parse header from the given unsigned char vector source starting
// from offset.  On success the offset indicator is updated to point
// one past the last decoded byte.
bool PDU::DecodeHeader(const std::vector<uint8_t>& buf, size_t& offset) {
  utils::Log* L = utils::Log::Instance();

  try {
    // (See EncodeHeader for pics.)

    // Version.
    if (((buf.at(0) & 0xC0) >> 6) != Version::v1) {
      L->Debug("PDU carries an unknown version");
      return false;
    }

    version_ = Version::v1;

    // Type (safe to cast, all values are mapped).
    type_ = static_cast<Type>((buf.at(0) & 0x30) >> 4);

    // Token length.
    if ((token_length_ = (buf.at(0) & 0x0F)) > 8) {
      L->Debug("invalid token length (%u)", token_length_);
      return false;
    }

    // Code is tricky, there are holes.
    if (!IsValidCode(buf.at(1))) {
      L->Debug("unknown code (%u)", buf.at(1));
      return false;
    }

    code_ = static_cast<Code>(buf.at(1));

    // Message Id.
    message_id_ = ntohs((buf.at(2) << 8) | buf.at(3));

    // We've already checked that buf is at least 4 + token_length.
    if (token_length_ > 0)
      std::copy(&buf[4], &buf[4 + token_length_],
                std::back_inserter(token_));

    // Update offset.
    offset += 4 + token_length_;

    return true;
  } catch (const std::out_of_range& e) {
    L->Debug("header decoding failed: %s", e.what());
    return false;
  }
}

std::ostream& operator<< (std::ostream& out, const PDU& pdu) {
  out << "| Ver: " << std::to_string(static_cast<uint8_t>(pdu.version_))
      << " | T: " << std::to_string(static_cast<uint8_t>(pdu.type_))
      << " | TKL: " << std::to_string(pdu.token_length_)
      << " | Code: " << std::to_string(static_cast<uint8_t>(pdu.code_))
      << " | Message ID: " << std::to_string(pdu.message_id_)
      << " |";

  return out;
}

}   // namespace coap
