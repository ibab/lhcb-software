
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

