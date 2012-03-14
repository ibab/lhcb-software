// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "DumpParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DumpParticle
//
// 2012-03-14 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DumpParticle );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DumpParticle::DumpParticle( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
DumpParticle::~DumpParticle() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DumpParticle::initialize() {
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DumpParticle::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // code goes here  
  LHCb::Particle::Range parts = particles();
  info() << "Will print out " << parts.size() << " particles" << endmsg ;
  for ( LHCb::Particle::Range::const_iterator p = parts.begin() ; p!=parts.end() ; ++p){
    info() << "Particle " << (*p)->key() << " in "<< (*p)->parent()->registry()->identifier() 
           << " :\n " << *(*p) << endmsg ;
    const LHCb::Vertex* v = (*p)->endVertex();
    if (0!=v) info() << "endVertex " << v->key() << " in "<< v->parent()->registry()->identifier() 
                     << " :\n " << *v << endmsg ;
    const LHCb::ProtoParticle* pp = (*p)->proto() ;
    if (0!=pp) {
      info() << "proto " << pp->key() << " in "<< pp->parent()->registry()->identifier()
             << " :\n " << *pp << endmsg ;
      const LHCb::Track* t = pp->track() ;
      if (0!=t) info() << "track " << t->key() << " in "<< t->parent()->registry()->identifier()
                       << " :\n " << *t << endmsg ;
    }
  } 

  setFilterPassed(true);  // Mandatory. Set to true if event is accepted. 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DumpParticle::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize();
}

//=============================================================================
