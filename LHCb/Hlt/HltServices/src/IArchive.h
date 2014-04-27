#ifndef IARCHIVE_H
#define IARCHIVE_H

#include <string>
#include <vector>
#include <sstream>
#include <sstream>
#include "boost/iostreams/filtering_stream.hpp"
#include "boost/optional.hpp"

namespace {
constexpr struct all_ {
  template <typename T> bool operator()(const T &) const { return true; }
} all{};

struct PrefixFilenameSelector {
  PrefixFilenameSelector(const std::string &_prefix) : prefix(_prefix) {}
  bool operator()(const std::string &fname) const {
    return fname.compare(0, prefix.size(), prefix) == 0;
  }
  std::string prefix;
};
}

class IArchive {
public:
  virtual ~IArchive() = default;

  template <typename SELECTOR>
  std::vector<std::string> files(const SELECTOR &selector = all) const {
    std::vector<std::string> f = contents();
    f.erase(std::remove_if(std::begin(f), std::end(f),
                           [=](const std::string &s) { return !selector(s); }),
            std::end(f));
    return f;
  }

  template <typename T> boost::optional<T> get(const std::string &name) const {
    boost::iostreams::filtering_istream strm;
    if (!setupStream(strm, name))
      return {};
    T t;
    strm >> t;
    return !strm.fail() ? t : boost::optional<T>();
  }

  virtual  bool operator!() const  = 0;
  virtual bool writeable() const = 0;
  virtual bool append(const std::string &, std::stringstream &) = 0;

protected:
  virtual bool setupStream(boost::iostreams::filtering_istream &s,
                           const std::string &name) const = 0;
  virtual std::vector<std::string> contents() const = 0;
};
#endif
