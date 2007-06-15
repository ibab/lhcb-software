// $Id: FilterGoodDecay.cpp,v 1.4 2007-06-15 16:18:38 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "FilterGoodDecay.h"
#include "MCInterfaces/IMCDecayFinder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterGoodDecay
//
// 2004-11-16 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY(FilterGoodDecay );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterGoodDecay::FilterGoodDecay( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_pMCDecFinder(0)
{

}
//=============================================================================
// Destructor
//=============================================================================
FilterGoodDecay::~FilterGoodDecay() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FilterGoodDecay::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  m_pMCDecFinder = tool<IMCDecayFinder>("MCDecayFinder", this); 
  if ( !m_pMCDecFinder ){
    fatal() << "Cannot retrive MCDecayFinder" << endreq ;
    return StatusCode::FAILURE ; 
  }
  

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterGoodDecay::execute() {

  debug() << "==> Execute" << endmsg;
  
  if (m_pMCDecFinder->hasDecay()){
    info() << "The requested decay is found" << endreq ;
    setFilterPassed(true);
  } else {
    debug() << "The requested decay is not found" << endreq ;
    setFilterPassed(false);
  } 

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FilterGoodDecay::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
