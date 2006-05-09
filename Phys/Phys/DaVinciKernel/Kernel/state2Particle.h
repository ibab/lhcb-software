// $Id: state2Particle.h,v 1.1 2006-05-09 16:21:27 pkoppenb Exp $
#ifndef STATE2PARTICLE_H 
#define STATE2PARTICLE_H 1

// Include files
#include "Event/Particle.h"

/** @function State2Particle State2Particle.h
 *  
 *  Function that converts a State to a Particle
 *
 *  @author Patrick Koppenburg
 *  @date   2006-05-05
 */
//=============================================================================
/// Fill Composite Particle from a state
//=============================================================================
StatusCode state2Particle( const LHCb::State& state, LHCb::Particle& particle ){
  
  // point on the track and error 
  particle.setReferencePoint( state.position() ) ;
  particle.setPosCovMatrix( state.errPosition()  ) ;
  
  // momentum
  Gaudi::XYZVector mom = state.momentum();
  double mass = particle.measuredMass();
  double e = sqrt( state.p()*state.p()+mass*mass );
  particle.setMomentum(  Gaudi::XYZTVector(mom.X(),mom.Y(),mom.Z(),e) ) ;

  // momentum error
  Gaudi::SymMatrix4x4 err ;
  err.Place_at(state.errMomentum(),0,0); // no error on mass
  particle.setMomCovMatrix(err);

  Gaudi::Matrix4x3 posMomMatrix;
  Gaudi::SymMatrix6x6 spm = state.posMomCovariance();
  for ( unsigned int i = 0 ; i<3; ++i){
    for ( unsigned int j = 0 ; j<3; ++j){
      posMomMatrix(i,j) = spm(3+i,j);  /// @todo : Check this
    }
  }
  particle.setPosMomCovMatrix(posMomMatrix);
  
  return StatusCode::SUCCESS ;
  
}
#endif // STATE2PARTICLE_H
