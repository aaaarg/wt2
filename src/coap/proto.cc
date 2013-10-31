// Copyleft 2013 tho@autistici.org

#include "coap/proto.h"

namespace coap {

bool IsValidCode(uint8_t code) {
  switch (code) {
    case Code::Empty:
    case Code::GET:
    case Code::POST:
    case Code::PUT:
    case Code::DELETE:
    case Code::Created:
    case Code::Deleted:
    case Code::Valid:
    case Code::Changed:
    case Code::Content:
    case Code::BadRequest:
    case Code::Unauthorized:
    case Code::BadOption:
    case Code::Forbidden:
    case Code::NotFound:
    case Code::MethodNotAllowed:
    case Code::NotAcceptable:
    case Code::PreconditionFailed:
    case Code::RequestEntityTooLarge:
    case Code::UnsupportedContentFormat:
    case Code::InternalServerError:
    case Code::NotImplemented:
    case Code::BadGateway:
    case Code::ServiceUnavailable:
    case Code::GatewayTimeout:
    case Code::ProxyingNotSupported:
      return true;
  }
  return false;
}

}  // namespace coap
