// $Id: STFun.h,v 1.4 2007-03-23 14:18:21 cattanem Exp $
#ifndef STFun_H
#define STFun_H 1

#include "GaudiKernel/SmartRefVector.h"
#include "Event/STDigit.h"
#include "Event/STCluster.h"

namespace STFun{

  /**
  * calculate mean strip
  * @param  digits vector of digits
  * @return mean strip
  */
  double position(const SmartRefVector<LHCb::STDigit>& digits);


  /**
  * calculate mean strip
  * @param  stripts vector of strip values
  * @return mean strip
  */
  double position(const LHCb::STCluster::ADCVector& strips);

};

#endif // STFun_H
