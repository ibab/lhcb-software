#ifndef ITDigitOccupancy_H 
#define ITDigitOccypancy_H !

#include "ITOccupancy.h"
#include "ITOccupancy.icpp"
#include "Event/STDigit.h"


typedef ITOccupancy<LHCb::STDigit> ITDigitOccupancy;

template<>
inline const std::string& ITDigitOccupancy::dataLocation() const{
  return LHCb::STDigitLocation::ITDigits;
}

template <>
inline const std::string ITDigitOccupancy::histoDirName() const{
  return "ITDigitOccupancy";
}

template <>
inline double ITDigitOccupancy::defaultThreshold() const{
  return 3.0; // no threshold for clusters
}

//template class ITOccupancy<LHCb::STDigit>;

DECLARE_NAMED_ALGORITHM_FACTORY( ITDigitOccupancy  , ITDigitOccupancy )

#endif
