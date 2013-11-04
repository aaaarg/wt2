// Copyleft 2013 tho@autistici.org

#ifndef COAP_OPTIONS_H_
#define COAP_OPTIONS_H_

#include <arpa/inet.h>
#include <err.h>

#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <map>
#include <cassert>
#include <iostream>

#include "utils/log.h"
#include "coap/proto.h"
#include "coap/optstore.h"

namespace coap {

class Option {
 public:
  Option()
    : format_(OptionFormat::unset)
  { }

  ~Option() = default;
  Option (const Option&) = default;
  Option& operator= (const Option&) = default;	

  bool IsPayloadMarker() const;
  void MakePayloadMarker();

  bool set_num(OptionNumber num);

  void set_format(OptionFormat format);

  void set_value(uint64_t v);
  void set_value(const std::string& v);
  void set_value(const std::vector<uint8_t>& v);
  void set_value();

  bool value_string(std::string& v);
  bool value_uint(uint64_t& v);
  bool value_opaque(std::vector<uint8_t>& v);
  void value(std::vector<uint8_t>& v);

  OptionNumber num() const;
  OptionFormat format() const;

  bool Decode(size_t&obase, const std::vector<uint8_t>& buf, size_t& offset);
  bool Encode(size_t&obase, std::vector<uint8_t>& buf) const;

  friend std::ostream& operator<< (std::ostream&, const Option&);

 private:
  bool DecodeExtended(const std::vector<uint8_t>&, size_t&, size_t&);

 private:
  size_t num_;
  OptionFormat format_;
  std::vector<uint8_t> raw_;
};

// TODO(tho) : see notes in Pukka
class Options {
 public:
  // Allow repeatable Options.
  typedef std::multimap<OptionNumber, Option> OptionMap;

 public:
  Options() = default;
  ~Options() = default;
  Options (const Options&) = default;
  Options& operator= (const Options&) = default;	

 public:
  bool AddIfMatch(const std::vector<uint8_t>& etag);
  bool AddUriHost(const std::string& uri_host);
  bool AddETag(const std::vector<uint8_t>& etag);
#if TODO_EMPTY_VALUE_SETTER
  bool AddIfNoneMatch();
#endif
  bool AddUriPort(uint64_t uri_port);
  bool AddLocationPath(const std::string& location_path);
  bool AddUriPath(const std::string& uri_path);
  bool AddContentFormat(uint64_t content_format);
  bool AddMaxAge(uint64_t max_age);
  bool AddUriQuery(const std::string& uri_query);
  bool AddAccept(uint64_t content_format);
  bool AddLocationQuery(const std::string& location_query);
  bool AddProxyUri(const std::string& proxy_uri);
  bool AddProxyScheme(const std::string& proxy_scheme);
  bool AddSize1(uint64_t sz);

  bool Encode(std::vector<uint8_t>& buf) const;
  bool Decode(const std::vector<uint8_t>& buf);

 private:
  template <typename Tp>
  bool Add(OptionNumber opt_num, const Tp& val);
  bool DoAdd(const Option& opt);

 public:
  class iterator
    : public std::iterator<std::input_iterator_tag, Option> {

   private:
    bool at_end() const;

   public:
    iterator(OptionMap::iterator begin, OptionMap::iterator end);
    reference operator* ();
    bool operator== (const iterator& other) const;
    bool operator!= (const iterator& other) const;
    iterator& operator++ ();
    iterator operator++ (int);

   private:
    OptionMap::iterator omap_cur_;
    OptionMap::iterator omap_end_;
  };

 public:
  iterator begin();
  iterator end();
  size_t count() const;

 private:
  OptionMap map_;
};

}   // namespace coap

#endif  // COAP_OPTIONS_H_
