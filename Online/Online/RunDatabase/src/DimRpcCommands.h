#ifndef DIMRPCCOMMANDS_H
#define DIMRPCCOMMANDS_H 1
#include "dis.hxx" 
#include "dic.hxx"
#include <string>

/*
 *    LHCb namespace
 */
namespace LHCb {
  /**@class DimRpcCommandHandler DimRpcCommands.h DimRpcCommands.h
    *
    * @author  M.Frank
    * @version 1.0
    */
  class DimRpcCommandHandler  {
  public:
    virtual void release() = 0;
    virtual std::string handle(const std::string& msg) = 0;
  };

  /**@class DimRpcCommandServer DimRpcCommands.h DimRpcCommands.h
    *
    * @author  M.Frank
    * @version 1.0
    */
  class DimRpcCommandServer : public DimRpc     {
  protected:
    DimRpcCommandHandler* m_handler;
  public:
    /// Initializing constructor
    DimRpcCommandServer(DimRpcCommandHandler* h, const std::string& mount);
    /// Default destructor
    virtual ~DimRpcCommandServer();
    virtual std::string handleCommand(const std::string& msg);
    virtual std::string handleException(const std::string& msg);
    virtual void rpcHandler();
  };

  /**@class DimRpcCommandClient DimRpcCommands.h DimRpcCommands.h
    *
    * @author  M.Frank
    * @version 1.0
    */
  class DimRpcCommandClient : public DimRpcInfo   {
  public:
    /// Initializing constructor
    DimRpcCommandClient(const std::string& nam);
    virtual std::string executeCmd(const std::string& cmd);
  };
}      // end namespace LHCb
#endif // DIMRPCCOMMANDS_H
