// $Id: IFilterCriterion.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef DAVINCIKERNEL_IFILTERCRITERION_H 
#define DAVINCIKERNEL_IFILTERCRITERION_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
namespace LHCb 
{
  class Particle;
}

/** @class IFilterCriterion IFilterCriterion.h Kernel/IFilterCriterion.h 
 *  Interface Class for FilterCriterion.
 *  Given a Particle, returns a yes/no decision.
 *  @author Paul Colrain
 *  @date   14/03/2002
 */

class GAUDI_API IFilterCriterion : virtual public IAlgTool {

public:

  DeclareInterfaceID(IFilterCriterion, 2, 0);

  /// Test if filter is satisfied
  virtual bool isSatisfied( const LHCb::Particle* const & ) = 0;

  /// Test if filter is satisfied
  virtual bool operator()( const LHCb::Particle* const & ) = 0;

};

#endif // DAVINCIKERNEL_IFILTERCRITERION_H


