#ifndef TAN_TANINTERFACE_H
#define TAN_TANINTERFACE_H
#include <CPP/SmartObject.h>
#include "TAN/TanMessage.h"
#include "TAN/TanErrno.h"

#ifdef  __cplusplus

/// Defintion of the TanInterface class
/** @class TanInterface

    C++ Interface to the tan library 

    M.Frank
*/
struct hostent;
class TcpNetworkChannel;
class TanInterface : public SmartObject<TanInterface> {
public:
  //@Man: public enumerations:
  /// Timeout value
  enum Constants {
    Receive_TMO = 5,
    Connect_TMO = 5
  };
protected:
  //@Man: protected member data
  /// Name of the local host
  char        m_pcHostName[32];
  /// UDP name service address
  sockaddr_in m_sinudp;
  /// TCP name service address
  sockaddr_in m_sintcp;
  /// Allocated port number
  NetworkChannel::Port m_portAllocated;
  /// Pointer to nameserver connection on local node
  TcpNetworkChannel* m_channel;

  //@Man: Protected member functions
  /// Protected constructor
  TanInterface();
  /// Destructor
  virtual ~TanInterface();

  /// Convert strings of the type NODE::TASK  and TASK@NODE.DOMAIN into a reasonable task - and node name
  void     nodeWithName  (const char* name, char* node, char* proc);
  /// Set address of remote node for sending and receiving an inquire cycle
  int      setInquireAddr   (const char* node, NetworkChannel::Address& sin, NetworkChannel::Address& rin);
  /// Set address of local node
  int      setLocalAddress  (NetworkChannel::Address& sin);
  /// Get hostentry by name from inetdb
  hostent* hostByName    (const char* name);

  /// Fatal nameserver connection error: close channel and return given error code
  int fatalError(int code);
  /// Convert TAN error codes to operating system specific ones
  int errorCode(int tan_error);
  /// Send shutdown request to nameserver
  int sendAction(int which, const char* node);
public:
  //@Man: Public member functions
  /// Instanciator
  static TanInterface& instance();
  /// Dump remote database on server node
  int      shutdown         (const char* node);
  /// Dump remote database on server node
  int      dumpDB           (const char* node);
  /// Get local host name
  int      hostName         (char* node, size_t size) const;
  /// Get address of remote task givenb by name
  int      addressByName    (const char* name, NetworkChannel::Address& sin);
  /// Declare alias name to local nameserver
  int      declareAlias     (const char* name);
  /// Remove alias name from local nameserver
  int      removeAlias      (const char* name);
  /// Alloctae port entry on local name server
  int      allocatePort     (const char* name, NetworkChannel::Port *port);
  /// De-Alloctae port entry on local name server
  int      deallocatePort   (const char* name);
};

#endif     /*   __cplusplus          */

#ifndef __cplusplus
#define __CXX_CONST__
#else      /*   __cplusplus          */
#define __CXX_CONST__  const
extern "C"  { 
#endif     /*   __cplusplus          */
  int tan_allocate_port_number   (__CXX_CONST__ char *name, __NetworkPort__ *prot);
  int tan_deallocate_port_number (__CXX_CONST__ char *name);
  int tan_get_address_by_name    (__CXX_CONST__ char *name, __NetworkAddress__ *sad);
  int tan_declare_alias          (__CXX_CONST__ char *name);
  int tan_remove_alias           (__CXX_CONST__ char *name);
  int tan_dump_dbase             (__CXX_CONST__ char *node);
  int tan_shutdown               (__CXX_CONST__ char *node);
  int tan_host_name              (char* node, size_t siz);
#ifdef __cplusplus
}
#endif     /*   __cplusplus          */
#undef __CXX_CONST__
#endif     /*   TAN_TANINTERFACE_H   */
