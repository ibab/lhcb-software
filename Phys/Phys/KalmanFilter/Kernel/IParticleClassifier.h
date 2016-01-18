// $Id:$
// ============================================================================
#ifndef KERNEL_IPARTICLECLASSIFIER_H 
#define KERNEL_IPARTICLECLASSIFIER_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// KalmanFilter
// ============================================================================
#include "KalmanFilter/ParticleTypes.h"
// ============================================================================
// forward declaration
// ============================================================================
namespace LHCb { class Particle ; }
// ============================================================================
/** @class IParticleClassifier 
 *  Abstract interface for particle classification 
 *  @see  LoKi::KalmanFilter::ParticleType
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-04-30
 */
class GAUDI_API IParticleClassifier : virtual public IAlgTool 
{
 public:
  // ==========================================================================
  /// interface machinery 
  DeclareInterfaceID(IParticleClassifier, 2, 0) ;  
  // ==========================================================================  
 public:
  // ==========================================================================
  /** get the particle type 
   *  @see LoKi::KalmanFilter::ParticleType
   */
  virtual LoKi::KalmanFilter::ParticleType 
    particleType   ( const LHCb::Particle*   p )  const = 0 ;
  /** check the particle type 
   *  @see LoKi::KalmanFilter::ParticleType
   */
  virtual bool 
    isParticleType 
    ( const LHCb::Particle*                  p , 
      const LoKi::KalmanFilter::ParticleType t ) const = 0 ;
  // ==========================================================================
 public:
  // ==========================================================================
  /// is the combination of particles good enouth for vertex ? 
  virtual bool goodForVertex 
    ( const LHCb::Particle::Range&         parts ) const = 0 ;
  // ==========================================================================
 protected:
  // ==========================================================================
  // virtual and protected destructor 
  virtual ~IParticleClassifier () ; ///< virtual and protected destructor 
  // ==========================================================================
};
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // KERNEL_IPARTICLECLASSIFIER_H
// ============================================================================
