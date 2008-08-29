// $Id: DummyAlley.cpp,v 1.5 2008-08-29 14:31:55 bdepaula Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "DummyAlley.h"
#include "Event/HltSummary.h"
// from LHCb core
#include "Event/ODIN.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : DummyAlley
//
// 2006-01-16 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DummyAlley );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DummyAlley::DummyAlley( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selection(*this)
{
  declareProperty("L0DULocation", m_l0Location = L0DUReportLocation::Default );
  declareProperty("RightType", m_RightType = -1 );
  declareProperty("WrongType", m_WrongType = -1 );
  m_selection.declareProperties();
}
//=============================================================================
// Destructor
//=============================================================================
DummyAlley::~DummyAlley() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DummyAlley::initialize() {
 // debug() << "==> Initialize" << endmsg;
  debug() << "==> Initialize" << endmsg;
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

   m_selection.registerSelection();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode DummyAlley::execute() {

//  m_l0 = get<L0DUReport>(m_l0Location);
  
  debug() << " Entering the execute " << endreq;
//  LHCb::HltSummary *summary = get<LHCb::HltSummary>(LHCb::HltSummaryLocation::Default);
//  summary->setDecision(true);
  if (!exist<LHCb::ODIN> ( LHCb::ODINLocation::Default )) {
    // m_incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
   setFilterPassed( false );
   return Error( "ODIN missing, skipping event", StatusCode::SUCCESS );
  }
  LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
  debug() << " Trigger type =  " << odin->triggerType() << endreq;
  if (m_RightType!= -1 && m_WrongType!=-1){ 
    warning() << "You are setting both the wanted and the unwanted trigger type. No events accepted" << endreq;
  } else if (m_WrongType == -1 && odin->triggerType()== m_RightType){
    debug() << "This choses trigger type " << m_RightType << endreq; 
    m_selection.output()->setDecision(true);
  } else if (m_RightType == -1 && odin->triggerType()!= m_WrongType){
    debug() << "This ignores trigger type " << m_WrongType << endreq; 
    m_selection.output()->setDecision(true);
  } else if (m_RightType == -1 && m_WrongType==-1) {
    debug() << "No specific trigger type. All events accepted" << endreq;
    m_selection.output()->setDecision(true);
  }
  

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DummyAlley::finalize() {
  debug() << " Entering the finalize " << endreq;
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================



