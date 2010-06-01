// $Id: ParticleMakerBase.cpp,v 1.7 2010-06-01 09:52:04 pkoppenb Exp $
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
{
  declareProperty ( "Input"   , m_input = LHCb::ProtoParticleLocation::Charged ) ; 
  declareProperty ( "Particle" , m_pid = "UNDEFINED" , "Particle to create : pion, kaon, muon..."   ) ;
}
//=========================================================================
//  
//=========================================================================
StatusCode ParticleMakerBase::initialize ( ) {
  StatusCode sc = DVAlgorithm::initialize();
  if (!sc) return sc;

  if ( getDecayDescriptor() == "" ) setDecayDescriptor(m_pid) ;

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
  for (LHCb::Particle::Vector::const_iterator i = newParts.begin() ; i!= newParts.end() ; ++i) constParts.push_back(*i);
  
  sc = desktop()->saveTrees(constParts);
  if ( sc.isFailure() ){ return Error( "Not able to save desktop" );}

  if ( msgLevel(MSG::DEBUG) ){
    // Log number of vertices and particles
    debug() << "Number of particles in desktop = " 
            << desktop()->particles().size() << endmsg;
    if ( ! this->primaryVertices().empty() ) 
    {
      debug() << "Number of primary vertices in desktop = " 
              << this->primaryVertices().size() << endmsg;
    }
    else { debug() << "No primary vertices in desktop " << endmsg; }
    
    debug() << "Number of secondary vertices in desktop = " 
            <<desktop()->secondaryVertices().size() << endmsg;
  }

  setFilterPassed((!newParts.empty()));

  return sc;
}
