// Copyleft 2013 tho@autistici.org

#ifndef COAP_OPTIONS_H_
#define COAP_OPTIONS_H_

#include <arpa/inet.h>
#include <err.h>

#include <tuple>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <stdexcept>

#include "utils/log.h"
#include "coap/proto.h"
#include "coap/optstore.h"

namespace coap {

class Option {
 public:
  Option()
    : type_(OptionFormat::unset)
  { }

  ~Option() { }

  bool IsPayloadMarker() const;
  void MakePayloadMarker();

  bool set_num(OptionNumber num) {
    // TODO lookup store
    return true; 
  };

  void set_value(uint64_t v);
  void set_value(const std::string& v);
  void set_value(const std::vector<uint8_t>& v);
  void set_value();

  bool value_string(std::string& v);
  bool value_uint(uint64_t& v);
  bool value_opaque(std::vector<uint8_t>& v);
  void value(std::vector<uint8_t>& v);

  bool Decode(size_t&obase, const std::vector<uint8_t>& buf, size_t& offset);
  bool Encode(size_t&obase, std::vector<uint8_t>& buf) const;

 private:
  bool DecodeExtended(const std::vector<uint8_t>&, size_t&, size_t&);

 private:
  size_t num_;
  OptionFormat type_;
  std::vector<uint8_t> raw_;
};

class Options {
  // TODO(tho)
  // interfaces:
  //  add(name==number, value)
  //  encode(to vector)
  //  decode(from vector)
  //  get(name==number) -> value
  //  iterator (provides ordered walk)
  // internals:
  //  std::multimap<name==number, value>  // allows repetition
  //  option value is conceptually an union or a polymorphic object (int, string, opaque)
};

}   // namespace coap

#endif  // COAP_OPTIONS_H_
