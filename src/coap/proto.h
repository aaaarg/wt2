// Copyleft 2013 tho@autistici.org

#ifndef COAP_PROTO_H_
#define COAP_PROTO_H_

#include <stdint.h>

namespace coap {

enum Version {
  // "Implementations of this specification MUST set this field
  //  to 1 (01 binary).  Other values are reserved for future versions.
  //  Messages with unknown version numbers MUST be silently ignored."
  v1 = 1
};

enum Type : uint8_t {
  CON = 0,
  NON = 1,
  ACK = 2,
  RST = 3
};

enum CodeBlocks {
  // Methods
  ReqMethodMin = 1,
  ReqMethodMax = 31,

  // 2.xx
  RespSuccessMin = 64,
  RespSuccessMax = RespSuccessMin + 31,

  // 4.xx
  RespClientErrorMin = 128,
  RespClientErrorMax = RespClientErrorMin + 31,

  // 5.xx
  RespServerErrorMin = 160,
  RespServerErrorMax = RespServerErrorMin + 31
};

// MUST be kept in sync with IsValidCode().
enum Code {
  // Empty message code
  Empty                     = 0,          // 0.00

  // Request methods
  GET                       = 1,          // 0.01
  POST                      = 2,          // 0.02
  PUT                       = 3,          // 0.03
  DELETE                    = 4,          // 0.04

  // Success status code
  Created                   = 64 + 1,     // 2.01
  Deleted                   = 64 + 2,     // 2.02
  Valid                     = 64 + 3,     // 2.03
  Changed                   = 64 + 4,     // 2.04
  Content                   = 64 + 5,     // 2.05

  // Client Error status code
  BadRequest                = 128 + 0,    // 4.00
  Unauthorized              = 128 + 1,    // 4.01
  BadOption                 = 128 + 2,    // 4.02
  Forbidden                 = 128 + 3,    // 4.03
  NotFound                  = 128 + 4,    // 4.04
  MethodNotAllowed          = 128 + 5,    // 4.05
  NotAcceptable             = 128 + 6,    // 4.06
  PreconditionFailed        = 128 + 12,   // 4.12
  RequestEntityTooLarge     = 128 + 13,   // 4.13
  UnsupportedContentFormat  = 128 + 15,   // 4.15

  // Server Error status code
  InternalServerError       = 160 + 0,    // 5.00
  NotImplemented            = 160 + 1,    // 5.01
  BadGateway                = 160 + 2,    // 5.02
  ServiceUnavailable        = 160 + 3,    // 5.03
  GatewayTimeout            = 160 + 4,    // 5.04
  ProxyingNotSupported      = 160 + 5,    // 5.05
};

bool IsValidCode(uint8_t code);

}  // namespace coap

#endif  // COAP_PROTO_H_
