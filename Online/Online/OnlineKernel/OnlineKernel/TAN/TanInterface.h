#ifndef TAN_TANINTERFACE_H
#define TAN_TANINTERFACE_H
#include <CPP/SmartObject.h>
#include "TAN/TanMessage.h"
#include "TAN/tanerrno.h"
#ifdef  __cplusplus

/// Defintion of the TanInterface class
/**

   {\Large{\bf Class TanInterface}}

   C++ Interface to the tan library         \\
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
    char        _pcHostName[32];
    /// Pointer to hostentry of the local host
    hostent*    _pLocalHost;
    /// UDP name service address
    sockaddr_in _sinudp;
    /// TCP name service address
    sockaddr_in _sintcp;
    /// Allocated port number
    NetworkChannel::Port _portAllocated;
    /// Pointer to nameserver connection on local node
    TcpNetworkChannel* m_channel;
    /// Pointer to Instance object
    static TanInterface* __instance;
    //@Man: Protected member functions
    /// Protected constructor
    TanInterface();
    /// Convert strings of the type NODE::TASK  and TASK@NODE.DOMAIN into a reasonable task - and node name
    void     GetNodeWithName  (const char* name, char* node, char* proc);
    /// Set address of remote node for sending and receiving an inquire cycle
    int      SetInquireAddr   (const char* node, NetworkChannel::Address& sin, NetworkChannel::Address& rin);
    /// Set address of local node
    int      SetLocalAddress  (NetworkChannel::Address& sin);
    /// Get hostentry by name from inetdb
    hostent* GetHostByName    (const char* name);
    /// Fatal nameserver connection error: close channel and return given error code
    int FatalError(int code);
    /// Convert TAN error codes to operating system specific ones
    int ErrorCode(int tan_error);
public:
    //@Man: Public member functions
    /// Instanciator
    static TanInterface& Instance();
    /// Destructor
    virtual ~TanInterface();
    /// Dump remote database on server node
    int DumpDB                (const char* node);
    /// Get address of remote task givenb by name
    int      GetAddressByName (const char* name, NetworkChannel::Address& sin);
    /// Declare alias name to local nameserver
    int      DeclareAlias     (const char* name);
    /// Remove alias name from local nameserver
    int      RemoveAlias      (const char* name);
    /// Alloctae port entry on local name server
    int      AllocatePort     (const char* name, NetworkChannel::Port *port);
    /// De-Alloctae port entry on local name server
    int      DeallocatePort   (const char* name);
};

#ifdef _TanInterface_C_
TanInterface* TanInterface::__instance = 0;
#endif

#endif     /*   __cplusplus          */

#ifndef __cplusplus
#define __CXX_CONST__
#else      /*   __cplusplus          */
#define __CXX_CONST__  const
extern "C"  { 
#endif     /*   __cplusplus          */
    int tan_allocate_port_number    (__CXX_CONST__ char *name, __NetworkPort__ *prot);
    int tan_deallocate_port_number  (__CXX_CONST__ char *name);
    int tan_get_address_by_name       (__CXX_CONST__ char *name, __NetworkAddress__ *sad);
    int tan_declare_alias       (__CXX_CONST__ char *name);
    int tan_remove_alias       (__CXX_CONST__ char *name);
    int tan_dump_dbase                (__CXX_CONST__ char *node);
#ifdef __cplusplus
}
#endif     /*   __cplusplus          */
#undef __CXX_CONST__
#endif     /*   TAN_TANINTERFACE_H   */
