#ifndef _ISTDigitSelector_H
#define _ISTDigitSelector_H

#include "GaudiKernel/IAlgTool.h"

namespace LHCb{ class STDigit;};

static const InterfaceID IID_ISTDigitSelector( "ISTDigitSelector", 0, 0 );

class ISTDigitSelector: virtual public IAlgTool {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ISTDigitSelector ; }

  /// event-by-event init
  virtual StatusCode initializeEvent() = 0;

  virtual bool execute(const LHCb::STDigit* aDigit) = 0;

};

#endif // _ISTDigitSelector_H
