// $Id: IParticleValue.h,v 1.6 2010-05-14 15:03:45 ibelyaev Exp $
// ============================================================================
#ifndef DAVINCIKERNEL_IPARTICLEVALUE_H 
#define DAVINCIKERNEL_IPARTICLEVALUE_H 1
// ============================================================================
// Include files
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// Forward declarations
// ============================================================================
namespace LHCb { class Particle ; }
// ============================================================================
/** @class IParticleValue IParticleValue.h Kernel/IParticleValue.h
 *  Interface Class for Particle Values. 
 *  Given an LHCb::Particle, return the value of a variable calculated from its properties
 *
 *  @author Sebastian Neubert
 *  @date   05/08/2013
 *
 */
class GAUDI_API IParticleValue : virtual public IAlgTool 
{
public:
  // ==========================================================================
  DeclareInterfaceID ( IParticleValue , 4 , 0 ) ;
  /// Value: Return false if particle does not pass filter.
  virtual double operator()( const LHCb::Particle* ) const = 0;
  // ==========================================================================
protected:
  // ==========================================================================
  /// virtual and protected destructor
  virtual ~IParticleValue() ;              // virtual and protected destructor
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCIKERNEL_IPARTICLEVALUE_H
// ============================================================================


