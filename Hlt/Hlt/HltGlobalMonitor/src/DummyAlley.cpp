// $Id: DummyAlley.cpp,v 1.1 2008-08-01 10:49:09 graven Exp $
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
{
//  declareProperty("L0DULocation", m_l0Location = L0DUReportLocation::Default );
  declareProperty("RightType", m_RightType = -1 );
  declareProperty("WrongType", m_WrongType = -1 );
//  m_doInitSelections = false;
//  m_algoType = "DummyAlley";
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

  // default configuration
  // TODO; this needs to be solved with the TCK!!
//  registerSelection(m_outputSelectionName);
  registerSelection();

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
 LHCb::ODIN* odin = 0;
 try {
   odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
 }
 catch( const GaudiException& Exception ) {
  // m_incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
   this->setFilterPassed( false );
   return Error( "ODIN missing, skipping event", StatusCode::SUCCESS );
 }
  debug() << " Trigger type =  " << odin->triggerType() << endreq;
  if (m_RightType!= -1 && m_WrongType!=-1){ 
    warning() << "You are setting both the wanted and the unwanted trigger type. No events accepted" << endreq;
  }
  else if (m_WrongType == -1 && odin->triggerType()== m_RightType){
    debug() << "This choses trigger type " << m_RightType << endreq; 
    setDecision(true);
    this->setFilterPassed( true );
  }
  else if (m_RightType == -1 && odin->triggerType()!= m_WrongType){
    debug() << "This ignores trigger type " << m_WrongType << endreq; 
    setDecision(true);
    this->setFilterPassed( true );
  }
  else if (m_RightType == -1 && m_WrongType==-1) {
    debug() << "No specific trigger type. All events accepted" << endreq;
    setDecision(true);
    this->setFilterPassed( true );
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



