// $Id: ParticleMakerBase.cpp,v 1.8 2010-08-16 16:40:38 odescham Exp $
// Include files

#include "GaudiKernel/DeclareFactoryEntries.h"

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
  : DVAlgorithm ( name , pSvcLocator )
  , m_apid   (   ) 
  , m_pp     ( 0 ) 
  , m_app    ( 0 )
  , m_brem    ( NULL )
{
  declareProperty ( "Input"   , m_input = LHCb::ProtoParticleLocation::Charged ) ; 
  declareProperty ( "Particle" , m_pid = "UNDEFINED" , "Particle to create : pion, kaon, muon..."   ) ;
  declareProperty( "AddBremPhotonTo",  m_addBremPhoton, "particleIDs to be Brem-corrected (default : electrons only)");
  m_addBremPhoton.push_back( "e+" );
}
//=========================================================================
//  
//========================================================================
StatusCode ParticleMakerBase::initialize ( ) {
  StatusCode sc = DVAlgorithm::initialize();
  if (!sc) return sc;

  if ( getDecayDescriptor() == "" ) setDecayDescriptor(m_pid) ;
  // BremStrahlung correction
  m_brem = tool<IBremAdder>("BremAdder","BremAdder", this);

  return sc;
}

//=============================================================================
// Destructor
//=============================================================================
ParticleMakerBase::~ParticleMakerBase() {};

//=============================================================================
// Main execution
//=============================================================================
StatusCode ParticleMakerBase::execute()  
{
  LHCb::Particle::Vector newParts ;
  
  StatusCode sc = makeParticles(newParts);
  if (!sc) return sc;

  LHCb::Particle::ConstVector constParts ; /// @todo this is a hack due to CaloParticle...
  for (LHCb::Particle::Vector::const_iterator i = newParts.begin() ; i!= newParts.end() ; ++i) {
    constParts.push_back(*i);
    addBrem( *i );
  }

  this->markTrees(constParts);
  this->saveParticles();

  if ( sc.isFailure() ){ return Error( "Not able to save particles" );}

  if ( msgLevel(MSG::DEBUG) ){
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

  setFilterPassed((!newParts.empty()));

  return sc;
}
