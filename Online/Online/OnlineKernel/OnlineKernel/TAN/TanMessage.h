/*
              Definition of the TANMESSAGE structure
              C interface
              C++ usage

*/
#ifndef __TANMessage_H__
#define __TANMessage_H__
#include "NET/NetworkChannel.h"

#define NAME_SERVICE_NAME    "TAN_NAME"
#define NAME_SERVICE_PORT    6789

#ifdef __cplusplus
#include <cstring>
#include <ctype.h>

/// Definition of a TAN communication message
/**
   {\Large{\bf Class TanMessage}}

    Message structure to communicate with the nameserver task
*/
class TanMessage  {
public:
  //@Man: Public local structure declarations
  /// Definition of the Port structure
  typedef __NetworkPort__       Port;
  /// Definition of the Family structure
  typedef __NetworkFamily__     Family;
  /// Definition of the Channle structure
  typedef __NetworkChannel__    Channel;
  /// Definition of the Address structure 
  typedef __NetworkAddress__    Address;
  /// Definition of the Subaddress structure
  typedef __NetworkSubAddress__ SubAddress;
public:
#else
typedef struct  {
#endif
  unsigned int       length;
  unsigned int       error;
  unsigned int       function;
  char               name [32];
  __NetworkAddress__ sin;
#ifndef __cplusplus
}  TAN_MSG, TanMessage;
#else                                            /*   C++ Only!!!   */
public:
  /// Enumeration: Request codes
  enum Type {
    ALLOCATE,
    DEALLOCATE,
    INQUIRE,
    ALIAS,
    DEALIAS,
    DUMP
  };
public:
  //@Man: Public interface: 
  /// Constructor to build request message
  TanMessage ( u_int func, const char* proc = "" );
  /// Standard constructor
  TanMessage ();
  /// Standard destructor
  ~TanMessage ()  {
  }
  /// equal operator
  TanMessage& operator = (const TanMessage& cp);
  /// Retrieve pointer to name
  char* _Name() {
    return name;
  }
  /// Retrieve reference to network address
  Address& Addr()  {
    return sin;
  }
  /// Retrieve reference to network sub address
  SubAddress& _Address() {
    return sin.sin_addr;
  }
  /// Retrieve port number
  Port    _Port() const {
    return sin.sin_port;
  }
  /// Retrieve network family type
  Family  _Family() const {
    return sin.sin_family;
  }
  /// Retrieve error code
  unsigned int _Error() const {
    return error;
  }
  /// Retrieve function code (of type Type)
  unsigned int _Function() const {
    return function;
  }
  /// Retrive length
  unsigned int _Length() const {
    return length;
  }
  /// Swap to network structure
  void Convert();
};

inline TanMessage::TanMessage () {
  length = sizeof (TanMessage);
  error = function = 0;
  memset  (name,0,sizeof(name));
  memset  (&sin, 0, sizeof (sin));
}

inline TanMessage::TanMessage (u_int func, const char* proc)  {
  length    = htonl (sizeof (TanMessage));
  error     = htonl (0);
  function  = htonl (func);
  for ( size_t i = 0; i < strlen(proc); i++ )
     name[i] = tolower(proc[i]);
  name[i] = 0;
  memset  (&sin, 0, sizeof (sin));
}

inline TanMessage& TanMessage::operator = (const TanMessage& cp)  {
  sin       = cp.sin;
  error     = cp._Error();
  length    = sizeof(cp);
  function  = cp._Function();
  for ( size_t i = 0; i < strlen(cp.name); i++ )
     name[i] = tolower(cp.name[i]);
  name[i] = 0;
  return *this;
}

inline void TanMessage::Convert()  {
  length   = htonl(length);
  error    = htonl(error);
  function = htonl(function);
  //sin.sin_family = ntohs (sin.sin_family);
}

#endif   /* __cplusplus       */
#endif   /* __TANMessage_H__  */
