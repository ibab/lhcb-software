// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MDFReceiver.cpp,v 1.1 2006-06-26 08:45:15 frankb Exp $
//	====================================================================
//  MDFReceiver.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "Event/RawBank.h"
#include "MDF/MDFHeader.h"
#include "MBM/Producer.h"
#include "MBM/mepdef.h"
#include "AMS/amsdef.h"
#include "WT/wtdef.h"

using namespace MBM;

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /**@class MDFReceiver MDFReceiver.cpp
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class MDFReceiver : public Algorithm   {
    Producer*   m_prod;
    int         m_partitionID;
    std::string m_buffer;
    std::string m_procName;
  public: 
    /// Standard algorithm constructor
    MDFReceiver(const std::string& name, ISvcLocator* pSvcLocator)
    :	Algorithm(name, pSvcLocator), m_prod(0)  
    {
      m_procName = RTL::processName();
      declareProperty("Buffer",      m_buffer = "EVENT");
      declareProperty("ProcessName", m_procName);
      declareProperty("PartitionID", m_partitionID = 0x103);
    }

    /// Standard Destructor
    virtual ~MDFReceiver()      {
    }

    static int s_receiveEvt(const amsuc_info* info, void* param)   {
      MDFReceiver* alg = (MDFReceiver*)param;
      return alg->receiveEvent(info);
    }
    int receiveEvent(const amsuc_info* info)    {
      MsgStream log(msgSvc(),name());
      char     source[64];
      unsigned int facility;
      if ( info->status != AMS_SUCCESS )  {
        log << MSG::ERROR << "Failed to spy on message. status:" << info->status << ". " << endmsg;
        return AMS_SUCCESS;
      }
      int sc = m_prod->getSpace(info->length);
      if ( sc == MBM_NORMAL ) {
        EventDesc& e = m_prod->event();
        size_t size = e.len;
        sc = ::amsc_read_message(e.data,&size,source,&facility,0);
        if ( AMS_SUCCESS != sc )   {
          log << MSG::ERROR << "Failed to read message. status:" << sc << ". " << endmsg;
          return AMS_SUCCESS;
        }
        RawBank*   b = (RawBank*)e.data;
        MDFHeader* h = (MDFHeader*)b->data();
        MDFHeader::SubHeader sh = h->subHeader();
        e.type    = EVENT_TYPE_EVENT;
        ::memcpy(e.mask,sh.H1->triggerMask(),sizeof(e.mask));
        e.len     = size;
        return m_prod->sendEvent() == MBM_NORMAL ? AMS_SUCCESS : AMS_ERROR;
      }
      log << MSG::ERROR << "Failed to retrieve " << info->length 
          << " bytes of space. status:" << sc << ". " << endmsg;
      return AMS_SUCCESS;
    }

    static int s_taskDead(const amsuc_info* info, void* param)  {
      return ((MDFReceiver*)param)->taskDead(info);
    }
    int taskDead(const amsuc_info* info)  {
      MsgStream log(msgSvc(),name());
      log << MSG::WARNING << "Sender task:" << info->source << " died...." << endmsg;
      return AMS_SUCCESS;
    }

    /// Initialize
    virtual StatusCode initialize()   {
      MsgStream log(msgSvc(),name());
      int sc = amsuc_subscribe(WT_FACILITY_DAQ_EVENT,s_receiveEvt,s_taskDead,this);
      if ( AMS_SUCCESS != sc )  {
        log << MSG::ERROR << "amsuc_subscribe(WT_FACILITY_DAQ_EVENT) Failed status:" 
            << sc << ". " << endmsg;
        return StatusCode::FAILURE;
      }
      m_prod = new Producer(m_buffer,m_procName,m_partitionID);
      return StatusCode::SUCCESS;
    }
    /// Finalize
    virtual StatusCode finalize()     {    
      if ( m_prod ) delete m_prod;
      m_prod = 0;
      m_procName = "";
      amsuc_remove(WT_FACILITY_DAQ_EVENT);
      return StatusCode::SUCCESS;
    }
    /// Execute procedure
    virtual StatusCode execute()    {
      return StatusCode::SUCCESS;
    }
  };
}

#include "GaudiKernel/AlgFactory.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFReceiver);
