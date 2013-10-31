// Copyleft 2013 tho@autistici.org

#include "utils/log.h"
#include "coap/options.h"

namespace coap {

bool Option::Encode(size_t& option_base, std::vector<uint8_t>& buf) const {
  utils::Log* L = utils::Log::Instance();

  // handle payload marker
  if (type_ == OptionFormat::marker) {
    buf.push_back(0xFF);
    return true;
  }

  size_t delta = num_ - option_base;
  size_t length = raw_.size();
  size_t base = buf.size();

  // Encode delta
  if (delta <= 12) {
    buf.push_back(delta << 4);
  } else if (delta >= 13 && delta <= 268) {
    buf.push_back(13 << 4);
    buf.push_back(delta - 13);
  } else if (delta >= 269 && delta <= (65535 + 269)) {
    buf.push_back(14 << 4);
    buf.push_back(delta - 269);
  } else {
    L->Debug("encoding failed: delta is out-of-range (%zu)", delta);
    return false;
  }

  // Encode length
  if (length <= 12) {
    buf[base] |= length;
  } else if (length >= 13 && length <= 268) {
    buf[base] |= 13;
    buf.push_back(length - 13);
  } else if (length >= 269 && length <= (65535 + 269)) {
    buf[base] |= 14;
    buf.push_back(length - 269);
  } else {
    L->Debug("encoding failed: length is out-of-range (%zu)", length);
    return false;
  }
 
  return true;
}

// Decode option starting at offset in buf.  The absolute option number is
// computed by adding the decoded delta to option_base.
// On success offset is updated to point to the first undecoded byte.
bool Option::Decode(size_t& option_base, const std::vector<uint8_t>& buf,
                    size_t& offset) {
  utils::Log* L = utils::Log::Instance();

  try {
    //      0   1   2   3   4   5   6   7
    //    +---------------+---------------+
    //    |               |               |
    //    |  Option Delta | Option Length |   1 byte
    //    |               |               |
    //    +---------------+---------------+
    //
    size_t delta = (buf.at(offset) & 0xF0) >> 4;
    size_t length = buf.at(offset) & 0x0F;

    offset += 1;

    // Check whether we are on a payload marker.
    // Return true, and let the caller test the "IsPayloadMarker"
    // condition.
    if (delta == 0xF) {
      // "Reserved for the Payload Marker.  If the field is set to
      //  this value but the entire byte is not the payload marker,
      //  this MUST be processed as a message format error."
      if (length == 0xF) {
        type_ = OptionFormat::marker;
        return true;
      }
      L->Debug("badly formatted payload marker (0x%x%x)", delta, length);
      return false;
    }

    // Overwrite delta if needed.
    if (delta >= 13)
      if (!DecodeExtended(buf, offset, delta))
        return false;

    // Update option base and set option number.
    option_base += delta;
    num_ = option_base;

    // Look up option properties.
    auto prop_it = OptStore.find(static_cast<OptionNumber>(num_));

    // Handle unknown options.
    if (prop_it == OptStore.end()) {
      L->Debug("unknown option number (%d)", num_);
      return false;
    }

    // Overwrite length if needed.
    if (length >= 13)
      if (!DecodeExtended(buf, offset, length))
        return false;

    // Check given length bounds against Option properties.
    auto prop = prop_it->second;
    if (length > prop.max_length() || length < prop.min_length()) {
      L->Debug("%s length out of range: %zu", prop.name(), length);
      return false;
    }

    //    +-------------------------------+
    //    \                               \
    //    /                               /
    //    \                               \
    //    /         Option Value          /   0 or more bytes
    //    \                               \
    //    /                               /
    //    \                               \
    //    +-------------------------------+
    //
    // So far everything OK.  Now make sure we have been given enough bytes
    // and then copy in the Option Value.
    if (length) {
      if (buf.size() < offset + length) {
        L->Debug("option length is %zu but only %zu byte(s) left",
                 length, buf.size() - offset);
        return false;
      }
      std::copy(&buf[offset], &buf[offset + length],
                std::back_inserter(raw_));
    }

    return true;
  } catch (const std::out_of_range& e) {
    L->Debug("option decoding failed: %s", e.what());
    return false;
  }
}

// Try to decode option's delta or length.
// XXX Expect the caller to catch out_of_range exceptions.
bool Option::DecodeExtended(const std::vector<uint8_t>& buf, size_t& offset,
                            size_t& dl) {

  // +---------------+---------------+
  // \                               \
  // /         Option Delta          /   0-2 bytes
  // \          (extended)           \
  // +-------------------------------+
  // \                               \
  // /         Option Length         /   0-2 bytes
  // \          (extended)           \
  // +-------------------------------+
  switch (dl) {   // XXX check value is >= minuend !!!
    case 13:  // extended format: 1 byte
      dl = buf.at(offset + 1) - 13;
      offset += 1;
      break;
    case 14:  // extended format: 2 bytes
      dl = ntohs((buf.at(offset + 1) << 8) | buf.at(offset + 2)) - 269;
      offset += 2;
      break;
    default:
      return false;
  }

  return true;
}

bool Option::IsPayloadMarker() const {
  return type_ == OptionFormat::marker;
}

void Option::set_value(uint64_t v) {
  type_ = OptionFormat::uint;

  uint64_t ui_bytes[] = {
    (1ULL <<  8) - 1,
    (1ULL << 16) - 1,
    (1ULL << 24) - 1,
    (1ULL << 32) - 1,
    (1ULL << 40) - 1,
    (1ULL << 48) - 1,
    (1ULL << 56) - 1,
    UINT64_MAX
  };

  // Special case 0:
  // "the number 0 is represented with an empty option value (a zero-length
  //  sequence of bytes)"
  if (v == 0) {
    raw_.resize(0); // XXX is this needed?
    return;
  }

  size_t needed_bytes = 0;

  // Pick size:
  // "a sender SHOULD represent the integer with as few bytes as possible".
  for (size_t i = 0; i < (sizeof ui_bytes / sizeof(uint64_t)); ++i) {
    if (ui_bytes[i] >= v) {
      needed_bytes = i + 1;
      break;
    }
  }

  raw_.resize(needed_bytes);

  for (size_t j = 0; j < needed_bytes; ++j) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    raw_[j] = (v >> (8 * j)) & 0xff;
#else
    raw_[needed_bytes - (1 + j)] = (v >> (8 * j)) & 0xff;
#endif  // __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  }
}

void Option::set_value(const std::string& v) {
  type_ = OptionFormat::string;
  std::copy(v.begin(), v.end(), std::back_inserter(raw_));
}

void Option::set_value(const std::vector<uint8_t>& v) {
  type_ = OptionFormat::opaque;
  raw_ = v;
}

void Option::set_value() {
  type_ = OptionFormat::empty;
}

void Option::MakePayloadMarker() {
  type_ = OptionFormat::marker;
}

bool Option::value_string(std::string& v) {
  if (type_ != OptionFormat::string)
    return false;

  v.clear();
  std::copy(raw_.begin(), raw_.end(), std::back_inserter(v));

  return true;
}

bool Option::value_uint(uint64_t& ui) {
  if (type_ != OptionFormat::uint)
    return false;

  size_t nbytes = raw_.size();

  if (nbytes > sizeof(uint64_t))
    return false;

  if (nbytes == 0) {
    ui = 0;
    return true;
  }

  ui = 0;

  for (size_t j = 0; j < nbytes; ++j) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    ui |= static_cast<int64_t>(raw_[j]) << (8 * j);
#else
    ui |= static_cast<uint64_t>(raw_[j]) << (8 * (nbytes - (j + 1)));
#endif  // __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  }

  return true;
}

bool Option::value_opaque(std::vector<uint8_t>& v) {
  if (type_ != OptionFormat::opaque)
    return false;
  v = raw_;
  return true;
}

void Option::value(std::vector<uint8_t>& v) {
  v = raw_;
}

}   // namespace coap
