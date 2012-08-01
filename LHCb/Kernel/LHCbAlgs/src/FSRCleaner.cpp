// Include files

// local
#include "FSRCleaner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FSRCleaner
//
// 2012-08-01 : Rob Lambert
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FSRCleaner )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FSRCleaner::FSRCleaner( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : EmptyEventNodeCleaner( name , pSvcLocator )
{
  //set sensible FSR defaults
  m_inputStream = "/FileRecords";
  m_dataSvcName="FileRecordDataSvc";
  
}

//=============================================================================
// Destructor
//=============================================================================
FSRCleaner::~FSRCleaner() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FSRCleaner::finalize()
{
  //clean performed in execute of EmptyEventNodeCleaner... so let's call it!
  StatusCode sc=EmptyEventNodeCleaner::execute();
    
  // return, try to avoid shortcutting the usual finalize
  return (GaudiAlgorithm::finalize() && sc);
  
}
//=============================================================================
