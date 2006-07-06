#include "DimRpcCommands.h"

#include <iostream>
#include <stdexcept>
typedef const std::string& CSTR;

/// Initializing constructor
LHCb::DimRpcCommandServer::DimRpcCommandServer(DimRpcCommandHandler* h, CSTR mount)
: DimRpc(std::string(mount).c_str(),"C","C"), m_handler(h)
{
  size_t idx = mount.rfind("/");
  if ( idx != std::string::npos )  {
    std::string mnt = mount.substr(0,idx);
    // std::cout << "Dim Server mount is :" << mnt << std::endl;
    DimServer::start(mnt.c_str()); 
  }
}

/// Default destructor
LHCb::DimRpcCommandServer::~DimRpcCommandServer()  {
  if ( m_handler ) m_handler->release();
  m_handler = 0;
}

std::string LHCb::DimRpcCommandServer::handleException(CSTR msg) {
  return msg;
}

std::string LHCb::DimRpcCommandServer::handleCommand(CSTR msg) {
  return m_handler->handle(msg);
}

void LHCb::DimRpcCommandServer::rpcHandler()   {
  std::string result = "";
  try   {
    const char* data = getString();
    result = (data) ? handleCommand(data) : handleCommand("");
    setData((char*)result.c_str());
    return;
  }
  catch(std::exception& e)  {
    result = "Exception occurred while processing request:";
    result += e.what();
    result += ".";
  }
  catch(...)  {
    result = "Unknwon exception occurred while processing request....";
  }
  std::string errm = handleException(result);
  setData((char*)errm.c_str());
}

LHCb::DimRpcCommandClient::DimRpcCommandClient(CSTR nam)
: DimRpcInfo(nam.c_str(), -1)  { 
}

std::string LHCb::DimRpcCommandClient::executeCmd(CSTR cmd)   {
  setData((char*)cmd.c_str()); 
  char* val = getString(); 
  if ( val != 0 && *(int*)val != -1)  {
    //std::cout << "Callback RPC Received : " << val << std::endl; 
    return val;
  }
  else  {
    //std::cout << "Callback RPC Received : (Null-Pointer)" << std::endl; 
  }
  return "";
}
