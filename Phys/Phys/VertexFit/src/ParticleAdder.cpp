// Include files 


#include "Event/Vertex.h"
#include "Event/Particle.h"

#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
// local
#include "ParticleAdder.h"

using namespace LHCb;
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleAdder
//
// 2006-11-30 : Yasmine Amhis & Olivier Deschamps
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleAdder::ParticleAdder( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IVertexFit>       (this);
  declareInterface<IParticleCombiner>(this);
}

//=============================================================================
// Destructor
//=============================================================================
ParticleAdder::~ParticleAdder() {}

//=============================================================================
// Fit the vertex from a vector of Particles
//=============================================================================
StatusCode ParticleAdder::fit
( const LHCb::Particle::ConstVector& parts,
  LHCb::Vertex&   V ,
  LHCb::Particle& P ) const
{
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "start ParticleAdder fit " <<endreq;
    debug() << "using " << parts.size() <<" particles" <<endreq;
  }

  P.clearDaughters();
  V.clearOutgoingParticles();
  V.setChi2(0.0);
  V.setNDoF(0);
  P.setEndVertex( &V );

  for(Particle::ConstVector::const_iterator iterP = parts.begin();
      iterP != parts.end(); ++iterP)
  {
    P.addToDaughters(*iterP);
    V.addToOutgoingParticles(*iterP);
  }

  const SmartRefVector<LHCb::Particle>& daughters = P.daughters();
  Gaudi::LorentzVector momentum;
  Gaudi::SymMatrix4x4  matrix;
  for(SmartRefVector<LHCb::Particle>::const_iterator id = daughters.begin();
      id !=daughters.end() ;++id)
  {
    const LHCb::Particle* daughter = *id;
    momentum += daughter->momentum();
    matrix   += daughter->momCovMatrix();
  }
  P.setMomentum( momentum );
  P.setMomCovMatrix( matrix );
  P.setMeasuredMass( P.momentum().M() );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Fit the vertex from a vector of Particles
//=============================================================================
StatusCode ParticleAdder::fit
( LHCb::Vertex& V ,
  const LHCb::Particle::ConstVector& parts ) const
{
  LHCb::Particle tPart;
  return fit(parts, V , tPart );
}
//=============================================================================
StatusCode ParticleAdder::reFit( LHCb::Particle& particle ) const
{
  LHCb::Vertex* vertex = particle.endVertex() ;
  return fit( particle.daughtersVector(), *vertex , particle ) ;
}
//=============================================================================
StatusCode ParticleAdder::add(const LHCb::Particle*,
                              LHCb::Vertex& ) const
{
  return Error("add is to be implemented for ParticleAdder");
}
//=============================================================================
StatusCode ParticleAdder::remove(const LHCb::Particle*,
                                 LHCb::Vertex& ) const
{
  return Error("remove is to be implemented for ParticleAdder");
}
//=============================================================================
StatusCode ParticleAdder::combine
( const LHCb::Particle::ConstVector& daughter,
  LHCb::Particle&                    mother  ,
  LHCb::Vertex&                      vertex  ) const
{
  return fit ( daughter , vertex , mother ) ;
}
//=============================================================================

//-----------------------------------------------------------------------------
// Declaration of the Tool Factory
//-----------------------------------------------------------------------------
DECLARE_TOOL_FACTORY(ParticleAdder)
//-----------------------------------------------------------------------------
