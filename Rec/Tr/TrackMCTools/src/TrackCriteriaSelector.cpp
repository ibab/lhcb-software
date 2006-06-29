// $Id: TrackCriteriaSelector.cpp,v 1.7 2006-06-29 08:56:47 mneedham Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from Kernel/LHCbDefinitions
#include "Kernel/PhysicalConstants.h"

// local
#include "TrackCriteriaSelector.h"

#include "Event/MCParticle.h"
#include "Event/Track.h"

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
static const  ToolFactory<TrackCriteriaSelector>          s_factory ;
const        IToolFactory& TrackCriteriaSelectorFactory = s_factory ;

/// Standard constructor, initializes variables
TrackCriteriaSelector::TrackCriteriaSelector( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_mcParticleJudge( 0 )
  , m_previousTrackType( Track::TypeUnknown )
  , m_previousMCParticle( 0 )
  ,m_endTracker(950.0*Gaudi::Units::cm)
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
  declareProperty("rejectElectrons", m_rejectElectrons = false);
  declareProperty("rejectInteractions", m_rejectInteractions = false);

}

/// Destructor
TrackCriteriaSelector::~TrackCriteriaSelector() {};

/// Initialization
StatusCode TrackCriteriaSelector::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  

  debug() << "==> Initialize" << endreq;

  // Retrieve the reconstructibility tool
  m_mcParticleJudge = tool<ITrackReconstructible>( m_mcParticleJudgeName );

  return StatusCode::SUCCESS;
}

/// Select the track
bool TrackCriteriaSelector::select( Track* track ) const
{
  bool selected = true;

  // Check the momentum of track (at first state)  
  State* firstState = track->states().front();
  if ( firstState ) {
    double momentum = firstState -> p();
    if ( momentum < m_minP || momentum > m_maxP ) selected = false;
  }

  // Check if the track is of the requested type
  if ( !selectByTrackType( track ) ) selected = false;

  return selected;
}

/// Select the MCParticle
bool TrackCriteriaSelector::select( MCParticle* mcParticle )
{
  bool selected = true;

  // Check the momentum of MCParticle
  double momentum = mcParticle -> p();  
  if ( momentum < m_minP || momentum > m_maxP ) selected = false;

  // reject electrons
  if ((m_rejectElectrons)&&(mcParticle->particleID().abspid() == 11)) selected = false;

  // reject interactions
  if ((m_rejectInteractions)&&
      (zInteraction(mcParticle->endVertices()) < m_endTracker)) selected = false;

  // Check if the MCParticle is of the requested type
  if ( !selectByTrackType( mcParticle ) ) selected = false;
  
  return selected;
}

/// Select the Track only by track type, unique- and valid-flag
bool TrackCriteriaSelector::selectByTrackType( Track* track ) const
{
  bool selected = true;

  // Check the Unique flag
  if ( m_uniqueFlag && track->checkFlag( Track::Clone ) )
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

/// Select the MCParticle only by track type
bool TrackCriteriaSelector::selectByTrackType( MCParticle* mcParticle )
{
  bool selected = true;

  // Check if the MCParticle is of the requested type
  int tracktype = trackType( mcParticle );
  if ( tracktype == Track::TypeUnknown ||
       ( !m_tracktypes.empty() &&
         std::find( m_tracktypes.begin(), m_tracktypes.end(),  
                    tracktype ) == m_tracktypes.end() ) ) selected = false;

  return selected;
}

/// Get the track type identifyer of the MCParticle
unsigned int TrackCriteriaSelector::trackType( MCParticle* mcPart )
{
  bool hasVelo = m_mcParticleJudge -> hasVelo( mcPart );
  bool hasSeed = m_mcParticleJudge -> hasSeed( mcPart );
  bool hasTT   = m_mcParticleJudge -> hasTT( mcPart );

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

/// Set the track type of a Track with an MCParticle's type
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


double TrackCriteriaSelector::zInteraction(const SmartRefVector<MCVertex>& vertices) const{

  double z = 9999.*Gaudi::Units::m;
  SmartRefVector<MCVertex>::const_iterator iterVert = vertices.begin();
  unsigned int type = 9999;
  for (; iterVert < vertices.end(); ++iterVert){
    double zTest = (*iterVert)->position().z();
    type = (*iterVert)->type();
    if ((zTest < z)&&(realInteraction(type))){
      z = zTest;
    }
  } // iterVert

  return z;
}

bool TrackCriteriaSelector::realInteraction(unsigned int type) const{
  
  bool real = true;
  if ((type == MCVertex::PhotoElectric)||
      (type == MCVertex::RICHPhotoElectric) ||
      (type == MCVertex::Cerenkov) ||
      (type == MCVertex::DeltaRay)) {
   real = false;
  }
  return real;
}
