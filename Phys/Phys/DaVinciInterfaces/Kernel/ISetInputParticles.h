// $Id: ISetInputParticles.h,v 1.4 2009-08-17 08:04:22 jpalac Exp $
// ============================================================================
#ifndef KERNEL_ISETINPUTPARTICLES_H 
#define KERNEL_ISETINPUTPARTICLES_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
/** @class ISetInputParticles ISetInputParticles.h Kernel/ISetInputParticles.h
 *
 *  A bit technical interface to allow some interactive manipulations 
 *  with CombineParticlesa algorithm form python.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-07-11
 */
class GAUDI_API ISetInputParticles : virtual public IInterface 
{
public:
  // ==========================================================================
  /// interface machinery
  DeclareInterfaceID(ISetInputParticles, 2, 0);
  // ==========================================================================
public:
  // ==========================================================================  
  /** the only one essential method:
   *  it sets the input particles
   *  @param input the vector of input particles 
   *  @return status code 
   */
  virtual StatusCode setInput 
  ( const LHCb::Particle::ConstVector& input )  = 0 ;
  // ==========================================================================
protected:
  // ==========================================================================
  /// the destructor is virtual and protected 
  virtual ~ISetInputParticles() ;    // the destructor is virtual and protected 
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_ISETINPUTPARTICLES_H
// ============================================================================
