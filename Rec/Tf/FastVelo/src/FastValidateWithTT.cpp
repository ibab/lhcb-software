// Include files 

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "Event/Track.h"

// local
#include "FastValidateWithTT.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FastValidateWithTT
//
// 2011-01-19 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FastValidateWithTT )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FastValidateWithTT::FastValidateWithTT( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_validationTool(NULL)
{
  declareProperty( "InputName"     , m_inputName      = LHCb::TrackLocation::Velo );
}
//=============================================================================
// Destructor
//=============================================================================
FastValidateWithTT::~FastValidateWithTT() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FastValidateWithTT::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_validationTool = tool<ITrackSelector>( "FastTTValidationTool" );
  m_nForward = 0;
  m_nGood    = 0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FastValidateWithTT::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::Tracks* velo = get<LHCb::Tracks>( m_inputName );
  
  for ( LHCb::Tracks::iterator itT = velo->begin(); velo->end() != itT ; ++itT ) {
    if ( (*itT)->checkFlag( LHCb::Track::Backward ) ) continue;

    m_nForward++;

    bool accepted = m_validationTool->accept( *(*itT) );
    
    if ( accepted ) {
      debug() << " +++ Track is a good candidate" << endmsg;
      m_nGood++;
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FastValidateWithTT::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  info() << "***** From " << m_nForward << " forward tracks, " << m_nGood << " are validated = " 
         << double( m_nGood ) / double ( m_nForward ) << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
