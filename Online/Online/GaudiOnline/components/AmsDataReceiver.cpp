#include "GaudiOnline/NetworkDataReceiver.h"
#include "WT/wt_facilities.h"
#include "AMS/amsdef.h"
#include "WT/wtdef.h"
#include <cstring>
#include <memory>

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {
    
  /** @class AmsDataReceiver AmsDataReceiver.cpp GaudiOnline/AmsDataReceiver.cpp
    *
    *  Implementation of the NetworkDataReceiver using AMS.
    *
    *  @author Markus Frank
    */
  class AmsDataReceiver : public NetworkDataReceiver  {
  protected:
    /// amsu command handler for receiving event request
    static int i_src_add_handler(const amsuc_info* i, void* p) {
      AmsDataReceiver* r = (AmsDataReceiver*)p;
      char buff[128], source[128];
      size_t size = sizeof(buff);
      unsigned int facility;
      ::amsc_read_message(buff,&size,source,&facility,0);
      r->handleSourceRequest(r->m_receivers.size(),i->source,i->source);
      return WT_SUCCESS;
    }
    /// amsu Callback on receiving ams message
    static int i_receive_evt(const amsuc_info* i, void* p) {
      AmsDataReceiver* r = (AmsDataReceiver*)p;
      size_t len = i->length;
      void* buff = 0; // Only when using amsc_read_message --> ::operator new(i->length);
      char source[128];
      unsigned int facility;
      int sc = ::amsc_read_message_long(&buff,&len,source,&facility,0);
      // int sc = ::amsc_read_message(buff,&len,source,&facility,0);
      if ( AMS_SUCCESS != sc )   {
        MsgStream err(r->msgSvc(), r->name());
        err << MSG::ERROR << "Failed to read message from " << i->source << " status:" << sc << endmsg;
        return WT_ERROR;
      }
      if ( !r->handleEventData(source, buff, len).isSuccess() )  {
        MsgStream err(r->msgSvc(), r->name());
        err << MSG::ERROR << "Failed to handle message from " << i->source << endmsg;
        if ( buff ) ::amsc_release_message_long(buff);
        return WT_ERROR;
      }
      return WT_SUCCESS;
    }
    /// amsu Callback on task dead
    static int i_task_dead(const amsuc_info* i, void* p)  {
      ((AmsDataReceiver*)p)->taskDead(i->source);
      return WT_SUCCESS;
    }
  public:
    /// Standard algorithm constructor
    AmsDataReceiver(const std::string& nam, ISvcLocator* pSvc) : NetworkDataReceiver(nam, pSvc) {}
    /// Standard Destructor
    virtual ~AmsDataReceiver()   { }
    /// Subscribe to network requests
    virtual StatusCode subscribeNetwork()  {
      MsgStream err(msgSvc(), name());
      int sc = ::amsuc_subscribe(WT_FACILITY_CBMEVENT,i_receive_evt,i_task_dead,this);
      if ( AMS_SUCCESS != sc )  {
        err << MSG::ERROR << "amsuc_subscribe(WT_FACILITY_CBMEVENT) Failed status:" << sc << endmsg;
        return StatusCode::FAILURE;
      }
      sc = ::amsuc_subscribe(WT_FACILITY_CBMCONNECT,i_src_add_handler,0,this);
      if ( AMS_SUCCESS != sc )  {
        err << MSG::ERROR << "amsuc_subscribe(WT_FACILITY_CBMCONNECT) Failed status:" << sc << endmsg;
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
    /// Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork()  {
      ::amsuc_remove(WT_FACILITY_CBMCONNECT);
      ::amsuc_remove(WT_FACILITY_CBMEVENT);
      ::wtc_flush(WT_FACILITY_CBMCONNECT);
      ::wtc_flush(WT_FACILITY_CBMEVENT);
      return StatusCode::SUCCESS;
    }
    /// Rearm network request for a single source
    virtual StatusCode rearmNetRequest(const RecvEntry& src)  {
      int sc = ::amsc_send_message("EVENT_REQUEST",14,src.service.c_str(),WT_FACILITY_CBMREQEVENT,0);
      return sc==AMS_SUCCESS ? StatusCode::SUCCESS : StatusCode::FAILURE;
    }
    /// Copy event data into buffer manager
    virtual StatusCode copyEventData(void* to, void* from, size_t len)  {
      ::memcpy(to,from,len);
      ::amsc_release_message_long(from);
      return StatusCode::SUCCESS;
    }
  };
}

#include "GaudiKernel/SvcFactory.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,AmsDataReceiver)
