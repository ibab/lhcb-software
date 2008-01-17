// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/RawEvent2MBMMergerAlg.cpp,v 1.1 2008-01-17 17:30:10 frankm Exp $
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
    /// Monitoring quantity: Counter of number of bytes sent
    int           m_bytesDeclared;

  public:
    /// Standard algorithm constructor
    RawEvent2MBMMergerAlg(const std::string& nam, ISvcLocator* pSvc)
    :  MDFWriter(MDFIO::MDF_NONE, nam, pSvc), m_mepMgr(0), m_prod(0)
    {
      declareProperty("Buffer",m_bufferName="RESULT");
    }
    /// Standard Destructor
    virtual ~RawEvent2MBMMergerAlg()      {                                 }
    /// Initialize
    virtual StatusCode initialize()   {
      StatusCode status = service("MEPManager",m_mepMgr);
      if ( !status.isSuccess() )   {
	MsgStream err(msgSvc(),name());
	err << MSG::ERROR << "Failed to access MEPManager service." << endmsg;
        return status;
      }
      m_prod = m_mepMgr->createProducer(m_bufferName,RTL::processName());
      //MEPID mepID = m_mepMgr->mepID();
      //m_prod = new Producer(mepID->resBuffer,mepID->processName,mepID->partitionID);
      if ( 0 == m_prod ) {
	MsgStream err(msgSvc(),name());
	err << MSG::ERROR << "Failed to create event producer for buffer:" << m_bufferName << endmsg;
	return StatusCode::FAILURE;
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
	MsgStream log(msgSvc(),name());
	log << MSG::ERROR << "Got exception when asking for BM space:" << e.what() << endmsg;
      }
      catch(...)  {
	MsgStream log(msgSvc(),name());
	log << MSG::ERROR << "Got unknown exception when asking for BM space" << endmsg;
      }
      MsgStream err(msgSvc(),name());
      err << MSG::ERROR << "Failed to get space for buffer manager." << endmsg;
      return MDFDescriptor(0,-1);
    }

    /// Write MDF record from serialization buffer
    StatusCode writeBuffer(void* const /* ioDesc */, const void* /* data */, size_t len)  {
      try {
	EventDesc& e = m_prod->event();
	RawBank*   b = (RawBank*)e.data;
	MDFHeader* h = (MDFHeader*)b->data();
	e.type       = EVENT_TYPE_EVENT;
	e.len        = len;
	::memcpy(e.mask,h->subHeader().H1->triggerMask(),sizeof(e.mask));
	int ret = m_prod->sendEvent();
	if ( MBM_NORMAL == ret )   {
	  m_bytesDeclared += len;
	  MsgStream log(msgSvc(),name());
	  log << MSG::DEBUG << "Wrote data of size:" << len << " to buffer " << m_bufferName << endmsg;
	  return StatusCode::SUCCESS;
	}
	MsgStream log(msgSvc(),name());
	log << MSG::ERROR << "Failed to declare event to buffer manager." << endmsg;
      }
      catch(std::exception& e)  {
	MsgStream log(msgSvc(),name());
	log << MSG::ERROR << "Got exception when declaring event:" << e.what() << endmsg;
      }
      catch(...)  {
	MsgStream log(msgSvc(),name());
	log << MSG::ERROR << "Got unknown exception when declaring event." << endmsg;
      }
      return StatusCode::FAILURE;
    }
  };
}

#include "GaudiKernel/AlgFactory.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,RawEvent2MBMMergerAlg);
