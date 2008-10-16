// $Id: TTClusterOccupancy.h,v 1.1 2008-10-16 13:12:03 mneedham Exp $

#ifndef TTClusterOccupancy_H 
#define TTClusterOccypancy_H !

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "TTOccupancy.h"
#include "TTOccupancy.icpp"
#include "Event/STCluster.h"


typedef TTOccupancy<LHCb::STCluster> TTClusterOccupancy;

template<>
inline const std::string& TTClusterOccupancy::dataLocation() const{
  return LHCb::STClusterLocation::TTClusters;
}

template <>
inline const std::string TTClusterOccupancy::histoDirName() const{
  return "TTClusterOccupancy";
}

template <>
inline double TTClusterOccupancy::defaultThreshold() const{
  return 0.0; // no threshold for clusters
}

//template class TTOccupancy<LHCb::STCluster>;

DECLARE_ALGORITHM_FACTORY( TTClusterOccupancy );

#endif
