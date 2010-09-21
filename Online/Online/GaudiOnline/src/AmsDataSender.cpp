#include "GaudiOnline/NetworkDataSender.h"
#include "WT/wt_facilities.h"
#include "AMS/amsdef.h"
#include "WT/wtdef.h"
/*
 *   LHCb namespace declaration
 */
namespace LHCb  {
  /** @class AmsDataSender AmsDataSender.cpp GaudiOnline/AmsDataSender.cpp
    *
    *  Implementation of the NetworkDataSender using AMS.
    *
    *  @author Markus Frank
    */
  class AmsDataSender : public NetworkDataSender  {
    /// amsu command handler for receiving event request
    static int i_evt_req_handler(const amsuc_info* /* info */, void* p) {
      AmsDataSender* r = (AmsDataSender*)p;
      char buff[256], source[128];
      size_t size = sizeof(buff);
      unsigned int facility;
      ::amsc_read_message(buff,&size,source,&facility,0);
      r->handleEventRequest(r->m_recipients.size(),source,buff);
      return WT_SUCCESS;
    }
    /// amsu Callback on task dead
    static int i_task_dead(const amsuc_info* i, void* p)  {
      ((AmsDataSender*)p)->taskDead(i->source);
      return WT_SUCCESS;
    }
  public:
    /// Standard algorithm constructor
    AmsDataSender(const std::string& nam, ISvcLocator* pSvc) : NetworkDataSender(nam, pSvc) {}
    /// Standard Destructor
    virtual ~AmsDataSender()   {}
    /// Subscribe to network requests
    virtual StatusCode subscribeNetwork()   {
      MsgStream output(msgSvc(),name());
      int sc = ::amsuc_subscribe(WT_FACILITY_CBMREQEVENT,i_evt_req_handler,i_task_dead,this);
      if ( AMS_SUCCESS != sc )  {
        output << MSG::ERROR << "amsuc_subscribe(WT_FACILITY_CBMREQEVENT) Failed status:" << sc << endmsg;
        return StatusCode::FAILURE;
      }
      if ( !m_target.empty() )  {
        static const char* req = "EVENT_SOURCE";
        sc = ::amsc_send_message(req,strlen(req)+1,m_target.c_str(),WT_FACILITY_CBMCONNECT,0);
        if ( sc != AMS_SUCCESS ) return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
    /// Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork() {
      ::amsuc_remove(WT_FACILITY_CBMREQEVENT);
      ::wtc_flush(WT_FACILITY_CBMREQEVENT);
      return StatusCode::SUCCESS;
    }
    /// Send data to target destination
    virtual StatusCode sendData(const Recipient& tar, const void* data, size_t len)  {
      int sc = ::amsc_send_message(data,len,tar.name.c_str(),WT_FACILITY_CBMEVENT,0);
      return sc==AMS_SUCCESS ? StatusCode::SUCCESS : StatusCode::FAILURE;
    }
  };
}

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,AmsDataSender)
