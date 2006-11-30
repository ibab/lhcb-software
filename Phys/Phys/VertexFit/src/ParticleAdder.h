
#ifndef PARTICLEADDER_H
#define PARTICLEADDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IVertexFit.h"            // Interface




/** @class ParticleAdder ParticleAdder.h
 *  
 *  Simple Particle Adder w/ IVertexFit interface
 * 
 *
 *  @author Yasmine Amhis & OD
 *  @date   2006-11-30
 */
class ParticleAdder : public GaudiTool, virtual public IVertexFit {
public: 
  /// Standard constructor
  ParticleAdder( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  StatusCode initialize();

  /// Method to fit a vertex 
  StatusCode fit( const LHCb::Particle::ConstVector&,LHCb::Vertex& ) const ;  

  /// Method to fit a vertex returning a Particle (that should already know its PID)
  StatusCode fit( const LHCb::Particle::ConstVector&,LHCb::Particle&,LHCb::Vertex& ) const ; 
  StatusCode reFit( LHCb::Particle& particle ) const {
     LHCb::Vertex* vertex = particle.endVertex() ;
     return fit( particle.daughtersVector(),
                 particle , *vertex         ) ;
  }
  StatusCode add(const LHCb::Particle*, 
                 LHCb::Vertex& ) const {
    Error("add is to be implemented for ParticleAdder");
    return StatusCode::FAILURE;
  }

  StatusCode remove(const LHCb::Particle*, 
                    LHCb::Vertex& ) const {
    Error("remove is to be implemented for ParticleAdder");
    return StatusCode::FAILURE;
  }


  StatusCode combine
  ( const LHCb::Particle::ConstVector& daughter, 
    LHCb::Particle&                    mother  , 
    LHCb::Vertex&                      vertex  ) const 
  {
    return fit ( daughter , mother , vertex ) ;
  }


  virtual ~ParticleAdder( ); ///< Destructor




protected:

  bool isResonance(const LHCb::Particle*) const; 
  bool isVertexed(const LHCb::Particle*) const;

 
private:
      

  IParticlePropertySvc* m_ppSvc;

};
#endif // PARTICLEADDER_H
