
#ifndef PARTICLEADDER_H
#define PARTICLEADDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IVertexFit.h"            // Interface

namespace LHCb { class IParticlePropertySvc; }

/** @class ParticleAdder ParticleAdder.h
 *
 *  Simple Particle Adder w/ IVertexFit interface
 *
 *
 *  @author Yasmine Amhis & OD
 *  @date   2006-11-30
 */
class ParticleAdder : public GaudiTool,
                      virtual public IVertexFit
{

public:

  /// Standard constructor
  ParticleAdder( const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

  /// Method to fit a vertex
  StatusCode fit ( LHCb::Vertex& ,
                   const LHCb::Particle::ConstVector& ) const ;

  /// Method to fit a vertex returning a Particle (that should already know its PID)
  StatusCode fit( const LHCb::Particle::ConstVector&,
                  LHCb::Vertex&  ,
                  LHCb::Particle&) const ;

  StatusCode reFit( LHCb::Particle& particle ) const;

  StatusCode add( const LHCb::Particle*,
                  LHCb::Vertex& ) const;

  StatusCode remove( const LHCb::Particle*,
                     LHCb::Vertex& ) const;

  StatusCode combine( const LHCb::Particle::ConstVector& daughter,
                      LHCb::Particle&                    mother  ,
                      LHCb::Vertex&                      vertex  ) const;

  virtual ~ParticleAdder( ); ///< Destructor

};
#endif // PARTICLEADDER_H
