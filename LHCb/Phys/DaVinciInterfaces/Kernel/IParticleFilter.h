// $Id: IParticleFilter.h,v 1.5 2010-05-12 13:32:24 jpalac Exp $
#ifndef DAVINCIKERNEL_IPARTICLEFILTER_H 
#define DAVINCIKERNEL_IPARTICLEFILTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
namespace LHCb {
  class Particle;
}


/** @class IParticleFilter IParticleFilter.h Kernel/IParticleFilter.h
 *  Interface Class for Particle Filters. 
 *  Given an LHCb::Particle, return true if it passes the filter.
 *
 *  @author Juan Palacios
 *  @date   12/05/2010
 *
 */

class GAUDI_API IParticleFilter : virtual public IAlgTool {

public:

  DeclareInterfaceID(IParticleFilter, 3, 0);

  /// Filter. Return false if particle does not pass filter.
  virtual bool operator()( const LHCb::Particle* ) const = 0;
                    
};
#endif // DAVINCIKERNEL_IPARTICLEFILTER_H


