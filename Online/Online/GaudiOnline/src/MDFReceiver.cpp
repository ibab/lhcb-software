// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MDFReceiver.cpp,v 1.3 2006-11-22 16:33:26 frankb Exp $
//	====================================================================
//  MDFReceiver.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/xtoa.h"
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
    /// Pointer to MBM producer
    Producer*       m_prod;
    /// Partition ID
    int             m_partitionID;
    /// MBM buffer name
    std::string     m_buffer, m_bm_name;
    /// Process name
    std::string     m_procName;
    /// Flag to indicate if a partitioned buffer should be connected
    bool            m_partitionBuffer;
    /// Monitoring quantity: Number of events received from network
    int             m_evtCount;

  public:
    /// Standard algorithm constructor
    MDFReceiver(const std::string& name, ISvcLocator* pSvcLocator)
    :	Algorithm(name, pSvcLocator), m_prod(0), m_evtCount(0)
    {
      m_procName = RTL::processName();
      declareProperty("Buffer",          m_buffer = "EVENT");
      declareProperty("ProcessName",     m_procName);
      declareProperty("PartitionID",     m_partitionID = 0x103);
      declareProperty("PartitionBuffer", m_partitionBuffer=false);
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
      m_bm_name = m_buffer;
      int sc = amsuc_subscribe(WT_FACILITY_DAQ_EVENT,s_receiveEvt,s_taskDead,this);
      if ( AMS_SUCCESS != sc )  {
        log << MSG::ERROR << "amsuc_subscribe(WT_FACILITY_DAQ_EVENT) Failed status:" 
            << sc << ". " << endmsg;
        return StatusCode::FAILURE;
      }
      if ( m_partitionBuffer )  {
        char txt[32];
        m_bm_name += "_";
        m_bm_name += ::_itoa(m_partitionID,txt,16);
      }
      m_prod = new Producer(m_bm_name,m_procName,m_partitionID);
      declareInfo("EvtCount",   m_evtCount=0, "Number of events received from network");
      declareInfo("MBMName",    m_bm_name,    "MBM buffer name");
      declareInfo("PartitionID",m_partitionID,"Partition identifier");
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
