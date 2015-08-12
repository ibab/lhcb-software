// Include files

// local
#include "ResetHits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ResetHits
//
// 2008-08-20 : Stephanie Hansmann-Menzemer
//-----------------------------------------------------------------------------


DECLARE_ALGORITHM_FACTORY( ResetHits )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ResetHits::ResetHits( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_othitcreator   ( "Tf::OTHitCreator/OTHitCreator" ),
    m_ithitcreator   ( "Tf::STHitCreator<Tf::IT>/ITHitCreator" ),
    m_tthitcreator   ( "Tf::STHitCreator<Tf::TT>/TTHitCreator")
{ 
  declareProperty( "ResetVeloHits",       m_veloHits = true ); 
  declareProperty( "ResetTTHits",         m_TTHits   = true );
  declareProperty( "ResetOTHits",         m_OTHits   = true );
  declareProperty( "ResetITHits",         m_ITHits   = true );
  
}
//=============================================================================
// Destructor
//=============================================================================
ResetHits::~ResetHits() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ResetHits::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  // load our hit creators
    sc = ( m_othitcreator.retrieve() &&
           m_ithitcreator.retrieve() &&
	   m_tthitcreator.retrieve());
    if ( sc.isFailure() ) return sc;
  
  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode ResetHits::execute() {

  if (m_OTHits)
   m_othitcreator->resetUsedFlagOfHits(); 
  if (m_ITHits)
   m_ithitcreator->resetUsedFlagOfHits();
  if (m_TTHits)
   m_tthitcreator->resetUsedFlagOfHits();
  
  return StatusCode::SUCCESS;
}

//=============================================================================
