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
    static void handle_death(netentry_t* /* e */, const netheader_t& hdr, void* param)  
    {  ((SocketDataSender*)param)->taskDead(hdr.name);               }
    static void handle_req(netentry_t* e, const netheader_t& hdr, void* param)  {
      SocketDataSender* p = (SocketDataSender*)param;
      char buff[256];
      int sc = net_receive(p->m_netPlug,e,buff);
      if ( sc == NET_SUCCESS )  {
        p->handleEventRequest(p->m_recipients.size(),hdr.name,hdr.name);
      }
    }
    /// Pointer to netplug device
    NET*     m_netPlug;
  public:
    /// Standard algorithm constructor
    SocketDataSender(const std::string& nam, ISvcLocator* pSvc) 
    : NetworkDataSender(nam, pSvc), m_netPlug(0)  {
    }
    /// Standard Destructor
    virtual ~SocketDataSender()   {}
    /// Subscribe to network requests
    virtual StatusCode subscribeNetwork()   {
      std::string self = RTL::dataInterfaceName()+"::"+RTL::processName();
      m_netPlug = net_init(self);
      net_subscribe(m_netPlug,this,WT_FACILITY_CBMREQEVENT,handle_req,handle_death);
      if ( !m_target.empty() )  {
        static const char* req = "EVENT_SOURCE";
        int sc = net_send(m_netPlug,req,strlen(req)+1,m_target,WT_FACILITY_CBMCONNECT);
        return sc==NET_SUCCESS ? StatusCode::SUCCESS : StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
    /// Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork() {
      net_unsubscribe(m_netPlug,this,WT_FACILITY_CBMREQEVENT);
      net_close(m_netPlug);
      m_netPlug = 0;
      return StatusCode::SUCCESS;
    }
    /// Send data to target destination
    virtual StatusCode sendData(const Recipient& tar, const void* data, size_t len)  {
      //  std::cout << "Send " << len << " bytes of data to " << tar.name << std::endl;
      int sc = net_send(m_netPlug,data,len,tar.name,WT_FACILITY_CBMEVENT);
      return sc==NET_SUCCESS ? StatusCode::SUCCESS : StatusCode::FAILURE;
    }
  };
}
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,SocketDataSender);
