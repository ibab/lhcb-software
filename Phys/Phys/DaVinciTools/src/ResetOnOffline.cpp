// $Id: ResetOnOffline.cpp,v 1.2 2005-06-08 16:41:44 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "ResetOnOffline.h"
#include "Kernel/IOnOffline.h" 

//-----------------------------------------------------------------------------
// Implementation file for class : ResetOnOffline
//
// 2005-06-08 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<ResetOnOffline>          s_factory ;
const        IAlgFactory& ResetOnOfflineFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ResetOnOffline::ResetOnOffline( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
    declareProperty( "ResetToOffline", m_offline = false );
    declareProperty( "ResetToOnline", m_online = false );
}
//=============================================================================
// Destructor
//=============================================================================
ResetOnOffline::~ResetOnOffline() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ResetOnOffline::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_onOffline = tool<IOnOffline>("OnOfflineTool");
  if (!m_onOffline){
    err() << "No OnOffline Tool" << endmsg ;
    return StatusCode::FAILURE;
  }
  if ( m_online && m_offline ){
    err() << "Cannot set to ONLINE and OFFLINE at the same time" << endmsg ;
    return  StatusCode::FAILURE;
  } else if ( m_online ) {
    info() << "Will Set to ONLINE" << endmsg ;
  } else if ( m_offline ) {
    info() << "Will Set to OFFLINE" << endmsg ;
  } else warning() << "Will be doing nothing" << endmsg ;

  return this->execute(); // also in initialisation
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode ResetOnOffline::execute() {

  debug() << "==> Execute" << endmsg;
  if ( m_online) {
    StatusCode sc = m_onOffline->setOnline( m_online );
    return sc ;
  } else if ( m_offline ) {
    StatusCode sc = m_onOffline->setOnline( !m_offline );
    return sc ;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ResetOnOffline::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
