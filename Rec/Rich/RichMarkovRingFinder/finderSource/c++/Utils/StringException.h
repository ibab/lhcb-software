#ifndef LESTERSTRINGEXCEPTIONH
#define LESTERSTRINGEXCEPTIONH

#include <exception>
#include <string>

class StringException : public std::exception {
  const std::string message;
public:
  StringException(const char * s) : message(s) {};
  StringException(const std::string & s) : message(s) {};
  virtual const char * what() const throw () {
    return message.c_str();
  };	
  ~StringException() throw() {};
};

#endif
