#include "GaudiOnline/NetworkDataHandler.h"
#include "GaudiKernel/AlgFactory.h"
#include "AMS/amsdef.h"
#include "WT/wtdef.h"

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {
    
  /** @class AMSDataHandler AMSDataHandler.cpp GaudiOnline/AMSDataHandler.cpp
    *
    *  Implementation of the NetworkDataHandler using AMS.
    *
    *  @author Markus Frank
    */
  class AMSDataHandler : public NetworkDataHandler  {
  public:
    /// amsu Callback on receiving ams message
    static int s_receive_event(const amsuc_info* info, void* param)   {
      AMSDataHandler* h = (AMSDataHandler*)param;
      if ( h->check(info).isSuccess() )  {
        if ( !h->handleEventData(info->source,info->length).isSuccess() )  {
        }
      }
      return AMS_SUCCESS;
    }
    static int s_receive_request(const amsuc_info* info, void* param)   {
      AMSDataHandler* h = (AMSDataHandler*)param;
      if ( h->check(info).isSuccess() )  {
        if ( !h->handleEventRequest(0,info->source,info->length).isSuccess() ) {
        }
      }
      return AMS_SUCCESS;
    }
    /// Local callback when receiving data;
    StatusCode check(const amsuc_info* info)  {
      if ( info->status != AMS_SUCCESS )  {
        MsgStream log(msgSvc(),name());
        log << MSG::ERROR << "Failed to spy on message. status:" << info->status << endmsg;
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
    /// amsu Callback on task dead
    static int s_taskDead(const amsuc_info* i, void* p)  {
      return ((AMSDataHandler*)p)->taskDead(i->source).isSuccess() ? AMS_SUCCESS : AMS_ERROR;
    }
    /// Standard algorithm constructor
    AMSDataHandler(const std::string& nam, ISvcLocator* pSvc) : NetworkDataHandler(nam, pSvc) {}
    /// Standard Destructor
    virtual ~AMSDataHandler()   {}
    /// Subscribe to network requests
    virtual StatusCode subscribeNetwork()  {
      int sc = AMS_SUCCESS;
      if ( isNetEventProducer() ) 
        sc = ::amsuc_subscribe(WT_FACILITY_DAQ_EVENT,s_receive_request,s_taskDead,this);
      else
        sc = ::amsuc_subscribe(WT_FACILITY_DAQ_EVENT,s_receive_event,s_taskDead,this);
      if ( AMS_SUCCESS != sc )  {
        MsgStream log(msgSvc(),name());
        log << MSG::ERROR << "amsuc_subscribe failed status:" << sc << endmsg;
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
    /// Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork()  {
      ::amsuc_remove(WT_FACILITY_DAQ_EVENT);
      return StatusCode::SUCCESS;
    }
    /// Networking layer overload: Read request for event data from network
    virtual StatusCode readEventRequest(void* data, size_t* len)
    {  return readData(data,len);                                          }
    /// Send data to target destination
    virtual StatusCode sendData(const Recipient& tar, const void* data, size_t len)  {
      int sc = amsc_send_message(data,len,tar.name.c_str(),WT_FACILITY_DAQ_EVENT,0);
      if ( AMS_SUCCESS != sc )   {
        MsgStream log(msgSvc(),name());
        log << MSG::ERROR << "Failed to send data to " << tar.name << ". status:" << sc << endmsg;
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
    /// Read data from network
    virtual StatusCode readData(void* data, size_t* len)  {
      char         src[128];
      unsigned int facility;
      int sc = ::amsc_read_message(data,len,src,&facility,0);
      if ( AMS_SUCCESS != sc )   {
        MsgStream log(msgSvc(),name());
        log << MSG::ERROR << "Failed to read message. status:" << sc << endmsg;
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
  };
}
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,AMSDataHandler)
