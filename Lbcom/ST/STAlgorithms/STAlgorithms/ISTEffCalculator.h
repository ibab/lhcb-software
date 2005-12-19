// $Id: ISTEffCalculator.h,v 1.1.1.1 2005-12-19 15:43:16 mneedham Exp $
#ifndef STALGORITHMS_IISTEFFCALCULATOR_H 
#define STALGORITHMS_ISTEFFCALCULATOR_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ISTEffCalculator( "ISTEffCalculator", 1, 0 );

/** @class ISTEffCalculator ISTEffCalculator.h
 *  
 *  To apply some inefficiency
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
#endif // ITALGORITHMS_ISTEFFCALCULATOR_H

