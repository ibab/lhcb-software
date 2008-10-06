// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/RawEvent2MBMMergerAlg.cpp,v 1.3 2008-10-06 11:49:19 frankb Exp $
//  ====================================================================
//  DecisionSetterAlg.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiOnline/MEPManager.h"
#include "Event/RawBank.h"
#include "MDF/MDFWriter.h"
#include "MDF/MDFHeader.h"
#include "MBM/Producer.h"
#include "RTL/rtl.h"

using namespace MBM;
static const unsigned int NO_ROUTING = (unsigned int)~0x0;

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /**@class RawEvent2MBMMergerAlg 
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class RawEvent2MBMMergerAlg : public MDFWriter   {
    /// Reference to MEP manager service
    IMEPManager*  m_mepMgr;
    /// Reference to BM producer
    Producer*     m_prod;
    /// Property: output buffer name
    std::string   m_bufferName;
    /// Property: Word 4 of trigger mask (Routing bits)
    unsigned int  m_routingBits;
    /// Monitoring quantity: Counter of number of bytes sent
    int           m_bytesDeclared;

  public:
    /// Standard algorithm constructor
    RawEvent2MBMMergerAlg(const std::string& nam, ISvcLocator* pSvc)
    :  MDFWriter(MDFIO::MDF_NONE, nam, pSvc), m_mepMgr(0), m_prod(0)
    {
      declareProperty("Buffer",      m_bufferName="RESULT");
      declareProperty("RoutingBits", m_routingBits=NO_ROUTING);
    }
    /// Standard Destructor
    virtual ~RawEvent2MBMMergerAlg()      {                                 }
    /// Initialize
    virtual StatusCode initialize()   {
      StatusCode status = service("MEPManager",m_mepMgr);
      if ( !status.isSuccess() )   {
	return error("Failed to access MEPManager service.");
      }
      m_prod = m_mepMgr->createProducer(m_bufferName,RTL::processName());
      //MEPID mepID = m_mepMgr->mepID();
      //m_prod = new Producer(mepID->resBuffer,mepID->processName,mepID->partitionID);
      if ( 0 == m_prod ) {
	return error("Failed to create event producer for buffer:"+m_bufferName);
      }
      declareInfo("SpaceCalls", m_spaceActions=0,  "Total number successful space requests.");
      declareInfo("SpaceErrors",m_spaceErrors=0,   "Total number failed space requests.");
      declareInfo("EventsOut",  m_writeActions=0,  "Total number of events declared to BM.");
      declareInfo("ErrorsOut",  m_writeErrors=0,   "Total number of declare errors.");
      declareInfo("BytesOut",   m_bytesDeclared=0, "Total number of bytes declared to BM.");
      return StatusCode::SUCCESS;
    }

    /// Finalize
    virtual StatusCode finalize()     {    
      if ( m_mepMgr )  {
        m_mepMgr->release();
        m_mepMgr = 0;
      }
      if ( m_prod )  {
        delete m_prod;
        m_prod = 0;
      }
      return StatusCode::SUCCESS;
    }

    /// Issue error condition
    StatusCode error(const std::string& msg) const {
      MsgStream log(msgSvc(),name());
      log << MSG::ERROR << msg << endmsg;
      return StatusCode::FAILURE;
    }

    /// Allocate space for IO buffer
    virtual MDFIO::MDFDescriptor getDataSpace(void* const /* ioDesc */, size_t len)  {
      try {
	int ret = m_prod->getSpace(len);
	if ( ret == MBM_NORMAL ) {
	  EventDesc& e = m_prod->event();
	  e.len = len;
	  return MDFDescriptor((char*)e.data,len);
	}
      }
      catch(std::exception& e)  {
	error("Got exception when asking for BM space:"+std::string(e.what()));
      }
      catch(...)  {
	error("Got unknown exception when asking for BM space");
      }
      error("Failed to get space for buffer manager.");
      return MDFDescriptor(0,-1);
    }

    /// Write MDF record from serialization buffer
    StatusCode writeBuffer(void* const ioDesc, const void* data, size_t len)  {
      try {
	EventDesc& e = m_prod->event();
	if ( 0 == e.data ) {
	  // If we end up here, the data were assembled from the raw event, but rather
	  // taken from an already existing MDF or BANK buffer.
	  // In this event we have to copy the data to the requested space.
	  MDFIO::MDFDescriptor dsc = getDataSpace(ioDesc,len);
	  if ( dsc.first == 0 ) {
	    return error("Failed to get space to copy data!");
	  }
	  ::memcpy(dsc.first,data,len);
	}
	if ( 0 != e.data ) {
	  RawBank*   b = (RawBank*)e.data;
	  MDFHeader* h = (MDFHeader*)b->data();
	  e.type       = EVENT_TYPE_EVENT;
	  e.len        = len;
	  const unsigned int* mask = h->subHeader().H1->triggerMask();
	  if ( m_routingBits != NO_ROUTING ) {
	    unsigned int m[] = { mask[0], mask[1], mask[2], m_routingBits };
	    h->subHeader().H1->setTriggerMask(m);
	  }
	  ::memcpy(e.mask,h->subHeader().H1->triggerMask(),sizeof(e.mask));
	  int ret = m_prod->sendEvent();
	  if ( MBM_NORMAL == ret )   {
	    MsgStream log(msgSvc(),name());
	    e.len = 0;
	    e.data = 0;
	    m_bytesDeclared += len;
	    log << MSG::DEBUG << "Wrote data of size:" << len << " to buffer " << m_bufferName << endmsg;
	    return StatusCode::SUCCESS;
	  }
	  e.len = 0;
	  e.data = 0;
	  return error("Failed to declare event to buffer manager:"+m_bufferName);
	}
	return error("Failed to declare event - no data present!");
      }
      catch(std::exception& e)  {
	return error("Got exception when declaring event:"+m_bufferName+" "+std::string(e.what()));
      }
      catch(...)  {
	return error("Got unknown exception when declaring event:"+m_bufferName);
      }
      return StatusCode::FAILURE;
    }
  };
}

#include "GaudiKernel/AlgFactory.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,RawEvent2MBMMergerAlg);
