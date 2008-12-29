// $Id: TTDigitOccupancy.h,v 1.2 2008-12-29 12:55:16 mneedham Exp $

#ifndef TTDigitOccupancy_H 
#define TTDigitOccypancy_H !

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "TTOccupancy.h"
#include "TTOccupancy.icpp"
#include "Event/STDigit.h"


typedef TTOccupancy<LHCb::STDigit> TTDigitOccupancy;

template<>
inline const std::string& TTDigitOccupancy::dataLocation() const{
  return LHCb::STDigitLocation::TTDigits;
}

template <>
inline const std::string TTDigitOccupancy::histoDirName() const{
  return "TTDigitOccupancy";
}

template <>
inline double TTDigitOccupancy::defaultThreshold() const{
  return 3.0; // no threshold for clusters
}

//template class ITOccupancy<LHCb::STDigit>;

DECLARE_ALGORITHM_FACTORY( TTDigitOccupancy );

#endif
