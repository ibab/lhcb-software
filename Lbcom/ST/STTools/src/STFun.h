#ifndef _STFun_H
#define _STFun_H

// $Id: STFun.h,v 1.2 2006-02-07 08:46:28 mneedham Exp $
#include "GaudiKernel/SmartRefVector.h"
#include "Event/STDigit.h"
#include "Event/STCluster.h"

#include <vector>
#include <utility>

namespace STFun{

  /**
  * calcuate mean strip
  * @param vector of digits
  * @return mean strip
  */
  double position(const SmartRefVector<LHCb::STDigit>& digits);


  /**
  * calcuate mean strip
  * @param vector of strip values
  * @return mean strip
  */
  double position(const LHCb::STCluster::ADCVector& strips);

};

#endif

