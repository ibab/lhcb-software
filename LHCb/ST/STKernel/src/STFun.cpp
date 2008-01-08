// $Id: STFun.cpp,v 1.1 2008-01-08 10:22:58 mneedham Exp $
#include "Kernel/STFun.h"


std::pair<double, unsigned int > STFun::position(const SmartRefVector<LHCb::STDigit>& digits, double trim)
{
  double maxCharge = 0.0;
  SmartRefVector<LHCb::STDigit>::const_iterator iterDigit = digits.begin();
  for (; iterDigit != digits.end(); ++iterDigit){
    if ((*iterDigit)->depositedCharge() > maxCharge) maxCharge =(*iterDigit)->depositedCharge(); 
  } // iterDigit

  const double trimVal = trim*maxCharge;

  unsigned int nUsed = 0;
  double trimmedCharge = 0.0; double firstMoment =0.0; 
  iterDigit = digits.begin();
  for (; iterDigit != digits.end() ; ++iterDigit){
    if ((*iterDigit)->depositedCharge() >= trimVal){
      firstMoment += ((*iterDigit)->depositedCharge()*(double)(*iterDigit)->channelID().strip());
      trimmedCharge += (*iterDigit)->depositedCharge() ;
      ++nUsed;
    }
  } // iterDigit
 
  // mean u
  return std::make_pair(firstMoment/trimmedCharge, nUsed);
}

std::pair<double, unsigned int> STFun::position(const LHCb::STCluster::ADCVector& strips, double trim)
{ 
 // mean u - weighted according to charge dep
  double firstMoment = 0.0;
  double trimmedCharge = 0; 

  // mean
  double maxCharge = 0.0;
  LHCb::STCluster::ADCVector::const_iterator iter = strips.begin();
  while (iter != strips.end()){
    if (iter->second > maxCharge) maxCharge = iter->second;
    ++iter; 
  }
  const unsigned int trimVal = int(trim*maxCharge) ;

  // calc trimmed mean
  unsigned int i =0; unsigned int nUsed = 0;
  iter = strips.begin();
  while (iter != strips.end()){
    if (iter->second >= trimVal){
      trimmedCharge += iter->second;
      firstMoment += iter->second
                   *(double)i;
      ++nUsed;
    }
    ++iter;
    ++i;
  }
 
  // mean u
  return std::make_pair(firstMoment/trimmedCharge, nUsed);
}
