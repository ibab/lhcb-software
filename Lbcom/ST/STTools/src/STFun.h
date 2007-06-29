// $Id: STFun.h,v 1.5 2007-06-29 14:37:24 mneedham Exp $
#ifndef STFun_H
#define STFun_H 1

#include "GaudiKernel/SmartRefVector.h"
#include "Event/STDigit.h"
#include "Event/STCluster.h"
#include <utility>

namespace STFun{

  /**
  * calculate mean strip
  * @param  digits vector of digits
  * @return mean strip
  */
  std::pair<double, unsigned int > position(const SmartRefVector<LHCb::STDigit>& digits, double trim = 0);


  /**
  * calculate mean strip
  * @param  stripts vector of strip values
  * @return mean strip
  */
  std::pair<double, unsigned int > position(const LHCb::STCluster::ADCVector& strips, double trim = 0);

};

#endif // STFun_H
