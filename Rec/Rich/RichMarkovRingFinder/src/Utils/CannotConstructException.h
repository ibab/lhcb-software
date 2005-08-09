#ifndef LESTERCANNOTCONSTRUCT
#define LESTERCANNOTCONSTRUCT

#include "Utils/StringException.h"

struct CannotConstructException : public StringException {
  CannotConstructException(const char * s) : StringException(s) {};
  CannotConstructException(std::string & s) : StringException(s) {};
};

#endif
