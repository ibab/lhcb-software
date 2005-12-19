#ifndef _STFun_H
#define _STFun_H

// $Id: STFun.h,v 1.1.1.1 2005-12-19 15:58:58 mneedham Exp $
#include "GaudiKernel/SmartRefVector.h"
#include "Event/STDigit.h"

namespace STFun{

  /**
  * calcuate mean strip
  * @param vector of digits
  * @return mean strip
  */
  double position(const SmartRefVector<LHCb::STDigit>& digits);

};

#endif

