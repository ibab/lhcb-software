//	====================================================================
//  EvtCollection.Write.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#define EVTCOLLECTION_WRITE_CPP

// Framework include files
#include "TestAlgorithm.h"  // Required for inheritance
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "Event/EventHeader.h"
#include "Event/MCVertex.h"
#include "Event/MCParticle.h"

class EvtNumberPrintout : public TestAlgorithm {
private:
  int m_evtCount;
  int m_printFreq;
public:
  /// Constructor: A constructor of this form must be provided.
  EvtNumberPrintout(const std::string& name, ISvcLocator* pSvcLocator);
  /// Standard Destructor
  virtual ~EvtNumberPrintout();
  /// Test execution procedure
  virtual StatusCode executeTest();
  /// Test finalize procedure
  virtual StatusCode finalizeTest();
};

#include "GaudiKernel/AlgFactory.h"
static const AlgFactory<EvtNumberPrintout>  Factory;
const IAlgFactory& EvtNumberPrintoutFactory = Factory;

/// Constructor: A constructor of this form must be provided.
EvtNumberPrintout::EvtNumberPrintout(const std::string& name,
                                     ISvcLocator* pSvcLocator)
:	TestAlgorithm(name, pSvcLocator), m_evtCount(-1)
{
  declareProperty("PrintFreq", m_printFreq=1);
}
/// Standard Destructor
EvtNumberPrintout::~EvtNumberPrintout()
{
}

/// Event callback
StatusCode EvtNumberPrintout::executeTest()
{
  // This just makes the code below a bit easier to read (and type)
  MsgStream log(msgSvc(), name());
  SmartDataPtr<MCParticles> mctracks(eventSvc(),MCParticleLocation::Default);
  SmartDataPtr<MCVertices>   mcvertices(eventSvc(),MCVertexLocation::Default);
  ++m_evtCount;
  if ( m_event && 0 != mctracks && 0 != mcvertices )
  {
    if ( m_evtCount%m_printFreq == 0 )
    {
      log << MSG::ALWAYS 
          << "================ EVENT:"; log.width(8); log << m_event->evtNum() 
          << " RUN:";                   log.width(8); log << m_event->runNum() 
          << " N(Track)=";              log.width(6); log << mctracks->size()
          << " N(Vertex)=";             log.width(6); log << mcvertices->size()
          << endreq;
    }
    return StatusCode::SUCCESS;
  }
  error(__FILE__, "Cannot retrieve basic event objects.", __LINE__);
  return StatusCode::FAILURE;
}

StatusCode EvtNumberPrintout::finalizeTest()
{
  MsgStream log(msgSvc(), name());
  log << MSG::ALWAYS
    << "=============================================================================" << endmsg
    << "===                    Final Test Results                                ====" << endmsg
    << "=============================================================================" << endmsg;
  return StatusCode::SUCCESS;
}
