// $Id: HltLumiOdinReader.cpp,v 1.1.1.1 2008-07-17 08:50:25 panmanj Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/ODIN.h"
// local
#include "HltLumiOdinReader.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltLumiOdinReader
//
// 2008-07-15 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltLumiOdinReader );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltLumiOdinReader::HltLumiOdinReader( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
//  declareProperty("L0DULocation", m_l0Location = L0DUReportLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
HltLumiOdinReader::~HltLumiOdinReader() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltLumiOdinReader::initialize() {
  debug() << "==> Initialize" << endmsg;
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // default configuration
  registerSelection();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltLumiOdinReader::execute() {


  // get ODIN
  LHCb::ODIN* odin;
  if( exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
  }else{
    StatusCode sc = Error("ODIN cannot be loaded",StatusCode::FAILURE);
    return sc;
  }

  std::stringstream bxType("");
  std::stringstream trType("");
  bxType << (LHCb::ODIN::BXTypes) odin->bunchCrossingType();
  trType << (LHCb::ODIN::TriggerType) odin->triggerType();
  info() << " Trigger Type : " << trType.str() << " BXType : " << bxType.str() << endreq;
  setDecision(true);

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltLumiOdinReader::finalize() {
  debug() << " Entering the finalize " << endreq;
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================



