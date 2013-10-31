// Copyleft 2013 tho@autistici.org

#ifndef COAP_OPTSTORE_H_
#define COAP_OPTSTORE_H_

#include <map>
#include <vector>

namespace coap {

// +-----+----+---+---+---+----------------+--------+--------+---------+
// | No. | C  | U | N | R | Name           | Format | Length | Default |
// +-----+----+---+---+---+----------------+--------+--------+---------+
// |   1 | x  |   |   | x | If-Match       | opaque | 0-8    | (none)  |
// |   3 | x  | x | - |   | Uri-Host       | string | 1-255  | (see    |
// |     |    |   |   |   |                |        |        | below)  |
// |   4 |    |   |   | x | ETag           | opaque | 1-8    | (none)  |
// |   5 | x  |   |   |   | If-None-Match  | empty  | 0      | (none)  |
// |   7 | x  | x | - |   | Uri-Port       | uint   | 0-2    | (see    |
// |     |    |   |   |   |                |        |        | below)  |
// |   8 |    |   |   | x | Location-Path  | string | 0-255  | (none)  |
// |  11 | x  | x | - | x | Uri-Path       | string | 0-255  | (none)  |
// |  12 |    |   |   |   | Content-Format | uint   | 0-2    | (none)  |
// |  14 |    | x | - |   | Max-Age        | uint   | 0-4    | 60      |
// |  15 | x  | x | - | x | Uri-Query      | string | 0-255  | (none)  |
// |  17 | x  |   |   |   | Accept         | uint   | 0-2    | (none)  |
// |  20 |    |   |   | x | Location-Query | string | 0-255  | (none)  |
// |  35 | x  | x | - |   | Proxy-Uri      | string | 1-1034 | (none)  |
// |  39 | x  | x | - |   | Proxy-Scheme   | string | 1-255  | (none)  |
// |  60 |    |   | x |   | Size1          | uint   | 0-4    | (none)  |
// +-----+----+---+---+---+----------------+--------+--------+---------+
enum class OptionFormat {
  unset,
  marker,
  string,
  uint,
  opaque,
  empty
};

enum OptionNumber {
  If_Match = 1,
  Uri_Host = 3,
  ETag = 4,
  If_None_Match = 5,
  Uri_Port = 7,
  Location_Path = 8,
  Uri_Path = 11,
  Content_Format = 12,
  Max_Age = 14,
  Uri_Query = 15,
  Accept = 17,
  Location_Query = 20,
  Proxy_Uri = 35,
  Proxy_Scheme = 39,
  Size1 = 60
};

class OptProp {
 public:
  OptProp(OptionNumber code, bool repeatable, const char* name,
          OptionFormat format, size_t min_length, size_t max_length,
          const char* default_value)
    : code_(code)
    , repeatable_(repeatable)
    , name_(name)
    , format_(format)
    , min_length_(min_length)
    , max_length_(max_length)
    , default_value_(default_value)
  { }

  OptionNumber code() const { return code_; }
  bool critical() const { return (code_ & 1); }
  bool unsafe() const { return (code_ & 2); }
  bool no_cache_key() const { return (code_ & 0x1E) == 0x1C; }
  bool repeatable() const { return repeatable_; }
  const char* name() const { return name_; }
  OptionFormat format() const { return format_; }
  const char* default_value() const { return default_value_; }
  size_t min_length() const { return min_length_; }
  size_t max_length() const { return max_length_; }

 private:
  OptionNumber code_;
  bool repeatable_;
  const char* name_;
  OptionFormat format_;
  size_t min_length_;
  size_t max_length_;
  const char* default_value_;
};

const std::map<OptionNumber, OptProp> OptStore {
  {
    OptionNumber::If_Match,
    {
      OptionNumber::If_Match, // No.
      true,                   // Repeatable
      "If-Match",             // mnemonic
      OptionFormat::opaque,   // Format
      0,                      // min-length
      8,                      // max-length
      nullptr                 // Default
    }
  },

  {
    OptionNumber::Uri_Host,
    {
      OptionNumber::Uri_Host, // No.
      false,                  // Repeatable
      "Uri-Host",             // mnemonic
      OptionFormat::string,   // Format
      1,                      // min-length
      255,                    // max-length
      nullptr                 // Default
    }
  },

  {
    OptionNumber::ETag,
    {
      OptionNumber::ETag,     // No.
      true,                   // Repeatable
      "ETag",                 // mnemonic
      OptionFormat::opaque,   // Format
      1,                      // min-length
      8,                      // max-length
      nullptr                 // Default
    }
  },

  {
    OptionNumber::If_None_Match,
    {
      OptionNumber::If_None_Match,  // No.
      false,                        // Repeatable
      "If-None-Match",              // mnemonic
      OptionFormat::empty,          // Format
      0,                            // min-length
      0,                            // max-length
      nullptr                       // Default
    }
  },

  {
    OptionNumber::Uri_Port,
    {
      OptionNumber::Uri_Port,   // No.
      false,                    // Repeatable
      "Uri-Port",               // mnemonic
      OptionFormat::uint,       // Format
      0,                        // min-length
      2,                        // max-length
      nullptr                   // Default
    }
  },

  {
    OptionNumber::Location_Path,
    {
      OptionNumber::Location_Path,  // No.
      true,                         // Repeatable
      "Location-Path",              // mnemonic
      OptionFormat::string,         // Format
      0,                            // min-length
      255,                          // max-length
      nullptr                       // Default
    }
  },

  {
    OptionNumber::Uri_Path,
    {
      OptionNumber::Uri_Path,   // No.
      true,                     // Repeatable
      "Uri-Path",               // mnemonic
      OptionFormat::string,     // Format
      0,                        // min-length
      255,                      // max-length
      nullptr                   // Default
    }
  },

  {
    OptionNumber::Content_Format,
    {
      OptionNumber::Content_Format,   // No.
      false,                          // Repeatable
      "Content-Format",               // mnemonic
      OptionFormat::uint,             // Format
      0,                              // min-length
      2,                              // max-length
      nullptr                         // Default
    }
  },

  {
    OptionNumber::Max_Age,
    {
      OptionNumber::Max_Age,  // No.
      false,                  // Repeatable
      "Max-Age",              // mnemonic
      OptionFormat::uint,     // Format
      0,                      // min-length
      4,                      // max-length
      "60"                    // Default
    }
  },

  {
    OptionNumber::Uri_Query,
    {
      OptionNumber::Uri_Query,  // No.
      true,                     // Repeatable
      "Uri-Query",              // mnemonic
      OptionFormat::string,     // Format
      0,                        // min-length
      255,                      // max-length
      nullptr                   // Default
    }
  },

  {
    OptionNumber::Accept,
    {
      OptionNumber::Accept,   // No.
      false,                  // Repeatable
      "Accept",               // mnemonic
      OptionFormat::uint,     // Format
      0,                      // min-length
      2,                      // max-length
      nullptr                 // Default
    }
  },

  {
    OptionNumber::Location_Query,
    {
      OptionNumber::Location_Query,   // No.
      true,                           // Repeatable
      "Location-Query",               // mnemonic
      OptionFormat::string,           // Format
      0,                              // min-length
      255,                            // max-length
      nullptr                         // Default
    }
  },

  {
    OptionNumber::Proxy_Uri,
    {
      OptionNumber::Proxy_Uri,    // No.
      false,                      // Repeatable
      "Proxy-Uri",                // mnemonic
      OptionFormat::string,       // Format
      1,                          // min-length
      1034,                       // max-length
      nullptr                     // Default
    }
  },

  {
    OptionNumber::Proxy_Scheme,
    {
      OptionNumber::Proxy_Scheme,   // No.
      false,                        // Repeatable
      "Proxy-Scheme",               // mnemonic
      OptionFormat::string,         // Format
      1,                            // min-length
      255,                          // max-length
      nullptr                       // Default
    }
  },

  {
    OptionNumber::Size1,
    {
      OptionNumber::Size1,  // No.
      false,                // Repeatable
      "Size1",              // mnemonic
      OptionFormat::uint,   // Format
      0,                    // min-length
      4,                    // max-length
      nullptr               // Default
    }
  },
};


}   // namespace coap

#endif  // COAP_OPTSTORE_H_
