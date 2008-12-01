#ifndef _STDigitFun_H
#define _STDigitFun_H 1

#include "Event/STDigit.h"
#include "Kernel/SiChargeFun.h"

/**  specialization for STDigit */
namespace SiChargeFun{
  template <> 
  inline double charge(LHCb::STDigit* input) {
  return double(input->depositedCharge());
  }
  template <> 
  inline double charge(const LHCb::STDigit* input) {
  return double(input->depositedCharge());
 }
}


#endif
