#ifndef CGLSTRINGTO
#define CGLSTRINGTO

#include <string>
#include <sstream>
#include <exception>

template <class Type>
Type stringTo(const std::string & s) {
  Type d;
  if (!(std::istringstream (s) >> d)) {
    throw std::exception();
  };
  return d;
};

template <class Type>
std::string stringFrom(const Type & r) {
  std::string s;
  std::ostringstream o;
  o << r;
  return o.str();
};

#endif
