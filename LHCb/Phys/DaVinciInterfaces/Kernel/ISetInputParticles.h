// $Id: ISetInputParticles.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
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

  DeclareInterfaceID(ISetInputParticles, 2, 0);
  
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
  virtual ~ISetInputParticles() ;
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_ISETINPUTPARTICLES_H
// ============================================================================
