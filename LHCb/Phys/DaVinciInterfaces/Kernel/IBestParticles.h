// $Id: IBestParticles.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef DAVINCIKERNEL_IBESTPARTICLES_H 
#define DAVINCIKERNEL_IBESTPARTICLES_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"

/** @class IBestParticles IBestParticles.h Kernel/IBestParticles.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-07-15
 */
class GAUDI_API IBestParticles : virtual public IAlgTool {
public: 

  DeclareInterfaceID(IBestParticles, 2, 0);

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
