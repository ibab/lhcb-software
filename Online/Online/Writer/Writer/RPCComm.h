#ifndef RPCCOMM_H
#define RPCCOMM_H

#include <xmlrpc-c/girmem.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>

#define RUNDB_SERVICE_FAIL    0
#define RUNDB_SERVICE_SUCCESS  1

/*
 * The LHCb namespace
 */
namespace LHCb {

  class RPCComm
  {
    private:

      /**
       * An instance of an XML-RPC client. This is reused
       * for all subsequent calls to the run database service.
       */
      xmlrpc_c::clientSimple  m_clientInstance;
      std::string m_serverURL;
      pthread_mutex_t m_rpcLock;

    public:
      RPCComm(std::string serverURL);
      /**
       * "Confirms" that the file is completely written to, closed,
       * and ready for migration to tape.
       */
      void confirmFile(std::string & fileName, unsigned int adlerSum, unsigned const char *md5Sum);

      /**
       * Creates an entry in the Run Database for the specified file,
       * associated with the specified Run Number and Stream.
       */
      void createFile(std::string & fileName, unsigned int runNumber);

  };
}

#endif /*RPCCOMM_H*/
