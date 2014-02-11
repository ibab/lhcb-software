#include "GaudiOnline/NetworkDataReceiver.h"
#include "GaudiKernel/SvcFactory.h"
#include "NET/DataTransfer.h"
#include "WT/wt_facilities.h"
#include "RTL/Lock.h"
#include <cstring>

int gauditask_task_lock();
int gauditask_task_trylock();
int gauditask_task_unlock();

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
    bool m_finish;
    /// Pointer to netplug device
    NET*         m_netPlug;

    static void handle_death(netentry_t* /* e */, const netheader_t& hdr, void* param)  {
      int sc = gauditask_task_trylock();
      if ( sc == 1 ) {
        SocketDataReceiver* p = (SocketDataReceiver*)param;
	p->taskDead(hdr.name);
        gauditask_task_unlock();
      }
    }
    static void handle_request(netentry_t* e, const netheader_t& hdr, void* param)  {
      int sc = gauditask_task_trylock();
      if ( sc == 1 ) {
        SocketDataReceiver* p = (SocketDataReceiver*)param;
        std::string source(hdr.name);
        char buff[256];
        sc = net_receive(p->m_netPlug,e,buff);
        if ( sc == NET_SUCCESS )  {
          p->handleSourceRequest(p->receivers().size(),source,source);
        }
        gauditask_task_unlock();
        return;
      }
      // Things go awfully wrong....e.g. finalize was called during data taking.
      ::fprintf(stdout,"Loosing event request....was finalize called ?\n");
      ::fflush(stdout);
    }
    static void handle_event(netentry_t* e, const netheader_t& hdr, void* param)  {
      int sc = gauditask_task_trylock();
      if ( sc == 1 ) {
        SocketDataReceiver* p = (SocketDataReceiver*)param;
        std::string source(hdr.name);
        char* buff = new char[hdr.size];
        sc = net_receive(p->m_netPlug,e,buff);
        if ( sc == NET_SUCCESS )  {
          RecvEntry* ent = p->receiver(source);
          if ( !ent ) {
            // In case the sender did not send the source request: 
            // add data source on the fly....
            p->handleSourceRequest(p->receivers().size(),source,source);
            ent = p->receiver(source);
          }
          if ( ent ) {
            if ( p->handleEventData(*ent,buff,hdr.size).isSuccess() ) {
              gauditask_task_unlock();
              return;
            }
          }
          ::fprintf(stdout,"Loosing EVENT....Cannot register data source:%s.\n",hdr.name);
          ::fflush(stdout);
        }
        delete [] buff;
        gauditask_task_unlock();
        return;
      }
      // Things go awfully wrong....e.g. finalize was called during data taking.
      ::fprintf(stdout,"Loosing event....was finailize called ?\n");
      ::fflush(stdout);
    }

  public:
    /// Standard algorithm constructor
    SocketDataReceiver(const std::string& nam, ISvcLocator* pSvc) 
      : NetworkDataReceiver(nam, pSvc), m_finish(false), m_netPlug(0) {
    }
    /// Standard Destructor
    virtual ~SocketDataReceiver()   { }
    /// Subscribe to network requests
    virtual StatusCode subscribeNetwork()  {
      m_finish = false;
      std::string self = RTL::dataInterfaceName()+"::"+RTL::processName();
      m_netPlug = net_init(self);
      net_subscribe(m_netPlug,this,WT_FACILITY_CBMEVENT,handle_event,handle_death);
      net_subscribe(m_netPlug,this,WT_FACILITY_CBMCONNECT,handle_request,handle_death);
      return StatusCode::SUCCESS;
    }
    /// Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork()  {
      m_finish = true;
      //RTL::Lock lock(m_lock);
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
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,SocketDataReceiver)
