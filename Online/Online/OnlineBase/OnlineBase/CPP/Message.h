#ifndef __MESSAGE
#define __MESSAGE

// C++ include files
#include <string>

/// CPP namespace declaration
namespace CPP  {

  /** @class Message Message.h CPP/Message.h
   */
  class Message {
  public:
    /// Message size
    int  size;
  };

  /** @class Address Message.h CPP/Message.h
   */
  class Address {
  public:
    /// Address source
    std::string   node_process;
    /// Message facility value
    unsigned int  facility;
    /// Initializing constructor
    explicit Address(const std::string& n="", unsigned int f=0) : node_process(n), facility(f) {
    }
    /// Copy constructor
    Address(const Address& c) : node_process(c.node_process), facility(c.facility) {
    }
    /// Assignment operator
    Address& operator=(const Address& c) { 
      if ( &c != this ) {
        node_process=c.node_process;
        facility=c.facility;
      }
      return *this;
    }
  };
}

using CPP::Message;
using CPP::Address;

#endif
