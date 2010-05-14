// $Id: IParticleFilter.h,v 1.6 2010-05-14 15:03:45 ibelyaev Exp $
// ============================================================================
#ifndef DAVINCIKERNEL_IPARTICLEFILTER_H 
#define DAVINCIKERNEL_IPARTICLEFILTER_H 1
// ============================================================================
// Include files
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// Forward declarations
// ============================================================================
namespace LHCb { class Particle ; }
// ============================================================================
/** @class IParticleFilter IParticleFilter.h Kernel/IParticleFilter.h
 *  Interface Class for Particle Filters. 
 *  Given an LHCb::Particle, return true if it passes the filter.
 *
 *  @author Juan Palacios
 *  @date   12/05/2010
 *
 */
class GAUDI_API IParticleFilter : virtual public IAlgTool 
{
public:
  // ==========================================================================
  DeclareInterfaceID ( IParticleFilter , 4 , 0 ) ;
  /// Filter: Return false if particle does not pass filter.
  virtual bool operator()( const LHCb::Particle* ) const = 0;
  // ==========================================================================
protected:
  // ==========================================================================
  /// virtual and protected destructor
  virtual ~IParticleFilter() ;              // virtual and protected destructor
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCIKERNEL_IPARTICLEFILTER_H
// ============================================================================


