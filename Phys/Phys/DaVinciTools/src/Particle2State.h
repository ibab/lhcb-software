// $Id: Particle2State.h,v 1.1 2006-05-16 17:54:38 pkoppenb Exp $
#ifndef PARTICLE2STATE_H 
#define PARTICLE2STATE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticle2State.h"            // Interface

/** @class Particle2State Particle2State.h
 *  
 *  Convert Particles to State and Back.
 *
 *  @author Patrick Koppenburg
 *  @date   2006-05-16
 */
class Particle2State : public GaudiTool, virtual public IParticle2State {
public: 
  /// Standard constructor
  Particle2State( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~Particle2State( ); ///< Destructor

  /// Fill a Particle from a State
  StatusCode state2Particle( const LHCb::State& state, LHCb::Particle& particle ) const ;
  
  /// Fill a State from Particle
  StatusCode particle2State( const LHCb::Particle& particle, LHCb::State& state ) const ;

  /// test 
  StatusCode test( const LHCb::Particle& particle ) const  ;
  /// test 
  StatusCode test( const LHCb::State& state ) const  ;

protected:

private:
  /// Get Jacobian for Tx,Ty,q/P -> Px,Py,Pz transformation
  Gaudi::Matrix5x5 stateJacobian(int charge, const Gaudi::XYZVector&) const ;

};
#endif // PARTICLE2STATE_H
