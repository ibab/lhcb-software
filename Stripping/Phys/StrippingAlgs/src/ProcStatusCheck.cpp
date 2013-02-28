#include "GaudiKernel/AlgFactory.h" 
#include "ProcStatusCheck.h"

#include <Event/ProcStatus.h>

//-----------------------------------------------------------------------------
// Implementation file for class : ProcStatusCheck
// 
// A simple algorithm that checks for the events aborted by reconstruction 
// algorithms. 
// 
// Checks for ProcStatus->aborted()
// 
// If OutputLevel == DEBUG, prints out the names and status codes of algorithms 
// in ProcStatus
//
// 2010-06-24 : Anton Poluektov
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ProcStatusCheck )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProcStatusCheck::ProcStatusCheck( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
}

//=============================================================================
// Destructor
//=============================================================================
ProcStatusCheck::~ProcStatusCheck() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode ProcStatusCheck::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  bool selected = false;
  
  LHCb::ProcStatus* procStat = get<LHCb::ProcStatus*>( LHCb::ProcStatusLocation::Default ); 
  if (procStat) {
    int procStatSize = procStat->algs().size();
    
    if ( procStat->aborted() ) {
      selected = true;
      
      if ( msgLevel(MSG::DEBUG) ) debug() << " ProcStat: aborted " << endmsg;
    }
    
    if ( msgLevel(MSG::DEBUG) ) debug() << " ProcStat size = " << procStatSize << endmsg;
    if( 0 < procStatSize ) {
      
      if ( msgLevel(MSG::DEBUG) ) { 
        for (int i = 0; i < procStatSize; i++) {
          debug() << "   Algorithm " << i+1 << " name " << 
                     procStat->algs()[i].first << ", status = " << 
                     procStat->algs()[i].second << endmsg;
        }
      }

    }
    
    
  }
      
  setFilterPassed( selected );
  return StatusCode::SUCCESS;
}

//=============================================================================
