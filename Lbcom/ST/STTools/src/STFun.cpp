
#include "STFun.h"

double STFun::position(const SmartRefVector<LHCb::STDigit>& digits) { 
  // mean u - weighted according to charge dep
  double totalCharge = 0.0;
  double firstMoment = 0.0;
 
  SmartRefVector<LHCb::STDigit>::const_iterator iterDigit = digits.begin();
  while (iterDigit != digits.end()){
    totalCharge += (*iterDigit)->depositedCharge();
    firstMoment += ((*iterDigit)->depositedCharge()
                   *(double)(*iterDigit)->channelID().strip());
    ++iterDigit;
  }
 
  // mean u
  return (firstMoment/totalCharge);
}

double STFun::position(const LHCb::STCluster::ADCVector& strips) { 
  // mean u - weighted according to charge dep
  double totalCharge = 0.0;
  double firstMoment = 0.0;
 
  unsigned int i =0;
  LHCb::STCluster::ADCVector::const_iterator iter = strips.begin();
  while (iter != strips.end()){
    totalCharge += iter->second;
    firstMoment += iter->second
                   *(double)i;
    ++iter;
    ++i;
  }
 
  // mean u
  return (firstMoment/totalCharge);
}
