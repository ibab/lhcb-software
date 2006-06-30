#ifndef __MESSAGE
#define __MESSAGE

#include <string>

class Message {
public:
  int  size;
};

class Address {
public:
  std::string   node_process;
  unsigned int  facility;
  Address( const std::string& n = "", unsigned int f = 0 ) : node_process(n), facility(f) {   }
  Address(const Address& c) : node_process(c.node_process), facility(c.facility) {}
};
#endif
