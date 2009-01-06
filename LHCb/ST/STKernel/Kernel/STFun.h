#ifndef _STKernel_STFun_H
#define _STKernel_STFun_H

#include "STBitsPolicy.h"
#include "GaudiKernel/SmartRefVector.h"
#include "Event/STDigit.h"
#include "Event/STCluster.h"
#include "Event/STTELL1BoardErrorBank.h"
#include "Kernel/StripRepresentation.h"
#include "Kernel/PPRepresentation.h"

#include <utility>

namespace STFun{

  /** quantize the interstrip fraction  from 'float' */
  double stripFraction(const double interStripPos);

  /**
  * calculate mean strip
  * @param  digits vector of digits
  * @return mean strip
  */
  std::pair<double, unsigned int > position(const SmartRefVector<LHCb::STDigit>& digits, double trim = 0);


  /**
  * calculate mean strip
  * @param  strips vector of strip values
  * @return mean strip
  */
  std::pair<double, unsigned int > position(const LHCb::STCluster::ADCVector& strips, double trim = 0);

  /** check if a strip can be recovered using error bank
  * @param LHCb::STErrorBank error bank 
  * @param STDAQ::StripRepresentation
  * @param unsigned int pcn
  * @return canBeRecovered  
  */
  bool canBeRecovered(const LHCb::STTELL1BoardErrorBank* bank, 
                      const STDAQ::StripRepresentation& strip,
                      const unsigned int pcn);

}

inline double STFun::stripFraction(const double interStripPos){
  return (LHCb::Math::round((1<<STBitsPolicy::interStripPrecision)*interStripPos))/double(1<<STBitsPolicy::interStripPrecision);
}

inline bool STFun::canBeRecovered(const LHCb::STTELL1BoardErrorBank* bank, 
                                  const STDAQ::StripRepresentation& strip,
                                  const unsigned int pcn){

  STDAQ::PPRepresentation ppRep = STDAQ::PPRepresentation(strip);
  unsigned int pp, beetle, port, chan;
  ppRep.decompose(pp, beetle, port, chan); // split up the word 
  const LHCb::STTELL1Error* errorInfo = bank->ppErrorInfo(pp);
  bool ok = false;
  if (errorInfo != 0 ){
    if (errorInfo->linkInfo(beetle, port,pcn) == LHCb::STTELL1Error::kNone){
      ok = true;
    }
  }
  return ok ; 
}

#endif //_STKernel_STFun_H
