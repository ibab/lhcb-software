#ifndef PYDIMRPCCOMMANDS_H
#define PYDIMRPCCOMMANDS_H 1

#include "DimRpcCommands.h"
#include <boost/python.hpp>
#include "Python.h"

/*
 *    LHCb namespace
 */
namespace LHCb  {

  /**@class PyDimRpcCommandServer PyDimRpcCommands.h PyDimRpcCommands.h
    *
    * @author  M.Frank
    * @version 1.0
    */
  class PyDimRpcCommandServer : public DimRpcCommandServer     {
  protected:
  public:
    /// Initializing constructor
     PyDimRpcCommandServer(boost::python::object obj, const std::string& call, const std::string& mount, const std::string& nam);
    /// Default destructor
    virtual ~PyDimRpcCommandServer();
    /// Overloaded execption handler
    virtual std::string handleException(const std::string& msg);
  };

  /**@class PyDimRpcCommandClient PyDimRpcCommands.h PyDimRpcCommands.h
    *
    * @author  M.Frank
    * @version 1.0
    */
  class PyDimRpcCommandClient : public DimRpcCommandClient   {
  protected:
    boost::python::object m_callback;
  public:
    /// Initializing constructor
    PyDimRpcCommandClient(boost::python::object cb, const std::string& nam);
    /// Default destructor
    virtual ~PyDimRpcCommandClient();
    /// Command execution call
    virtual std::string executeCmd(const std::string& cmd);
  }; 
}      // end namespace LHCb
#endif // PYDIMRPCCOMMANDS_H
