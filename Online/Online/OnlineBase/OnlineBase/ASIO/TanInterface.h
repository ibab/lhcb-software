// $Id: $
//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see OnlineSys/LICENSE.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef ONLINEBASE_ASIO_TAN_TANINTERFACE_H
#define ONLINEBASE_ASIO_TAN_TANINTERFACE_H

// Framework include files
#include "TAN/TanMessage.h"
#include "TAN/TanErrno.h"

#ifdef  __cplusplus

struct hostent;

// ZMQ namespace declaration
namespace BoostAsio {

  // Forward declaration
  class Socket;

  /// Defintion of the TanInterface class
  /** @class TanInterface
   *
   *      C++ Interface to the tan library 
   *      
   *      M.Frank
   */
  class TanInterface {
  public:
    //@Man: public enumerations:
    /// Timeout value
    enum Constants {
      Receive_TMO = 5,
      Connect_TMO = 5,
      Asio_REQ = 111,
      Asio_PUSH= 222,
      Asio_TCP_KEEPALIVE = 333
    };
  protected:
    //@Man: protected member data
    /// Name of the local host
    char         m_pcHostName[64];
    char         m_pcSelf[128];
    /// TCP name service address
    sockaddr_in  m_sintcp;
    /// Allocated port number
    NetworkChannel::Port m_portAllocated;
    int          m_numPorts;
    /// Pointer to outgoing nameserver connection on local node
    Socket*      m_conOut;
    /// Pointer to incoming nameserver connection on local node
    Socket*      m_conIn;

    //@Man: Protected member functions
    /// Protected constructor
    TanInterface();
    /// Destructor
    virtual ~TanInterface();

    /// Convert strings of the type NODE::TASK  and TASK@NODE.DOMAIN into a reasonable task - and node name
    void     nodeWithName  (const char* name, char* node, char* proc);
    /// Set address of remote node for sending and receiving an inquire cycle
    int      setInquireAddr   (const char* node, NetworkChannel::Address& sin);
    /// Set address of local node
    int      setLocalAddress  (NetworkChannel::Address& sin);
    /// Get hostentry by name from inetdb
    hostent* hostByName    (const char* name);
    /// Communicate request with local target name-server
    int      communicate(TanMessage& msg, int tmo);
    /// Communicate request with any target name-server
    int      communicate(Socket* in, Socket* out, TanMessage& msg, int tmo);

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
    int      allocatePort     (const char* name, bool force, NetworkChannel::Port *port);
    /// Allocate port number from local name server (force=false)
    int      allocatePort(const char* name, NetworkChannel::Port *port);
    /// De-Alloctae port entry on local name server
    int      deallocatePort   (const char* name);
  };
}
typedef BoostAsio::TanInterface TanInterface;

#endif     /*   __cplusplus          */

#ifndef __cplusplus
#define __CXX_CONST__
#else      /*   __cplusplus          */
#define __CXX_CONST__  const
extern "C"  { 
#endif     /*   __cplusplus          */
  int asio_tan_allocate_new_port_number(__CXX_CONST__ char *name, __NetworkPort__ *prot);
  int asio_tan_allocate_port_number    (__CXX_CONST__ char *name, __NetworkPort__ *prot);
  int asio_tan_deallocate_port_number  (__CXX_CONST__ char *name);
  int asio_tan_get_address_by_name     (__CXX_CONST__ char *name, __NetworkAddress__ *sad);
  int asio_tan_declare_alias           (__CXX_CONST__ char *name);
  int asio_tan_remove_alias            (__CXX_CONST__ char *name);
  int asio_tan_dump_dbase              (__CXX_CONST__ char *node);
  int asio_tan_shutdown                (__CXX_CONST__ char *node);
  int asio_tan_host_name               (char* node, size_t siz);
#ifdef __cplusplus
}
#endif     /*   __cplusplus              */
#undef __CXX_CONST__

#define tan_allocate_new_port_number asio_tan_allocate_new_port_number
#define tan_allocate_port_number     asio_tan_allocate_port_number
#define tan_deallocate_port_number   asio_tan_deallocate_port_number
#define tan_get_address_by_name      asio_tan_get_address_by_name
#define tan_remove_alias             asio_tan_remove_alias
#define tan_declare_alias            asio_tan_declare_alias
#define tan_dump_dbase               asio_tan_dump_dbase
#define tan_shutdown                 asio_tan_shutdown
#define tan_host_name                asio_tan_host_name 

#endif     /*   ONLINEBASE_ASIO_TAN_TANINTERFACE_H   */
