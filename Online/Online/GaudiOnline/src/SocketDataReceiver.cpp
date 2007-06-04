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
    struct NetPlug : public Worker  {
      NetworkDataReceiver* m_parent;
      NetPlug(const std::string& nam, NetworkDataReceiver* par) : Worker(nam), m_parent(par) {}
      virtual ~NetPlug() {}
      virtual void receive(netentry_t* e, const netheader_t& hdr)  {
        char *tmp=0, *buff=0, buffer[128];
        std::string source(hdr.name);
        NetErrorCode sc;
        switch(hdr.msg_type) {
          case NET_TASKDIED:
          case NET_CONNCLOSED:
            m_parent->taskDead(source);
            return;
          case NET_MSG_DATA:
            tmp = hdr.size<sizeof(buffer) ? buffer : buff=new char[hdr.size];
            sc = get(e, tmp);
            if ( sc == NET_SUCCESS )  {
              switch(hdr.facility)  {
                case WT_FACILITY_CBMCONNECT:
                  m_parent->handleSourceRequest(m_parent->receivers().size(),source,source);
                  break;
                case WT_FACILITY_CBMEVENT:
                  m_parent->handleEventData(source,tmp,hdr.size);
                  return; // Do not delete buffer !
                case WT_FACILITY_CBMREQEVENT:
                  break;
                default:
                  break;
              }
            }
            if ( buff ) delete [] buff;
        }
      }
    };
    NetPlug*     m_netPlug;
  public:
    /// Standard algorithm constructor
    SocketDataReceiver(const std::string& nam, ISvcLocator* pSvc) : NetworkDataReceiver(nam, pSvc) {}
    /// Standard Destructor
    virtual ~SocketDataReceiver()   { }
    /// Subscribe to network requests
    virtual StatusCode subscribeNetwork()  {
      std::string self = RTL::dataInterfaceName()+"::"+RTL::processName();
      m_netPlug = new NetPlug(self,this);
      return StatusCode::SUCCESS;
    }
    /// Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork()  {
      if ( m_netPlug ) delete m_netPlug;
      m_netPlug = 0;
      return StatusCode::SUCCESS;
    }
    /// Rearm network request for a single source
    virtual StatusCode rearmNetRequest(const RecvEntry& src)  {
      NetErrorCode sc = m_netPlug->send("EVENT_REQUEST",14,src.service,WT_FACILITY_CBMREQEVENT);
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
