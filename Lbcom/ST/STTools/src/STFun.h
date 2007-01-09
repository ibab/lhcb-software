// $Id: STFun.h,v 1.3 2007-01-09 15:02:24 jvantilb Exp $
#ifndef STFun_H
#define STFun_H 1

#include "GaudiKernel/SmartRefVector.h"
#include "Event/STDigit.h"
#include "Event/STCluster.h"

namespace STFun{

  /**
  * calculate mean strip
  * @param vector of digits
  * @return mean strip
  */
  double position(const SmartRefVector<LHCb::STDigit>& digits);


  /**
  * calculate mean strip
  * @param vector of strip values
  * @return mean strip
  */
  double position(const LHCb::STCluster::ADCVector& strips);

};

#endif // STFun_H
