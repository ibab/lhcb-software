// $Id: WriteSelResult.cpp,v 1.2 2007-09-07 15:34:22 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Event/SelResult.h"
// local
#include "WriteSelResult.h"

//-----------------------------------------------------------------------------
// Implementation file for class : WriteSelResult
//
// 2007-09-07 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( WriteSelResult );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
WriteSelResult::WriteSelResult( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IWriteSelResult>(this);
  declareProperty("Location",m_location = LHCb::SelResultLocation::Default );
  declareProperty("Node",m_node = "/Event/Phys/" );
}
//=============================================================================
// Destructor
//=============================================================================
WriteSelResult::~WriteSelResult() {} 

//=============================================================================

//=========================================================================
//  create and write a SelResult from algo name and result 
//=========================================================================
StatusCode WriteSelResult::write(std::string location, bool result, std::string decay ) {
  LHCb::SelResults* resultsToSave = getSelResults();
  LHCb::SelResult* myResult = new LHCb::SelResult();
  myResult->setLocation(m_node+location);
  myResult->setDecay(decay);
  myResult->setFound(result);
  resultsToSave->insert(myResult);
  return StatusCode::SUCCESS;
}
//=========================================================================
//  write a SelResult 
//=========================================================================
StatusCode WriteSelResult::write(LHCb::SelResult& sr ) {
  LHCb::SelResults* resultsToSave = getSelResults();
  LHCb::SelResult* myResult = new LHCb::SelResult(sr);
  resultsToSave->insert(myResult);
  return StatusCode::SUCCESS;
}

//=========================================================================
//   get pointer. Create if needed
//=========================================================================
LHCb::SelResults* WriteSelResult::getSelResults( ) {
  LHCb::SelResults* resultsToSave ;
  std::string location = m_location ; // 
  if ( exist<LHCb::SelResults>(location) ){
    if (msgLevel(MSG::VERBOSE)) verbose() << "SelResult exists already " << endmsg ;
    resultsToSave = get<LHCb::SelResults>(location);
  } else {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Putting new SelResult container " << endmsg ;
    resultsToSave = new LHCb::SelResults();
    put(resultsToSave,location);
  }
  return resultsToSave ;
}
