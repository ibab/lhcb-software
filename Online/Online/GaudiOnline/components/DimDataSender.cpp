#include "GaudiOnline/NetworkDataSender.h"
#include "dic.hxx"
#include "dis.hxx"
/*
 *   LHCb namespace declaration
 */
namespace LHCb  {
  /** @class DimDataSender DimDataSender.cpp GaudiOnline/DimDataSender.cpp
    *
    *  Implementation of the NetworkDataSender using DIM.
    *
    *  @author Markus Frank
    */
  class DimDataSender : public NetworkDataSender  {
  protected:
    int m_svcID;
    /// DIM command handler for receiving event request
    static void i_req_receive_handler(void* tag, void* buf, int* size)  {
      if ( *size <= 0 ) return;
      DimDataSender* h = *(DimDataSender**)tag;
      char* p = ::strchr((char*)buf,':');
      if ( p )  {
        *p = 0;
        h->handleEventRequest(::dis_get_conn_id(),p+1,(const char*)buf);
      }
    }
  public:
    /// Standard algorithm constructor
    DimDataSender(const std::string& nam, ISvcLocator* pSvc) 
    : NetworkDataSender(nam, pSvc), m_svcID(0) {}
    /// Standard Destructor
    virtual ~DimDataSender()   {}
    /// Subscribe to network requests
    virtual StatusCode subscribeNetwork()  {
      MsgStream output(msgSvc(),name());
      std::string nam = RTL::processName() + "/EventRequest";
      output << MSG::DEBUG << "Access command service:" << nam << endmsg;
      m_svcID = ::dis_add_cmnd((char*)nam.c_str(),(char*)"C",i_req_receive_handler,(long)this);
      if ( !m_target.empty() )  {
        char req[256];
        ::snprintf(req,sizeof(req),"EVENT_SOURCE:%s",RTL::processName().c_str());
        nam = m_target + "/EventSource";
        int ret = ::dic_cmnd_service((char*)nam.c_str(),req,::strlen(req)+1);
        if ( 1 != ret )  {
          return StatusCode::FAILURE;
        }
      }
      DimServer::start();
      return StatusCode::SUCCESS;
    }
    /// Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork()  {
      if ( m_svcID != 0 ) ::dis_remove_service(m_svcID);
      m_svcID = 0;
      return StatusCode::SUCCESS;
    }
    /// Send data to target destination
    virtual StatusCode sendData(const Recipient& tar, const void* data, size_t len)  {
      std::string target = tar.name + "/Event";
      if ( len <= 56 ) {
        MsgStream err(msgSvc(),name());
        err << MSG::ERROR << "Bad DataType option ? [Must be 2] event length:" 
            << len << " for " << target << endmsg;
      }
      if ( 1 != ::dic_cmnd_service((char*)target.c_str(),(void*)data,len) )
        return StatusCode::FAILURE;
      return StatusCode::SUCCESS;
    }
  };
}
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,DimDataSender)
