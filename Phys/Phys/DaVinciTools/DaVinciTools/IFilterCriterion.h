// $Id: IFilterCriterion.h,v 1.1 2002-03-27 20:27:45 gcorti Exp $
#ifndef DAVINCITOOLS_IFILTERCRITERION_H 
#define DAVINCITOOLS_IFILTERCRITERION_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
#include "Event/Particle.h"

static const InterfaceID IID_IFilterCriterion\
                           ("IFilterCriterion", 1 , 0); 

/** @class IFilterCriterion IFilterCriterion.h 
 *
 *  @author Paul Colrain
 *  @date   14/03/2002
 */

class IFilterCriterion : virtual public IAlgTool {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {
    return IID_IFilterCriterion;
  }

  /// Test if filter is satisfied
  virtual bool isSatisfied( const Particle* const & ) = 0;

  /// Test if filter is satisfied
  virtual bool operator()( const Particle* const & ) = 0;

};

#endif // DAVINCITOOLS_IFILTERCRITERION_H


