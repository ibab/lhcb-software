// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/src/MEPWriter.cpp,v 1.6 2007-11-19 19:27:32 frankb Exp $
//	====================================================================
//  MEPWriter.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiUtils/IIODataManager.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MEPWriter.h"
#include "MDF/MEPEvent.h"
#include "Event/RawEvent.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MEPWriter)

using namespace LHCb;

static void* extendBuffer(void* p, size_t len)   {
  StreamBuffer* s = (StreamBuffer*)p;
  s->reserve(len);
  return s->data();
}

/// Standard algorithm constructor
MEPWriter::MEPWriter(const std::string& name, ISvcLocator* pSvcLocator)
: MDFWriter(name, pSvcLocator), m_evID(0)
{
  declareProperty("PackingFactor",  m_packingFactor=20);
}

/// Standard Destructor
MEPWriter::~MEPWriter()     {
}

/// Execute procedure
StatusCode MEPWriter::execute()    {
  SmartDataPtr<RawEvent> raw(eventSvc(),"/Event/DAQ/RawEvent");
  if ( raw )  {
    StatusCode sc = StatusCode::SUCCESS;
    raw->addRef();
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
