// Include files 

// local
#include "EventCountHisto.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EventCountHisto
//
// 2007-01-08 : Robert Lambert
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( EventCountHisto )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventCountHisto::EventCountHisto( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_nEvents              ( 0 )
{

}
//=============================================================================
// Destructor
//=============================================================================
EventCountHisto::~EventCountHisto() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode EventCountHisto::initialize() {
  
  StatusCode sc = GaudiHistoAlg::initialize(); 
  if ( sc.isFailure() ) return sc;

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode EventCountHisto::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  // code goes here  

  m_nEvents++;

  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode EventCountHisto::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;

  info() <<  m_nEvents << " events processed" << endmsg;

  std::string tag="Events";
  std::string name="N Events";
  plot(1., tag, name ,0.,2,3, m_nEvents);

  return GaudiHistoAlg::finalize();
}

//=============================================================================
