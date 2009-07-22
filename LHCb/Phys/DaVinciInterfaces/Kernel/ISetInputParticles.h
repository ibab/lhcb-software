// $Id: ISetInputParticles.h,v 1.1.1.1 2009-07-22 20:54:51 jpalac Exp $
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
class ISetInputParticles : virtual public IInterface 
{
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
public: 
  // ==========================================================================
  /// Return the unique interface ID
  static const InterfaceID& interfaceID() ;
  // ==========================================================================  
protected:
  // ==========================================================================
  /// the destructor is virtual and protected 
  virtual ~ISetInputParticles() ;
  // ==========================================================================
};
// ============================================================================
namespace DaVinci
{
  // ==========================================================================
  namespace Utils
  {
    // ========================================================================
    /** set the input particles for some component
     *  @param component the component inself
     *  @param input the intut data 
     *  @return status code 
     * 
     *  - 300 : invalid component 
     *  - 301 : no valid ISetInputParticles interface 
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-07-11
     */
    StatusCode setInput 
    ( IInterface*                        component , 
      const LHCb::Particle::ConstVector& input     ) ;
    // ========================================================================
  } // end of namespace DaVinci::Utils 
  // ==========================================================================
} // end of namespace DaVinci 
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_ISETINPUTPARTICLES_H
// ============================================================================
