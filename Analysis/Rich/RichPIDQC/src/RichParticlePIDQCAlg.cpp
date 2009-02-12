// $Id: RichParticlePIDQCAlg.cpp,v 1.1.1.1 2009-02-12 12:58:14 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichParticlePIDQCAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichParticlePIDQCAlg
//
// 2009-02-11 : Chris Jones
//-----------------------------------------------------------------------------

using namespace Rich;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ParticlePIDQCAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticlePIDQCAlg::ParticlePIDQCAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ),
    m_jos       ( NULL ) 
{ 
  declareProperty( "ExtraHistos", m_extraHistos = true ); 
}

//=============================================================================
// Destructor
//=============================================================================
ParticlePIDQCAlg::~ParticlePIDQCAlg() { }

//=============================================================================
// Initialization
//=============================================================================
StatusCode ParticlePIDQCAlg::initialize()
{
  const StatusCode sc = DVAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;


  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ParticlePIDQCAlg::execute()
{
  // Never abort a sequence
  setFilterPassed(true);

  // get the desktop particles
  const LHCb::Particle::ConstVector & particles = desktop()->particles();
  if ( particles.empty() ) return StatusCode::SUCCESS;

  // Find all the stable daugthers of the input particles
  PartList stableParticles;
  for ( LHCb::Particle::ConstVector::const_iterator iP = particles.begin();
        iP != particles.end(); ++iP )
  {
    addParticle( *iP, stableParticles );
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Found " << stableParticles.size() << " stable Particles" << endmsg;

  // Loop over the stable particles and fill the default PID plots
  for ( PartList::const_iterator iP = stableParticles.begin();
        iP != stableParticles.end(); ++iP )
  {
    if ( msgLevel(MSG::DEBUG) ) debug() << " Reco : " << (*iP).second << endreq; 

    // Get the mass hypo typo for this Particle
    const LHCb::ParticleProperty * prop = partProp( (*iP).first->particleID() );
    // If not available, just abort
    if ( !prop ) { continue; }

    // stable particle type
    const Rich::ParticleIDType pid = pidType(prop);
    if ( pid == Rich::Unknown )
    {
      std::ostringstream mess;
      mess << "Unknown RICH stable particle : " << prop->particle();
      Warning( mess.str() ).ignore();
    }

    // fill the appropriate plots tool for this particle
    const LHCb::RichPID * rPID = (*iP).first->proto()->richPID();
    // Use a different plots tool for each decay tree
    //pidTool((*iP).second)->plots( rPID, pid );
    // Use same plots tool in all cases (not sure which is best...)
    pidTool("Plots")->plots( rPID, pid );

  }

  return StatusCode::SUCCESS;
}

const Rich::ParticleIDType 
ParticlePIDQCAlg::pidType( const LHCb::ParticleProperty * prop ) const
{
  Rich::ParticleIDType type = Rich::Unknown;
  if      ( abs(prop->pythiaID()) == 321  ) type = Rich::Kaon;
  else if ( abs(prop->pythiaID()) == 211  ) type = Rich::Pion;
  else if ( abs(prop->pythiaID()) == 2212 ) type = Rich::Proton;
  else if ( abs(prop->pythiaID()) == 11   ) type = Rich::Electron;
  else if ( abs(prop->pythiaID()) == 13   ) type = Rich::Muon;
  return type;
}

const Rich::Rec::IPIDPlots * ParticlePIDQCAlg::pidTool( const std::string & toolname ) const
{
  PIDToolMap::const_iterator iT = m_pidTools.find(toolname);
  if ( iT == m_pidTools.end() )
  {
    // turn on extra histos for each plots tool
    BooleanProperty p( "ExtraHistos", m_extraHistos );
    const StatusCode sc = joSvc()->addPropertyToCatalogue( name()+"."+toolname, p );
    if ( sc.isFailure() ) { this -> Error( "Error setting property" ); }
    // get the tool
    const Rich::Rec::IPIDPlots *& t = m_pidTools[toolname];
    t = tool<Rich::Rec::IPIDPlots>("Rich::Rec::PIDPlots", toolname, this );
    // return
    return t;
  }
  return iT->second;
}

void ParticlePIDQCAlg::addParticle( const LHCb::Particle * particle,
                                    PartList & stableParticles,
                                    const unsigned int recurCount,
                                    const std::string & history ) const
{
  // protect against too many recursive calls... (should never ever get this deep)
  if ( recurCount > 9999 )
  {
    Warning( "Recursive limit in addParticle reached. Aborting Particle tree scan" );
    return;
  }

  // get the history for the Particle
  const LHCb::ParticleProperty * prop = partProp( particle->particleID() );
  // If not available, just abort
  if ( !prop ) { return; }

  // Does this particle have a charged ProtoParticle
  const LHCb::ProtoParticle * proto = particle->proto();
  if ( proto )
  {
    if ( 0 != proto->charge() )
    {
      stableParticles.push_back( NamedParticle(particle,history+prop->particle()) );
    }
  }
  // if no proto, try and save descendants
  else if ( !particle->isBasicParticle() )
  {
    // loop over them and to add them instead
    const LHCb::Particle::ConstVector & daughters = particle->daughtersVector();
    for ( LHCb::Particle::ConstVector::const_iterator iP = daughters.begin();
          iP != daughters.end(); ++iP )
    {
      addParticle( *iP, stableParticles, recurCount+1,
                   history + prop->particle() + "To" );
    }
  }

}

const LHCb::ParticleProperty *
ParticlePIDQCAlg::partProp( const LHCb::ParticleID& id ) const
{
  const LHCb::ParticleProperty * prop = ppSvc()->find ( id );
  if ( NULL == prop )
  {
    std::ostringstream mess;
    mess << "LHCb::ParticleProperty missing for ParticleID " << id;
    Warning( mess.str(), StatusCode::SUCCESS, 1 );
  }
  return prop;
}

IJobOptionsSvc* ParticlePIDQCAlg::joSvc() const
{
  if (!m_jos)
  { m_jos = this -> svc<IJobOptionsSvc>( "JobOptionsSvc" ); }
  return m_jos;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ParticlePIDQCAlg::finalize()
{
  return DVAlgorithm::finalize();
}

//=============================================================================
