// $Id: IParticle2State.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef DAVINCIKERNEL_IPARTICLE2STATE_H 
#define DAVINCIKERNEL_IPARTICLE2STATE_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
#include "Event/Particle.h"

/** @class IParticle2State IParticle2State.h 
 *  Convert Particles to State and Back.
 *  For experts
 *  @author P. Koppenburg 
 *  @date   2006-05-16
 */

class GAUDI_API IParticle2State : virtual public IAlgTool {

public:

  DeclareInterfaceID(IParticle2State, 2, 0);
  
  /// Fill a Particle from a State
  virtual  StatusCode state2Particle( const LHCb::State& state, 
                                      LHCb::Particle& particle ) const = 0 ;
  
  /// Fill a State from Particle
  virtual  StatusCode particle2State( const LHCb::Particle& particle, 
                                      LHCb::State& state ) const = 0 ;

  /// Get Jacobian for Tx,Ty,q/P -> Px,Py,Pz transformation
  //  virtual Gaudi::Matrix5x5 stateJacobian(int charge, const Gaudi::XYZVector&) const = 0 ;


  /// test 
  virtual  StatusCode test( const LHCb::Particle& particle ) const = 0 ;
  /// test 
  virtual  StatusCode test( const LHCb::State& state ) const = 0 ;
  

};
#endif // DAVINCIKERNEL_IPARTICLE2STATE_H


