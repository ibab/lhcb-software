// $Id: DummyAlley.cpp,v 1.4 2008-08-19 09:42:38 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "DummyAlley.h"
// from LHCb core
#include "Event/ODIN.h"
#include "Event/L0DUReport.h"


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
  if (m_RightType!= -1 && m_WrongType!=-1){ 
    error() << "You are setting both the wanted and the unwanted trigger type. This is an error" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode DummyAlley::execute() {

  debug() << " Entering the execute " << endreq;
  if (!exist<LHCb::ODIN> ( LHCb::ODINLocation::Default )) {
    // m_incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
   return Error( "ODIN missing, skipping event", StatusCode::SUCCESS );
  }
  LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
  debug() << " Trigger type =  " << odin->triggerType() << endreq;
  if (m_WrongType == -1 && odin->triggerType()== m_RightType){
    debug() << "This accepts trigger type " << m_RightType << endreq; 
    m_selection.output()->setDecision(true);
  } else if (m_RightType == -1 && odin->triggerType()!= m_WrongType){
    debug() << "This rejects trigger type " << m_WrongType << endreq; 
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
