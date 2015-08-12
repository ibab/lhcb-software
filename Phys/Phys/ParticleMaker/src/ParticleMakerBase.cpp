// $Id: ParticleMakerBase.cpp,v 1.8 2010-08-16 16:40:38 odescham Exp $
// Include files

// from EventSys
#include "Event/Particle.h"
#include "Event/Vertex.h"

// local
#include "ParticleMakerBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleMakerBase
//
// 2009-04-21 P. Koppenburg
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleMakerBase::ParticleMakerBase( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : DaVinciAlgorithm ( name , pSvcLocator )
  , m_apid   (   )
  , m_pp     ( 0 )
  , m_app    ( 0 )
  , m_brem    ( NULL )
{
  declareProperty ( "Input"   , m_input = LHCb::ProtoParticleLocation::Charged ) ;
  declareProperty ( "Particle" , m_pid = "UNDEFINED" ,
                    "Particle to create : pion, kaon, muon..."   ) ;
  declareProperty( "AddBremPhotonTo",  m_addBremPhoton,
                   "ParticleIDs to be Brem-corrected (default : electrons only)");
  m_addBremPhoton.push_back( "e+" );
}
//=========================================================================
//
//========================================================================
StatusCode ParticleMakerBase::initialize ( )
{
  const StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if ( getDecayDescriptor() == "" ) setDecayDescriptor(m_pid) ;

  // BremStrahlung correction
  m_brem = tool<IBremAdder>("BremAdder","BremAdder", this);

  if ( this->inputLocations().empty() )
  {
    this->inputLocations().push_back(m_input);
  }
  else
  {
    m_input = "";
  }

  return sc;
}

//=============================================================================
// Destructor
//=============================================================================
ParticleMakerBase::~ParticleMakerBase() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ParticleMakerBase::execute()
{
  LHCb::Particle::Vector newParts;

  StatusCode sc = makeParticles(newParts);
  if ( sc.isFailure() ) return sc;

  LHCb::Particle::ConstVector constParts ; /// @todo this is a hack due to CaloParticle...
  constParts.reserve(newParts.size());

  for ( LHCb::Particle::Vector::const_iterator i = newParts.begin() ;
        i != newParts.end() ; ++i )
  {
    constParts.push_back(*i);
    addBrem( *i );
  }

  this->markNewTrees(constParts);

  if ( msgLevel(MSG::DEBUG) )
  {
    // Log number of vertices and particles
    debug() << "Number of particles = "
            << this->particles().size() << endmsg;
    if ( ! this->primaryVertices().empty() )
    {
      debug() << "Number of primary vertices = "
              << this->primaryVertices().size() << endmsg;
    }
    else { debug() << "No primary vertices" << endmsg; }
  }

  setFilterPassed( !newParts.empty() );

  return sc;
}

//=============================================================================

StatusCode ParticleMakerBase::loadEventInput()
{
  if (msgLevel(MSG::VERBOSE))
  {
    verbose() << ">>> ProtoParticleMakerBase::loadEventInput: load ProtoParticles from "
              << this->inputLocations() << endmsg;
  }

  m_protos.clear();

  for ( std::vector<std::string>::const_iterator iLoc = this->inputLocations().begin();
        iLoc != this->inputLocations().end(); ++iLoc )
  {
    const LHCb::ProtoParticle::Container* pp =
      getIfExists< LHCb::ProtoParticle::Container > ( *iLoc) ;
    if ( pp )
    {
      if (msgLevel(MSG::VERBOSE))
      {
        verbose() << "load " << pp->size() << " ProtoParticles from "
                  << *iLoc << endmsg;
      }
      for ( LHCb::ProtoParticle::Container::const_iterator iPP = pp->begin();
            iPP != pp->end(); ++iPP )
      {
        m_protos.push_back(*iPP);
      }
    }
    else
    {
      Info("No ProtoParticles at " + *iLoc);
      continue;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================

void ParticleMakerBase::addBrem( LHCb::Particle* particle )
{
  bool ok = false;
  for ( std::vector<std::string>::iterator p = m_addBremPhoton.begin() ;
        m_addBremPhoton.end() != p; ++p )
  {
    if ( *p == m_pid ) 
    {
      ok = true;
      break;
    }
  }
  
  if ( !ok ) return;
  if ( !m_brem->addBrem( particle ) ) return;

  if (msgLevel(MSG::DEBUG))
    debug() << " ------- BremStrahlung has been added to the particle "
            << particle << " (PID=" << m_pid << ")" << endmsg;

  counter( "Applying Brem-correction to " +
           Gaudi::Utils::toString(particle->particleID().pid()) ) += 1;
}

//=============================================================================
