// $Id: TrackTypeFilterCriterion.cpp,v 1.1.1.1 2004-08-24 06:35:15 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackTypeFilterCriterion.h"
#include "Event/ProtoParticle.h"
#include "Event/TrgTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackTypeFilterCriterion
//
// 2004-07-07 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackTypeFilterCriterion>          s_factory ;
const        IToolFactory& TrackTypeFilterCriterionFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackTypeFilterCriterion::TrackTypeFilterCriterion( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IFilterCriterion>(this);
  declareProperty( "RequireLong",       m_reqLong         = false );
  declareProperty( "VetoLong",          m_vetoLong        = false );
  declareProperty( "RequireDownstream", m_reqDownstream   = false );
  declareProperty( "VetoDownstream",    m_vetoDownstream  = false );
  declareProperty( "RequireUpstream",   m_reqUpstream     = false );
  declareProperty( "VetoUpstream",      m_vetoUpstream    = false );
  declareProperty( "RequireVelo",       m_reqVeloTrack    = false );
  declareProperty( "VetoVelo",          m_vetoVeloTrack   = false );
  declareProperty( "RequireBackward",   m_reqBackward     = false );
  declareProperty( "VetoBackward",      m_vetoBackward    = false );
  declareProperty( "RequireTtrack",     m_reqTtrack       = false );
  declareProperty( "VetoTtrack",        m_vetoTtrack      = false );
  declareProperty( "IgnoreNonTracks",   m_ignoreNonTracks = false );
}
//=============================================================================
// Destructor
//=============================================================================
TrackTypeFilterCriterion::~TrackTypeFilterCriterion() {}; 

//=============================================================================
// initialize() method
//=============================================================================
StatusCode TrackTypeFilterCriterion::initialize() {

  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc;
  debug() << ">>>   TrackTypeFilterCriterion::initialize() " << endreq;
  
  if ( m_reqLong ) debug() << "<<< Requiring Long tracks" << endreq ;
  if ( m_vetoLong ) debug() << "<<< Vetoing Long tracks" << endreq ;
  if ( m_reqDownstream ) debug() << "<<< Requiring Downstream tracks" 
                                 << endreq ;
  if ( m_vetoDownstream ) debug() << "<<< Vetoing Downstream tracks" 
                                  << endreq ;
  if ( m_reqUpstream ) debug() << "<<< Requiring Upstream tracks" 
                               << endreq ;
  if ( m_vetoUpstream ) debug() << "<<< Vetoing Upstream tracks" 
                                << endreq ;
  if ( m_reqVeloTrack ) debug() << "<<< Requiring VeloTracks" << endreq ;
  if ( m_vetoVeloTrack ) debug() << "<<< Vetoing VeloTracks" << endreq ;
  if ( m_reqBackward ) debug() << "<<< Requiring Backward tracks" 
                               << endreq ;
  if ( m_vetoBackward ) debug() << "<<< Vetoing Backward tracks" 
                                << endreq ;
  if ( m_reqTtrack ) debug() << "<<< Requiring Ttracks" << endreq ;
  if ( m_vetoTtrack ) debug() << "<<< Vetoing Ttracks" << endreq ;

  if ( m_ignoreNonTracks ) debug()  
    <<   "<<< Going to ignore resonances and neutrals" << endreq ;
  else  debug()  
    <<   "<<< Going to filter out resonances and neutrals!" << endreq ;
  if ((( m_reqLong ) && ( m_vetoLong )) || 
      (( m_reqDownstream ) && ( m_vetoDownstream )) ||
      (( m_reqUpstream ) && ( m_vetoUpstream )) ||
      (( m_reqVeloTrack ) && ( m_vetoVeloTrack )) ||
      (( m_reqBackward ) && ( m_vetoBackward )) ||
      (( m_reqTtrack ) && ( m_vetoTtrack ))){
    warning() << "Output will be empty! Check your options carefully" << endreq;
    warning() << "or switch OutputLevel to 2 to understand what's going on" 
              << endreq ;
  }
      
  return StatusCode::SUCCESS ;
}

//=============================================================================
bool TrackTypeFilterCriterion::isSatisfied( const Particle* const & part ) {

  verbose() << "Particle " << part->particleID().pid() 
            << " " << part->momentum() << endreq;

  // get ProtoParticle
  const ProtoParticle* proto = 
    dynamic_cast<const ProtoParticle*>(part->origin());
  // no protoparticle
  if ( ! proto ) { // composite particle or Trigger track
    verbose() << "This particle " << part->particleID().pid() << " has no protoparticle" << endreq;
    
    const TrgTrack* TrgT = dynamic_cast<const TrgTrack*>(part->origin());
    if ( TrgT ) { // Trigger track !
      verbose() << "This particle was made with a trigger track" << endreq;
      // assume all trigger tracks are long tracks. This might change some day!
      if ( m_vetoLong      ) return false ;
      if ( m_reqLong       ) return true ;
      if ( m_reqDownstream ) return false ;
      if ( m_reqUpstream   ) return false ;
      if ( m_reqVeloTrack  ) return false ;
      if ( m_reqBackward   ) return false ;
      if ( m_reqTtrack     ) return false ;
      else return true ;
    } else if ( m_ignoreNonTracks ) return true;
    else return false ;

  } else { // it is from a protoparticle
    // get Track
    const TrStoredTrack* track = proto->track();
    if ( !track ){ // neutral particle
      verbose() << "This particle " << part->particleID().pid() 
                << " has no track" << endreq;
      if ( m_ignoreNonTracks ) return true;
      else return false ;    
    }
    debug() << "This particle " << part->particleID().pid() 
            << " has track types L:" << track->isLong() 
            << " D: " << track->isDownstream()
            << " U: " << track->isUpstream()
            << " V: " << track->isVelotrack() 
            << " B: " << track->isBackward()
            << " T: " << track->isTtrack() << endreq ;
  
    if ((  m_reqLong       ) && ( !track->isLong()       )) return false ;
    if (( m_vetoLong       ) && (  track->isLong()       )) return false ;
    if ((  m_reqDownstream ) && ( !track->isDownstream() )) return false ;
    if (( m_vetoDownstream ) && (  track->isDownstream() )) return false ;
    if ((  m_reqUpstream   ) && ( !track->isUpstream()   )) return false ;
    if (( m_vetoUpstream   ) && (  track->isUpstream()   )) return false ;
    if ((  m_reqVeloTrack  ) && ( !track->isVelotrack()  )) return false ;
    if (( m_vetoVeloTrack  ) && (  track->isVelotrack()  )) return false ;
    if ((  m_reqBackward   ) && ( !track->isBackward()   )) return false ;
    if (( m_vetoBackward   ) && (  track->isBackward()   )) return false ;
    if ((  m_reqTtrack     ) && ( !track->isTtrack()     )) return false ;
    if (( m_vetoTtrack     ) && (  track->isTtrack()     )) return false ;

    verbose() << "This particle " << part->particleID().pid() <<" is accepted" 
              << endreq;
  }
  
  return true ;
}
//=============================================================================
bool TrackTypeFilterCriterion::operator()( const Particle* const & part ) {
  return this->isSatisfied( part );
}

