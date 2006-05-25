// $Id: L0DUFromRaw.cpp,v 1.1 2006-05-25 22:37:17 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
// DAQEvent
#include "Event/RawEvent.h"
// L0Event
#include "Event/L0DUReport.h"

// local
#include "L0DUFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0DUFromRaw
//
// Extract & decode L0DU Block from rawEvent
// - version 0 : L0DU Report only is extracted
//
// 2006-05-25 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0DUFromRaw );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUFromRaw::L0DUFromRaw( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_L0DUReportLocation ( LHCb::L0DUReportLocation::Default  ) 
  , m_channelNames (  ) 
  , m_conditionNames (  ) 
{
  declareProperty( "ReportLocation"    , m_L0DUReportLocation );
  declareProperty( "Channels"          , m_channelNames );
  declareProperty( "Conditions"        , m_conditionNames );
}
//=============================================================================
// Destructor
//=============================================================================
L0DUFromRaw::~L0DUFromRaw() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0DUFromRaw::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DUFromRaw::execute() {

  debug() << "==> Execute" << endmsg;

  // put L0DUReport data object on TES
  LHCb::L0DUReport* report = new LHCb::L0DUReport( );
  put (report , m_L0DUReportLocation );

  // Update Channel and Condition Names
  report->setChannelNames(m_channelNames);
  report->setConditionNames(m_conditionNames);
  
  // Extract the decisions & conditions patterns from raw & Update L0DUReport
  SmartDataPtr<LHCb::RawEvent> raw(eventSvc(),LHCb::RawEventLocation::Default);
  std::vector<LHCb::RawBank*> banks =  raw->banks(LHCb::RawBank::L0DU);
  std::vector<LHCb::RawBank*>::iterator ibank=banks.begin(); // Single L0DU Bank    
  unsigned int* data = (*ibank)->data();
  report->setReport(*data);
  ++data;
  report->setConditionPattern(*data);


  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0DUFromRaw::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
