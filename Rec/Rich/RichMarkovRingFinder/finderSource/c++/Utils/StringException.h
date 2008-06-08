#ifndef LESTERSTRINGEXCEPTIONH
#define LESTERSTRINGEXCEPTIONH

#include <exception>
#include <string>

class StringException : public std::exception
{
private:
  const std::string m_message;
public:
  StringException(const std::string & s) : m_message(s) {};
  virtual const char * what() const throw () 
  {
    return m_message.c_str();
  }	
  ~StringException() throw() {}
};

#endif
