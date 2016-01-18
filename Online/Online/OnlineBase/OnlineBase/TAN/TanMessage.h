/*
  Definition of the TANMESSAGE structure
  C interface
  C++ usage

*/
#ifndef ONLINEKERNEL_TAN_TANMESSAGE_H
#define ONLINEKERNEL_TAN_TANMESSAGE_H
#include "NET/NetworkChannel.h"
#include "TAN/TanErrno.h"

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
    unsigned int       m_length;
    unsigned int       m_error;
    unsigned int       m_function;
    char               m_name[128];
    __NetworkAddress__ m_sin;
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
    DISCONNECTED,
    DUMP,
    SHUTDOWN
  };
public:
  //@Man: Public interface: 
  /// Constructor to build request message
  explicit TanMessage ( u_int func, const char* proc = "" );
  /// Standard constructor
  TanMessage ();
  /// Standard destructor
  ~TanMessage ()  {
  }
  /// equal operator
  TanMessage& operator = (const TanMessage& cp);
  /// Retrieve pointer to name
  char* _Name() {
    return m_name;
  }
  /// Retrieve pointer to name
  const char* _Name()  const {
    return m_name;
  }
  /// Retrieve reference to network address
  Address& Addr()  {
    return m_sin;
  }
  /// Retrieve reference to network sub address
  SubAddress& address() {
    return m_sin.sin_addr;
  }
  /// Retrieve reference to network sub address
  const SubAddress& address()  const  {
    return m_sin.sin_addr;
  }
  /// Retrieve port number
  Port    port() const {
    return m_sin.sin_port;
  }
  /// Retrieve network family type
  Family  family() const {
    return m_sin.sin_family;
  }
  /// Retrieve error code
  unsigned int error() const {
    return m_error;
  }
  /// Retrieve function code (of type Type)
  unsigned int function() const {
    return m_function;
  }
  /// Retrive length
  unsigned int _Length() const {
    return m_length;
  }
  /// Swap to network structure
  void Convert();
};

inline TanMessage::TanMessage () {
  m_length = sizeof (TanMessage);
  m_error  = TAN_SS_SUCCESS;
  m_function = 0;
  ::memset(m_name,0,sizeof(m_name));
  ::memset(&m_sin, 0, sizeof (m_sin));
}

inline TanMessage::TanMessage (u_int func, const char* proc)  {
  size_t i, n;
  m_length    = sizeof (TanMessage);
  m_error     = TAN_SS_SUCCESS;
  m_function  = func;
  for (i=0, n=::strlen(proc); i<n; i++)
    m_name[i] = char(::tolower(proc[i]));
  m_name[i] = 0;
  ::memset  (&m_sin, 0, sizeof (m_sin));
}

inline TanMessage& TanMessage::operator = (const TanMessage& cp)  {
  if ( &cp != this )  {
    m_sin       = cp.m_sin;
    m_error     = cp.m_error;
    m_length    = sizeof(cp);
    m_function  = cp.m_function;
    ::strncpy(m_name, cp.m_name, sizeof(m_name));
  }
  return *this;
}

inline void TanMessage::Convert()  {
  //m_length   = htonl(m_length);
  //m_error    = htonl(m_error);
  //m_function = htonl(m_function);
  //m_sin.sin_family = ntohs (m_sin.sin_family);
}

#endif   /* __cplusplus                    */
#endif   /* ONLINEKERNEL_TAN_TANMESSAGE_H  */
