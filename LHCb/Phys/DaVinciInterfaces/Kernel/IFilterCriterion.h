// $Id: IFilterCriterion.h,v 1.3 2010-05-14 15:03:45 ibelyaev Exp $
// ============================================================================
#ifndef DAVINCIKERNEL_IFILTERCRITERION_H 
#define DAVINCIKERNEL_IFILTERCRITERION_H 1
// ============================================================================
// Include files
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IParticleFilter.h"
// ============================================================================
// Forward declarations
// ============================================================================
// PhysEvent
namespace LHCb { class Particle; }
// ============================================================================
/** @class IFilterCriterion Kernel/IFilterCriterion.h 
 *  Interface Class for FilterCriterion.
 *  Given a Particle, returns a yes/no decision.
 *  @author Paul Colrain
 *  @date   14/03/2002
 */
class GAUDI_API IFilterCriterion : virtual public IParticleFilter
{
  // ==========================================================================
public:
  // ==========================================================================  
  DeclareInterfaceID ( IFilterCriterion , 4 , 0 ) ;
  // ==========================================================================  
public:
  // ==========================================================================  
  /// Test if filter is satisfied
  virtual bool isSatisfied ( const LHCb::Particle* ) const = 0 ;
  // ==========================================================================
protected:
  // ==========================================================================
  /// virtual & protected destructor 
  virtual ~IFilterCriterion () ;             // virtual & protectede destrcutor 
  // ==========================================================================  
};
// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCIKERNEL_IFILTERCRITERION_H
// ============================================================================


