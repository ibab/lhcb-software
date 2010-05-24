// $Id: IDecayTreeFit.h,v 1.1 2010-05-24 14:45:19 ibelyaev Exp $
// ============================================================================
#ifndef KERNEL_IDECAYTREEFIT_H 
#define KERNEL_IDECAYTREEFIT_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// DaVinciTypes 
// ============================================================================
#include "Kernel/DecayTree.h"
// ============================================================================
// forward declarations 
// ============================================================================
namespace Gaudi { namespace Math { class ParticleParams ; } } //       LHCbMath
namespace LHCb  { class Particle   ; }                     //   Event/PhysEvent 
namespace LHCb  { class ParticleID ; }                     //   Kernel/PartProp 
namespace LHCb  { class DecayTree  ; }                     // Phys/DaVinciTypes   
// ============================================================================
/** @class IDecayTreeFit Kernel/IDecayTreeFit.h
 *  
 *
 *  @author Vanya Belyaev
 *  @date   2010-05-24
 */
class GAUDI_API IDecayTreeFit : public virtual IAlgTool 
{
public: 
  // ==========================================================================
  /// interface machinery 
  DeclareInterfaceID ( IDecayTreeFit , 1 , 0 ) ;
  // ==========================================================================
public: 
  // ==========================================================================
  /// the actual type for the fit parameters 
  typedef  Gaudi::Math::ParticleParams                                 Fitted ;
  // ==========================================================================
public: 
  // ==========================================================================
  /** fit the decay tree 
   *  @param decay  (INPUT) the decay tree to fit 
   *  @param origin (INPUT) the origin vertex 
   *  @return status code 
   */                                                           
  virtual StatusCode fit 
  ( const LHCb::Particle*   decay      , 
    const LHCb::VertexBase* origin = 0 ) const = 0 ;
  /** get the fitted parameters for the particle 
   *  @param particle (INPUT) the particle, NULL corresponds to the decay head  
   *  @return the fitted parameters, NULL for invalid argument/fit failure 
   */
  virtual const Fitted* fitted ( const LHCb::Particle* p = 0 ) const = 0 ;
  // ==========================================================================  
public:
  // ==========================================================================
  /// get the fit resuts in form of self-consistent decay tree 
  virtual LHCb::DecayTree fittedTree () const = 0 ;
  // ==========================================================================  
public:
  // ==========================================================================
  /** add the constraint 
   *  @param pid (INPUT) particle-ID to be constrained
   */
  virtual void addConstraint ( const LHCb::ParticleID& pid ) = 0 ;
  // ==========================================================================  
protected:
  // ==========================================================================
  /// virtual and protected desctructor 
  virtual ~IDecayTreeFit() ;               // virtual and protected desctructor 
  // ==========================================================================
} ;
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_IDECAYTREEFIT_H
// ============================================================================
