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
    static int s_receiveData(const amsuc_info* info, void* param)   {
      return ((AMSDataHandler*)param)->i_receiveData(info);
    }
    /// Local callback when receiving data;
    int i_receiveData(const amsuc_info* info)  {
      if ( info->status != AMS_SUCCESS )  {
        error() << "Failed to spy on message. status:" << info->status << endmsg;
        return AMS_SUCCESS;
      }
      StatusCode sc = (this->*m_net_handler)(info->length);
      return sc.isSuccess() ? AMS_SUCCESS : AMS_ERROR;
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
      int sc = ::amsuc_subscribe(WT_FACILITY_DAQ_EVENT,s_receiveData,s_taskDead,this);
      if ( AMS_SUCCESS != sc )  {
        error() << "amsuc_subscribe failed status:" << sc << endmsg;
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
    /// Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork()  {
      ::amsuc_remove(WT_FACILITY_DAQ_EVENT);
      return StatusCode::SUCCESS;
    }
    /// Send data to target destination
    virtual StatusCode sendData(const std::string& tar, const void* data, size_t len)  {
      int sc = amsc_send_message(data,len,tar.c_str(),WT_FACILITY_DAQ_EVENT,0);
      if ( AMS_SUCCESS != sc )   {
        error() << "Failed to send MDF to " << tar << ". status:" << sc << endmsg;
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
    /// Read data from network
    virtual StatusCode readData(void* data, size_t* len, char* source)  {
      char         buff[128];
      unsigned int facility;
      char* src = source ? source : buff;
      int sc = ::amsc_read_message(data,len,src,&facility,0);
      if ( AMS_SUCCESS != sc )   {
        error() << "Failed to read message. status:" << sc << endmsg;
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }
  };
}
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,AMSDataHandler);
