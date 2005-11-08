// Include files
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// local
#include "TrackCriteriaSelector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackCriteriaSelector
//
// 2005-05-04 : Eduardo Rodrigues (adaptations to new track event model)
//
//  28-07-2003: Jeroen van Tilburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackCriteriaSelector>          s_factory ;
const        IToolFactory& TrackCriteriaSelectorFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackCriteriaSelector::TrackCriteriaSelector( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_mcParticleJudge( 0 )
  , m_previousTrackType( Track::TypeUnknown )
  , m_previousMCParticle( 0 )
{
  // interfaces
  declareInterface<ITrackCriteriaSelector>(this);
  // job options
  declareProperty( "UniqueFlag",  m_uniqueFlag = true );
  declareProperty( "ValidFlag",   m_validFlag = true );
  declareProperty( "MinP",        m_minP = 0.0*GeV );
  declareProperty( "MaxP",        m_maxP = 100.0*TeV );
  declareProperty( "TrackTypes",  m_tracktypes );
  declareProperty( "MCParticles", m_mcParticleJudgeName = "TrackAcceptance" );
}

//=============================================================================
// Destructor
//=============================================================================
TrackCriteriaSelector::~TrackCriteriaSelector() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackCriteriaSelector::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  

  debug() << "==> Initialize" << endreq;

  // Retrieve the reconstructibility tool
  m_mcParticleJudge = tool<ITrackReconstructible>( m_mcParticleJudgeName );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Select the track
//=============================================================================
bool TrackCriteriaSelector::select( Track* track ) const
{
  bool selected = true;

  // Check the momentum of track (at first state)  
  State* firstState = *( track -> states().begin() );
  if ( firstState ) {
    double momentum = firstState -> p();
    if ( momentum < m_minP || momentum > m_maxP ) selected = false;
  }

  // Check if the track is of the requested type
  if ( !selectByTrackType( track ) ) selected = false;

  return selected;
}

//=============================================================================
// Select the MCParticle
//=============================================================================
bool TrackCriteriaSelector::select( MCParticle* mcParticle )
{
  bool selected = true;

  // Check the momentum of MCParticle
  double momentum = mcParticle -> momentum().vect().mag();  
  if ( momentum < m_minP || momentum > m_maxP ) selected = false;

  // Check if the MCParticle is of the requested type
  if ( !selectByTrackType( mcParticle ) ) selected = false;
  
  return selected;
}

//=============================================================================
// Select the Track only by track type, unique- and valid-flag
//=============================================================================
bool TrackCriteriaSelector::selectByTrackType( Track* track ) const
{
  bool selected = true;

  // Check the Unique flag
  if ( m_uniqueFlag && !( track->checkFlag( Track::Unique ) ) )
    selected = false;

  // Check the validity flag
  if ( m_validFlag && track->checkFlag( Track::Invalid ) )
    selected = false;

  // Check if the track is of the requested type
  const Track::Types& tracktype = track -> type();
  if ( tracktype == Track::TypeUnknown ||
       ( !m_tracktypes.empty() &&  
         std::find( m_tracktypes.begin(), m_tracktypes.end(), 
                    tracktype ) == m_tracktypes.end() ) ) selected = false;

  return selected;
}

//=============================================================================
// Select the MCParticle only by track type
//=============================================================================
bool TrackCriteriaSelector::selectByTrackType( MCParticle* mcParticle )
{
  bool selected = true;

  // Check if the MCParticle is of the requested type
  unsigned int tracktype = trackType( mcParticle );
  if ( tracktype == Track::TypeUnknown ||
       ( !m_tracktypes.empty() &&
         std::find( m_tracktypes.begin(), m_tracktypes.end(),  
                    tracktype ) == m_tracktypes.end() ) ) selected = false;

  return selected;
}

//=============================================================================
// Get the track type identifyer of the MCParticle
//=============================================================================
unsigned int TrackCriteriaSelector::trackType( MCParticle* mcPart )
{
  bool hasVelo = m_mcParticleJudge -> hasVelo( mcPart );
  bool hasSeed = m_mcParticleJudge -> hasSeed( mcPart );
  bool hasTT   = m_mcParticleJudge -> hasTT( mcPart );

  const HepLorentzVector fourMom = mcPart->momentum();

  int tracktype = Track::TypeUnknown;

  if ( hasVelo && hasSeed ) {            // long track
    tracktype = Track::Long;
  } else if ( hasVelo && hasTT ) {       // upstream track
    tracktype = Track::Upstream;
  } else if ( hasSeed && hasTT ) {       // downstream track
    tracktype = Track::Downstream;
  } else if ( hasVelo ) {                // velo track
    tracktype = Track::Velo;
  } else if ( hasSeed ) {                // seed track
    tracktype = Track::Ttrack;
  }

  m_previousTrackType  = tracktype;
  m_previousMCParticle = mcPart;

  return tracktype;
}

//=============================================================================
// Set the track type of a Track with an MCParticle's type
//=============================================================================
StatusCode TrackCriteriaSelector::setTrackType( MCParticle* mcPart,
                                                Track*& track )
{
  int tracktype = Track::TypeUnknown;

  if ( mcPart == m_previousMCParticle ) {
    tracktype = m_previousTrackType;
  }
  else {
    tracktype = trackType( mcPart );
  }

  track -> setType ( (Track::Types&) tracktype );
  if ( Track::TypeUnknown == tracktype ) return StatusCode::FAILURE;
  else                                   return StatusCode::SUCCESS;
}

//=============================================================================
