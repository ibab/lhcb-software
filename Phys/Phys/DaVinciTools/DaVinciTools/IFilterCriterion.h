// $Id: IFilterCriterion.h,v 1.4 2004-03-11 16:12:34 pkoppenb Exp $
#ifndef DAVINCITOOLS_IFILTERCRITERION_H 
#define DAVINCITOOLS_IFILTERCRITERION_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
#include "Event/Particle.h"

static const InterfaceID IID_IFilterCriterion("IFilterCriterion", 1 , 0); 

/** @class IFilterCriterion IFilterCriterion.h DaVinciTools/IFilterCriterion.h 
 *  Interface Class for FilterCriterion.
 *  Given a Particle, returns a yes/no decision.
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


