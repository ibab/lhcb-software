// $Id: ISTEffCalculator.h,v 1.2 2007-01-09 15:34:31 jvantilb Exp $
#ifndef ISTEFFCALCULATOR_H 
#define ISTEFFCALCULATOR_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ISTEffCalculator( "ISTEffCalculator", 1, 0 );

/** @class ISTEffCalculator ISTEffCalculator.h
 *  
 *  Tool interface to apply inefficiency
 *
 *  @author M Needham
 *  @date   08/11/2005
 */

class ISTEffCalculator : virtual public IAlgTool {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ISTEffCalculator; }

  // Actual operator function
  virtual bool accept() = 0;

};

#endif // ISTEFFCALCULATOR_H
