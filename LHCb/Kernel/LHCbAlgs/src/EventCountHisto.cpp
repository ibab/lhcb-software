// $Id: EventCountHisto.cpp,v 1.1 2009-02-11 17:12:33 rlambert Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/AlgFactory.h"

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

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode EventCountHisto::execute() {

  debug() << "==> Execute" << endmsg;

  // code goes here  

  m_nEvents++;

  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode EventCountHisto::finalize() {

  debug() << "==> Finalize" << endmsg;

  info() <<  m_nEvents << " events processed" << endmsg;

  std::string tag="Events";
  std::string name="N Events";
  plot(1., tag, name ,0.,2,3, m_nEvents);

  return GaudiHistoAlg::finalize();
}

//=============================================================================
