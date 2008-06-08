#ifndef LESTERCANNOTCONSTRUCT
#define LESTERCANNOTCONSTRUCT

#include "Utils/StringException.h"

class CannotConstructException : public StringException 
{
public:
  CannotConstructException(const std::string & s) : StringException(s) {}
};

#endif
