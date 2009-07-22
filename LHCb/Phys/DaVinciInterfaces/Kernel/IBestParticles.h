// $Id: IBestParticles.h,v 1.1.1.1 2009-07-22 20:54:51 jpalac Exp $
#ifndef DAVINCIKERNEL_IBESTPARTICLES_H 
#define DAVINCIKERNEL_IBESTPARTICLES_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"

static const InterfaceID IID_IBestParticles ( "IBestParticles", 1, 0 );

/** @class IBestParticles IBestParticles.h Kernel/IBestParticles.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-07-15
 */
class IBestParticles : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IBestParticles; }
  virtual StatusCode Candidates( const LHCb::Particle::ConstVector & input,
                                 LHCb::Particle::ConstVector& output) = 0;
  //  virtual StatusCode Candidates( const Particles & input,
  //                                 Particles& output) = 0;
  virtual StatusCode BestCandidate( const LHCb::Particle::ConstVector& input,
                                    const LHCb::Particle* output) = 0;
  //  virtual StatusCode BestCandidate( Particles& input,
  //                                   LHCb::Particle* output) = 0;

protected:

private:

};
#endif // DAVINCIKERNEL_IBESTPARTICLES_H
