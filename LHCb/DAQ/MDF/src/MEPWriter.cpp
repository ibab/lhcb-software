// $Id: MEPWriter.cpp,v 1.12 2009-02-06 09:37:57 frankb Exp $
//  ====================================================================
//  MEPWriter.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiUtils/IIODataManager.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MEPWriter.h"
#include "MDF/MEPEvent.h"
#include "Event/RawEvent.h"

using namespace LHCb;

static void* extendBuffer(void* p, size_t len)   {
  StreamBuffer* s = (StreamBuffer*)p;
  s->reserve(len);
  return s->data();
}

/// Standard algorithm constructor
MEPWriter::MEPWriter(const std::string& name, ISvcLocator* pSvcLocator)
  : MDFWriter(name, pSvcLocator), m_evID(0), m_halfWindow(0)
{
  declareProperty("PackingFactor",  m_packingFactor=20);
  declareProperty("MakeTAE",        m_makeTAE=false);
}

/// Standard Destructor
MEPWriter::~MEPWriter()     {
}

//=========================================================================
//  Initialisation, check parameter consistency
//=========================================================================
StatusCode MEPWriter::initialize ( ) {
  StatusCode sc = MDFWriter::initialize();
  if ( sc.isSuccess() ) {
    m_halfWindow = (m_packingFactor-1)/2;
    if ( m_makeTAE ) {
      if ( 7 < m_halfWindow ||
	   0 > m_halfWindow ||
	   2 * m_halfWindow +1 != m_packingFactor ) {
	MsgStream msg( msgSvc(), name() );
	msg << MSG::ERROR << "In TAE mode, window size should be odd and less than 15" << endmsg;
	return StatusCode::FAILURE;
      }
    }
  }
  return sc;
}

/// Execute procedure
StatusCode MEPWriter::execute()    {
  SmartDataPtr<RawEvent> raw(eventSvc(),"/Event/DAQ/RawEvent");
  if ( raw )  {
    StatusCode sc = StatusCode::SUCCESS;
    raw->addRef();
    if ( m_makeTAE ) change2TAEEvent(raw, m_halfWindow );
    m_events.insert(std::pair<unsigned int, RawEvent*>(m_evID++, raw.ptr()));
    if ( int(m_events.size()) == m_packingFactor )  {
      MEPEvent* me = 0;
      encodeMEP(m_events, 0x103, &m_data, extendBuffer, &me);
      int res = m_ioMgr->write(m_connection,m_data.data(),me->size()+me->sizeOf());
      if ( !res )  {
        MsgStream log(msgSvc(),name());
        log << MSG::ERROR << "Failed to write MEPS for event:"
            << m_evID-m_packingFactor << " to " << m_evID << endmsg;
        sc = StatusCode::FAILURE;
      }
      for(Events::iterator i=m_events.begin(); i != m_events.end(); ++i)  {
        (*i).second->release();
      }
      m_events.clear();
    }
    return sc;
  }
  return StatusCode::FAILURE;
}
