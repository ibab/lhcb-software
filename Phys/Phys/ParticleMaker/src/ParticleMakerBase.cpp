// $Id: ParticleMakerBase.cpp,v 1.1 2009-04-21 19:15:41 pkoppenb Exp $
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
  , m_p2s()
{
  setProperty( "AvoidSelResult", "true").ignore();
  setProperty( "DecayDescriptor", "\"null\"").ignore();
}


//=========================================================================
//  
//=========================================================================
StatusCode ParticleMakerBase::initialize ( ) {
  StatusCode sc = DVAlgorithm::initialize();
  m_p2s = tool<IParticle2State>("Particle2State"); // not private
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

  LHCb::Particle::ConstVector constParts ; /// @todo this is hack dur to CaloParticle...
  for (LHCb::Particle::Vector::const_iterator i = newParts.begin() ; i!= newParts.end() ; ++i) constParts.push_back(*i);
  
  sc = desktop()->saveTrees(constParts);
  if ( sc.isFailure() ){ return Error( "Not able to save desktop" );}

  if ( msgLevel(MSG::DEBUG) ){
    // Log number of vertices and particles
    debug() << "Number of particles in desktop = " 
            << desktop()->particles().size() << endmsg;
    debug() << "Number of primary vertices in desktop = " 
            << desktop()->primaryVertices()->size() << endmsg;
    debug() << "Number of secondary vertices in desktop = " 
            <<desktop()->secondaryVertices().size() << endmsg;
  }

  setFilterPassed((!newParts.empty()));

  return sc;
}
