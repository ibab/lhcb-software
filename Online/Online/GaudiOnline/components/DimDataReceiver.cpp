#include "GaudiOnline/NetworkDataReceiver.h"
#include "GaudiKernel/SvcFactory.h"
#include "dic.hxx"
#include "dis.hxx"
/*
 *   LHCb namespace declaration
 */
namespace LHCb  {
    
  /** @class DimDataReceiver DimDataReceiver.cpp GaudiOnline/DimDataReceiver.cpp
    *
    *  Implementation of the NetworkDataReceiver using DIM.
    *
    *  @author Markus Frank
    */
  class DimDataReceiver : public NetworkDataReceiver  {
  protected:
    int  m_evtSvcID;
    int  m_evtSrcID;
    bool checkSize(int siz)  { return siz > 0;  }
    static const char* i_client_name() {
      static char buff[256];
      ::dis_get_client(buff);
      return buff;
    }
    /// DIM command handler for receiving event request
    static void i_src_add_handler(void* tag, void* buf, int* size)  {
      if ( *size <= 0 ) return;
      DimDataReceiver* h = *(DimDataReceiver**)tag;
      char *p = ::strchr((char*)buf,':');
      if ( !p ) {
        MsgStream info(h->msgSvc(), h->name());
        info << MSG::INFO << "Received spurious (invalid) data source request from:" << i_client_name() << endmsg;
        return;
      }
      h->handleSourceRequest(::dis_get_conn_id(),i_client_name(),p+1);
    }
    /// DIM client handler called when receiving event data
    static void i_evt_receive_handler(void* tag, void* buf, int* size)    {
      if ( *size <= 0 ) return;
      DimDataReceiver* h = *(DimDataReceiver**)tag;
      if ( *size <= 56 ) {
        MsgStream err(h->msgSvc(), h->name());
        err << MSG::ERROR << "Bad DataType option of sender:" << i_client_name() 
            << " [Must be 2] event length:" << *size << endmsg;
      }
      h->handleEventData(i_client_name(), buf, *size);
    }
  public:
    /// Standard algorithm constructor
    DimDataReceiver(const std::string& nam, ISvcLocator* pSvc) : NetworkDataReceiver(nam, pSvc) {}
    /// Standard Destructor
    virtual ~DimDataReceiver()   { }
    /// Subscribe to network requests
    virtual StatusCode subscribeNetwork()  {
      MsgStream debug(msgSvc(),name());
      std::string nam = RTL::processName() + "/Event";
      debug << MSG::DEBUG << "Creating data sources:" << nam << " and " << nam << "Source" << endmsg;
      m_evtSvcID = ::dis_add_cmnd((char*)nam.c_str(),(char*)"C",i_evt_receive_handler,(long)this);
      nam = RTL::processName() + "/EventSource";
      m_evtSrcID = ::dis_add_cmnd((char*)nam.c_str(),(char*)"C",i_src_add_handler,(long)this);
      DimServer::start();
      return StatusCode::SUCCESS;
    }
    /// Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork()  {
      ::dis_remove_service(m_evtSvcID);
      ::dis_remove_service(m_evtSrcID);
      return StatusCode::SUCCESS;
    }
    /// Create network request for a single source
    virtual StatusCode createNetRequest(RecvEntry& src)  {
      MsgStream debug(msgSvc(),name());
      debug << MSG::DEBUG << "Creating event request to data source:" << src.name << endmsg;
      src.identifier = ::dic_info_service((char*)src.name.c_str(),MONITORED,0,0,0,i_evt_receive_handler,(long)this,0,0);
      return StatusCode::SUCCESS;
    }
    /// Reset event request
    virtual StatusCode resetNetRequest(const RecvEntry& src)  {
      ::dic_change_address(src.identifier, 0, 0);
      return StatusCode::SUCCESS;
    }
    // Rearm network request for a single source
    virtual void deleteNetRequest(RecvEntry& src)  {
      if ( src.identifier != 0 )
        ::dic_release_service(src.identifier);
      src.identifier = 0;
      if ( src.buffer ) ::free(src.buffer);
      src.buffer = 0;
    }
    /// Rearm network request for a single source
    virtual StatusCode rearmNetRequest(const RecvEntry& src)  {
      char req[256];
      MsgStream log(msgSvc(),name());
      ::snprintf(req,sizeof(req),"EVENT_REQUEST:%s",RTL::processName().c_str());
      std::string svc = src.service + "/EventRequest";
      log << MSG::DEBUG << "Sending event request to:" << svc << " -> " << req << endmsg;
      if ( 1 == ::dic_cmnd_service((char*)svc.c_str(),(void*)req,::strlen(req)+1) )
        return StatusCode::SUCCESS;
      log << MSG::ERROR << "Failed to send request for events to:" << src.name << endmsg;
      return StatusCode::FAILURE;
    }
  };
}

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,DimDataReceiver)
