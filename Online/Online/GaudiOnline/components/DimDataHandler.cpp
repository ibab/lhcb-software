#include "GaudiKernel/Algorithm.h"
#define NO_LONGLONG_TYPEDEF
#include "dic.hxx"
#include "dis.hxx"
#include "WT/wtdef.h"
#include "MBM/Producer.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiOnline/NetworkDataHandler.h"

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {

  /** @class DimDataHandler DimDataHandler.cpp GaudiOnline/DimDataHandler.cpp
    *
    *  Implementation of the NetworkDataHandler using DIM.
    *
    *  @author Markus Frank
    */
  class DimDataHandler : public NetworkDataHandler  {
  protected:
    std::vector<int>         m_svcIDs;
    std::pair<size_t,void*>  m_evtData;
    bool checkSize(int siz)  {
      if ( siz > 0 ) return true;
      MsgStream error(msgSvc(), name());
      error << MSG::ERROR << "Skip empty message when expecting data" << endmsg;
      return false;
    }
    void getData(int size, void* buf) {
      m_evtData.first  = size;
      m_evtData.second = buf;
    }
    static std::string i_client_name() {
      char buff[256];
      ::dis_get_client(buff);
      char* p = ::strchr(buff,'@');
      if ( p ) *p = 0;
      ::strncat(buff,"/Event",sizeof(buff)-strlen(buff)-1);
      buff[sizeof(buff)-1] = 0;
      return buff;
    }
    static std::string i_server_name() {
      char buff[256];
      ::dic_get_server(buff);
      char* p = ::strchr(buff,'@');
      if ( p ) *p = 0;
      ::strncat(buff,"/Event",sizeof(buff)-strlen(buff)-1);
      buff[sizeof(buff)-1] = 0;
      return buff;
    }
    /// DIM command handler for receiving event request
    static void i_req_receive_handler(void* tag, void* buf, int* size)  {
      DimDataHandler* h = *(DimDataHandler**)tag;
      if ( !h->checkSize(*size) ) return;
      h->getData(*size,buf);
      StatusCode sc = h->handleEventRequest(::dis_get_conn_id(),i_client_name(),*size);
      if ( !sc.isSuccess() )  {
        MsgStream log(h->msgSvc(),h->name());
        log << MSG::ERROR << "Serious error in receiving event request." << endmsg;
        return;
      }
    }
    /// DIM client handler called when receiving event data
    static void i_evt_receive_handler(void* tag, void* buf, int* size)    {
      DimDataHandler* h = *(DimDataHandler**)tag;
      if ( !h->checkSize(*size) ) return;
      h->getData(*size,buf);
      if ( !h->handleEventData(i_server_name(), *size).isSuccess() )  {
        MsgStream log(h->msgSvc(),h->name());
        log << MSG::ERROR << "Serious error in receiving event data." << endmsg;
      }
    }
    /// DIM service handler called while updating service data
    static void i_evt_send_handler(void* tag, void** buf, int* size, int* first)  {
      static const char* empty = "";
      DimDataHandler* h = *(DimDataHandler**)tag;
      if ( *first )  {
        *size = 0;
        *buf  = (void*)empty;
        return;
      }
      *size = h->m_evtData.first;
      *buf  = h->m_evtData.second;
    }
  public:
    /// Standard algorithm constructor
    DimDataHandler(const std::string& nam, ISvcLocator* pSvc) : NetworkDataHandler(nam, pSvc) {}
    /// Standard Destructor
    virtual ~DimDataHandler()   {
    }
    /// Networking layer overload: Read request for event data from network
    virtual StatusCode readEventRequest(void* data, size_t* len)  {
      *len = m_evtData.first;
      ::memcpy(data, m_evtData.second, m_evtData.first);
      return StatusCode::SUCCESS;
    }
    /// Subscribe to network requests
    virtual StatusCode subscribeNetwork()  {
      if ( isNetEventProducer() )  {
        MsgStream log(msgSvc(),name());
        std::string nam = RTL::processName() + "/Event";
        log << MSG::DEBUG << "Creating data source:" << nam << endmsg;
        long id = ::dis_add_service((char*)nam.c_str(),(char*)"C",0,0,i_evt_send_handler,(long)this);
        m_svcIDs.push_back(id);
        nam += "Request";
        log << "Creating command service:" << nam << endmsg;
        id = ::dis_add_cmnd((char*)nam.c_str(),(char*)"C",i_req_receive_handler,(long)this);
        m_svcIDs.push_back(id);
        DimServer::start();
      }
      return StatusCode::SUCCESS;
    }
    /// Create network request for a single source
    virtual StatusCode createNetRequest(RecvEntry& src)  {
      MsgStream log(msgSvc(),name());
      log << "Creating event request to data source:" << src.name << endmsg;
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
      if ( src.identifier != 0 ) {
        ::dic_release_service(src.identifier);
        src.identifier = 0;
      }
    }
    /// Rearm network request for a single source
    virtual StatusCode rearmNetRequest(const RecvEntry& src)  {
      MsgStream log(msgSvc(),name());
      std::string svc = src.name + "Request";
      const std::string& req = requestMessage();
      log << MSG::DEBUG << "Sending event request to:" << svc << " -> " << req << endmsg;
      MBM::EventDesc& e = src.producer->event();
      ::dic_change_address(src.identifier, e.data, e.len);
      int id = ::dic_cmnd_service((char*)svc.c_str(),(void*)req.c_str(),req.length()+1);
      if ( 1 == id )  {
        return StatusCode::SUCCESS;
      }
      log << MSG::ERROR << "Failed to send request for events to:" << src.name << endmsg;
      return StatusCode::FAILURE;
    }
    /// Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork()  {
      for(std::vector<int>::const_iterator i=m_svcIDs.begin(); i != m_svcIDs.end(); ++i)
        ::dis_remove_service(*i);
      m_svcIDs.clear();
      return StatusCode::SUCCESS;
    }
    /// Send data to target destination
    virtual StatusCode sendData(const Recipient& tar, const void* /* data */, size_t /* len */)  {
      if ( 0 != tar.identifier && !m_svcIDs.empty() )  {
        int targets[2] = {int(tar.identifier), 0};
        int nupdated = ::dis_selective_update_service(m_svcIDs[0],targets);
        if ( 1 == nupdated )  {
          return StatusCode::SUCCESS;
        }
        MsgStream log(msgSvc(),name());
        log << MSG::ERROR << "[DIS error] Failed to send data to " << tar.name << endmsg;
      }
      MsgStream log(msgSvc(),name());
      log << MSG::ERROR << "[Invalid message target] Failed to send data to " << tar.name << endmsg;
      return StatusCode::FAILURE;
    }
    /// Read data from network
    virtual StatusCode readData(void* data, size_t* len)  {
      if ( *len <= m_evtData.first )  {
        *len = std::min(m_evtData.first,*len);
        if ( data != m_evtData.second )  {
          ::memcpy(data,m_evtData.second,*len);
        }
        //else  {
        //  error("Reusing existing buffer space....");
        //}
        return StatusCode::SUCCESS;
      }
      MsgStream log(msgSvc(),name());
      log << MSG::ERROR << "[Insufficient buffer] Failed to receive data." << endmsg;
      return StatusCode::FAILURE;
    }
  };
}
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,DimDataHandler)
