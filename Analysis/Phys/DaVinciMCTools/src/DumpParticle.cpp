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
DECLARE_ALGORITHM_FACTORY( DumpParticle )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  DumpParticle::DumpParticle( const std::string& name,
                              ISvcLocator* pSvcLocator)
    : DaVinciAlgorithm ( name , pSvcLocator )
{ }

//=============================================================================
// Destructor
//=============================================================================
DumpParticle::~DumpParticle() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DumpParticle::execute()
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // code goes here
  LHCb::Particle::Range parts = particles();
  info() << "Will print out " << parts.size() << " particles" << endmsg ;
  for ( LHCb::Particle::Range::const_iterator p = parts.begin() ; p!=parts.end() ; ++p)
  {
    if (! dump(*p)) return StatusCode::FAILURE ;
  }

  setFilterPassed(true);  // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
// Dumping
//=============================================================================
StatusCode DumpParticle::dump(const LHCb::Particle* p)
{

  if (0==p) {
    fatal() << "NULL incoming particle" << endmsg;
    return StatusCode::FAILURE ;
  }
  info() << "############## Particle " << (p)->key() << " in "<< (p)->parent()->registry()->identifier()
         << " :\n " << *(p) << endmsg ;
  const LHCb::Vertex* v = (p)->endVertex();
  if (0!=v) {
    info() << "endVertex " << v->key() << " in "<< v->parent()->registry()->identifier()
           << " :\n " << *v << endmsg ;
    const SmartRefVector< LHCb::Particle > dauts = v->outgoingParticles();
    info() << "This vertex has " << dauts.size() << " outgoing Particles" << endmsg ;
    for (SmartRefVector< LHCb::Particle >::const_iterator d = dauts.begin() ; d!=dauts.end() ; ++d){
      if (0==(*d))
      {
        fatal() << "The particle going out of this vertex is " << *d << endmsg ;
        return StatusCode::FAILURE ;
      }
      else
      {
        info() << " --> Particle " << (*d)->key() << " in "
               << (*d)->parent()->registry()->identifier() << endmsg ;
      }
    }
  }
  const LHCb::ProtoParticle* pp = (p)->proto() ;
  if (0!=pp) {
    info() << "proto " << pp->key() << " in "<< pp->parent()->registry()->identifier()
           << " :\n " << *pp << endmsg ;
    const LHCb::Track* t = pp->track() ;
    if (0!=t) info() << "track " << t->key() << " in "<< t->parent()->registry()->identifier()
                     << " :\n " << *t << endmsg ;
  }
  const SmartRefVector< LHCb::Particle > dauts = p->daughters();
  info() << "This particle has " << dauts.size() << " daughters" << endmsg ;
  for (SmartRefVector< LHCb::Particle >::const_iterator d = dauts.begin() ;
       d != dauts.end(); ++d )
  {
    if (0==(*d)) {
      fatal() << "The daughter is " << *d << endmsg ;
      return StatusCode::FAILURE ;
    }
    if (!dump(*d)) return StatusCode::FAILURE ;
  }
  info() << "################ End of dump" << endmsg ;

  return StatusCode::SUCCESS ;
}

//=============================================================================
