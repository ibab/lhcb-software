// $Id: ParticleTransporter.cpp,v 1.24 2009-09-11 17:14:05 jonrob Exp $
#include <cmath>
// local
#include "ParticleTransporter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleTransporter
//
// 2006-01-20 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleTransporter::ParticleTransporter
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GaudiTool ( type, name , parent )
  , m_trackExtrapolator     ( NULL )
  , m_trackExtrapolatorName ( "TrackParabolicExtrapolator:PUBLIC" )
  , m_ppSvc          ( NULL )
  , m_particle2State ( NULL )
  , m_eID            ( 0    )
{
  //
  if      ( std::string::npos != name.find ( "Master"     ) ) 
  { m_trackExtrapolatorName = "TrackMasterExtrapolator:PUBLIC"     ; }
  else if ( std::string::npos != name.find ( "RungeKutta" ) ) 
  { m_trackExtrapolatorName = "TrackRungeKuttaExtrapolator:PUBLIC" ; }
  //
  declareInterface<IParticleTransporter>(this);
  declareProperty
    ( "TrackExtrapolator"                  , 
      m_trackExtrapolatorName              ,
      "Extrapolator to be use for tracks"  ) ;
}
//=============================================================================
// Destructor
//=============================================================================
ParticleTransporter::~ParticleTransporter() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode ParticleTransporter::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  if ( m_trackExtrapolatorName != "" )
  {
    debug() << "Using the " << m_trackExtrapolatorName
            << " tool to extrapolate particles from tracks" << endmsg;
    m_trackExtrapolator = tool<ITrackExtrapolator>(m_trackExtrapolatorName,this);
  }
  else { warning() << "No TrackExtrapolator given for tracks" << endmsg ; }

  m_ppSvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc", true);

  m_particle2State = tool<IParticle2State>("Particle2State"); // not private

  // a complicated way of getting 11
  const LHCb::ParticleProperty * pe = m_ppSvc->find("e-");
  m_eID = abs( pe->particleID().pid() );

  return sc;
}

//=============================================================================
// transport methods
//=============================================================================
StatusCode ParticleTransporter::transport(const LHCb::Particle* P,
                                          const double zNew,
                                          LHCb::Particle& transParticle)
{
  StatusCode sc = StatusCode::SUCCESS;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Transport PID " << P->particleID().pid()
            << " p " << P->momentum() << " from "
            << P->referencePoint()  << " to " << zNew << endmsg ;

  // avoid some "extra" self-assignements:
  if ( &transParticle != P ) { transParticle = LHCb::Particle(*P) ; }

  if ( ! P->isBasicParticle() )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Using DaVinci::Transporter::transportComposite" << endmsg;
    sc = DaVinci::Transporter::transportComposite(P, zNew, transParticle);
    if (!sc) return sc;
  } 
  else if ( P->charge() == 0 )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Using DaVinci::Transporter::transportNeutralBasic" << endmsg;
    sc = DaVinci::Transporter::transportNeutralBasic(P, zNew, transParticle);
    if (!sc) return sc;
  } 
  else
  {
    sc = transportChargedBasic(P, zNew, transParticle);
    if (!sc) return sc;
  }

  sc = checkParticle(transParticle);

  return sc;
}
//=============================================================================
StatusCode ParticleTransporter::transportAndProject(const LHCb::Particle* P,
                                                    const double zNew,
                                                    LHCb::Particle& transParticle)
{
  StatusCode sc = StatusCode::SUCCESS;

  // avoid some "extra" self-assignements:
  if ( &transParticle != P ) { transParticle = LHCb::Particle(*P) ; }

  if ( ! P->isBasicParticle() ) 
  {
    sc = DaVinci::Transporter::transportAndProjectComposite(P,
                                                            zNew,
                                                            transParticle);
    if (!sc) return sc;
  } 
  else if ( P->charge()==0 ) 
  {
    sc = DaVinci::Transporter::transportAndProjectNeutralBasic(P,
                                                               zNew,
                                                               transParticle);
    if (!sc) return sc;
  }
  else
  {
    sc = transportChargedBasic(P, zNew, transParticle);
    if (!sc) return sc;
  }

  sc = checkParticle(transParticle);

  return sc;
}
//=============================================================================
StatusCode
ParticleTransporter::transportChargedBasic(const LHCb::Particle* P,
                                           const double zNew,
                                           LHCb::Particle& transParticle)
{
  StatusCode sc = StatusCode::SUCCESS;

  if ( msgLevel(MSG::VERBOSE) ) 
  {
    sc = m_particle2State->test(*P);
    if ( sc.isFailure() ) return sc;
  }
  
  LHCb::State s ; // state to extrapolate
  sc = state(P,zNew,s);
  if ( sc.isFailure() ) return sc;
  if ( !m_trackExtrapolator )
  {
    return Error( "No extrapolator defined" );
  }
  sc = m_trackExtrapolator->propagate(s,zNew,P->particleID());

  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Extrapolated state is" << endmsg;
    verbose() << s << endmsg ;
  }

  sc = m_particle2State->state2Particle(s,transParticle);

  return sc;
}

//=============================================================================

StatusCode
ParticleTransporter::checkParticle(const LHCb::Particle& transParticle)
{
  const double e = transParticle.momentum().E();
  if ( std::isfinite(e) && !std::isnan(e) )
  {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Obtained Particle "
                << transParticle.particleID().pid() << endmsg;
      verbose() << transParticle << endmsg ;
    }
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Transported " << transParticle.particleID().pid()
              << " " << transParticle.momentum() << " to "
              << transParticle.referencePoint() << endmsg ;
    }
  } 
  else 
  {
    return Warning("Transported Particle gets infinite/NaN momentum. Check Track states used.");
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
// get a state from a Particle
//=============================================================================
StatusCode ParticleTransporter::state(const LHCb::Particle* P,
                                      const double zNew,
                                      LHCb::State& s) const
{
  // charged basic: no need to make
  verbose() << "Starting from Particle : " << *P << endmsg ;

  StatusCode sc = StatusCode::SUCCESS ;

  if ( P->charge() != 0 && P->isBasicParticle() ) 
  {
    // Particles from MCParticles
    if ( !P->proto() ) 
    {
      const LHCb::ParticleProperty *pp = m_ppSvc->find(P->particleID());
      if (pp) 
      {
        Warning( pp->particle()+" has no proto nor endVertex. Assuming it's from MC.",
                 StatusCode::SUCCESS );
      }
      else
      {
        err() << "Particle with unknown PID " << P->particleID().pid() << " has no endVertex. "
              <<  "Assuming it's from MC" << endmsg ;
      }
      sc = m_particle2State->particle2State(*P,s);
    } 
    else if (m_eID==P->particleID().abspid())
    {
      // Electrons. We don't want to loose Bremsstrahlung correction
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "Special treatment for electrons" << endmsg ;
      sc = m_particle2State->particle2State(*P,s);
    }
    else if ( !P->proto()->track() ) 
    {
      // Charged protopraticle without track -> error
      err() << "Basic Particle of ID " << P->particleID().pid() << " has no track" << endmsg;
      return StatusCode::FAILURE;
    } 
    else
    {
      // That's fine
      s = P->proto()->track()->closestState(zNew);
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Getting state closest to " << zNew << " at "
                << s.position().z() << " : " << s.stateVector() << endmsg ;
        debug() << "Track has " << P->proto()->track()->nStates() << ". First at "
                << P->proto()->track()->firstState().position().z() << " : "
                << P->proto()->track()->firstState().stateVector() << endmsg ;
      }
    }
  } 
  else
  { // make a state
    return m_particle2State->particle2State(*P,s);
  }
  
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Got state: " << s << endmsg ;

  return sc ;
}

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ParticleTransporter )
