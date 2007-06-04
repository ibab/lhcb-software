#include "GaudiOnline/NetworkDataSender.h"
#include "NET/DataTransfer.h"
#include "WT/wt_facilities.h"

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {

  using namespace DataTransfer;
  class SocketDataSender;

  /** @class SocketDataSender SocketDataSender.cpp GaudiOnline/SocketDataSender.cpp
    *
    *  Implementation of the NetworkDataSender using TCP/IP sockets.
    *
    *  @author Markus Frank
    */
  class SocketDataSender : public NetworkDataSender  {
    struct NetPlug : public Worker  {
      SocketDataSender* m_parent;
      NetPlug(const std::string& nam, SocketDataSender* par) : Worker(nam), m_parent(par) {}
      virtual ~NetPlug() {}
      virtual void receive(netentry_t* e, const netheader_t& hdr)  {
        int typ = hdr.msg_type;
        if ( typ == NET_TASKDIED || typ == NET_CONNCLOSED )  {
          m_parent->taskDead(hdr.name);
          return;
        }
        char* buff = new char[hdr.size];
        get(e, buff);
        m_parent->handleEventRequest(m_parent->m_recipients.size(),hdr.name,buff);
        delete [] buff;
      }
    };
    /// Pointer to netplug device
    NetPlug*     m_netPlug;
  public:
    /// Standard algorithm constructor
    SocketDataSender(const std::string& nam, ISvcLocator* pSvc) 
    : NetworkDataSender(nam, pSvc), m_netPlug(0)  {}
    /// Standard Destructor
    virtual ~SocketDataSender()   {}
    /// Subscribe to network requests
    virtual StatusCode subscribeNetwork()   {
      std::string self = RTL::dataInterfaceName()+"::"+RTL::processName()+"_"+name();
      m_netPlug = new NetPlug(self,this);
      if ( !m_target.empty() )  {
        static const char* req = "EVENT_SOURCE";
        NetErrorCode sc = m_netPlug->send(req,strlen(req)+1,m_target,WT_FACILITY_CBMCONNECT);
        return sc==NET_SUCCESS ? StatusCode::SUCCESS : StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
    /// Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork() {
      if ( m_netPlug ) delete m_netPlug;
      m_netPlug = 0;
      return StatusCode::SUCCESS;
    }
    /// Send data to target destination
    virtual StatusCode sendData(const Recipient& tar, const void* data, size_t len)  {
      NetErrorCode sc = m_netPlug->send(data,len,tar.name,WT_FACILITY_CBMEVENT);
      return sc==NET_SUCCESS ? StatusCode::SUCCESS : StatusCode::FAILURE;
    }
  };
}
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,SocketDataSender);
