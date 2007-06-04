#include "GaudiOnline/NetworkDataReceiver.h"
#include "GaudiKernel/SvcFactory.h"
#include "NET/DataTransfer.h"
#include "WT/wt_facilities.h"

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {
    
  using namespace DataTransfer;
  class SocketDataReceiver;

  /** @class SocketDataReceiver SocketDataReceiver.cpp GaudiOnline/SocketDataReceiver.cpp
    *
    *  Implementation of the NetworkDataReceiver using AMS.
    *
    *  @author Markus Frank
    */
  class SocketDataReceiver : public NetworkDataReceiver  {
  protected:
    static void handle_death(netentry_t* e, const netheader_t& hdr, void* param)  
    {  ((SocketDataReceiver*)param)->taskDead(hdr.name);               }
    static void handle_request(netentry_t* e, const netheader_t& hdr, void* param)  {
      SocketDataReceiver* p = (SocketDataReceiver*)param;
      std::string source(hdr.name);
      char buff[256];
      int sc = net_receive(p->m_netPlug,e,buff);
      if ( sc == NET_SUCCESS )  {
        p->handleSourceRequest(p->receivers().size(),source,source);
      }
    }
    static void handle_event(netentry_t* e, const netheader_t& hdr, void* param)  {
      SocketDataReceiver* p = (SocketDataReceiver*)param;
      std::string source(hdr.name);
      char* buff = new char[hdr.size];
      int sc = net_receive(p->m_netPlug,e,buff);
      if ( sc == NET_SUCCESS )  {
        p->handleEventData(source,buff,hdr.size);
      }
    }
    /// Pointer to netplug device
    NET*         m_netPlug;
  public:
    /// Standard algorithm constructor
    SocketDataReceiver(const std::string& nam, ISvcLocator* pSvc) : NetworkDataReceiver(nam, pSvc) {}
    /// Standard Destructor
    virtual ~SocketDataReceiver()   { }
    /// Subscribe to network requests
    virtual StatusCode subscribeNetwork()  {
      std::string self = RTL::dataInterfaceName()+"::"+RTL::processName();
      m_netPlug = net_init(self);
      net_subscribe(m_netPlug,this,WT_FACILITY_CBMEVENT,handle_event,handle_death);
      net_subscribe(m_netPlug,this,WT_FACILITY_CBMCONNECT,handle_request,handle_death);
      return StatusCode::SUCCESS;
    }
    /// Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork()  {
      net_unsubscribe(m_netPlug,this,WT_FACILITY_CBMEVENT);
      net_unsubscribe(m_netPlug,this,WT_FACILITY_CBMCONNECT);
      net_close(m_netPlug);
      m_netPlug = 0;
      return StatusCode::SUCCESS;
    }
    /// Rearm network request for a single source
    virtual StatusCode rearmNetRequest(const RecvEntry& src)  {
      int sc = net_send(m_netPlug,"EVENT_REQUEST",14,src.service,WT_FACILITY_CBMREQEVENT);
      return sc==NET_SUCCESS ? StatusCode::SUCCESS : StatusCode::FAILURE;
    }
    /// Copy event data into buffer manager
    virtual StatusCode copyEventData(void* to, void* from, size_t len)  {
      if ( from != to )  {
        ::memcpy(to,from,len);
        delete [] (char*)from;
      }
      return StatusCode::SUCCESS;
    }
  };
}
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,SocketDataReceiver);
