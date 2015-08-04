#ifndef TTClusterOccupancy_H 
#define TTClusterOccypancy_H !

// Gaudi
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

template <>
inline unsigned int TTClusterOccupancy::weight(const LHCb::STCluster* obj) const {
  return obj->size();
}

#include "STDet/DeSTSector.h"

template <>
inline double  TTClusterOccupancy::SN(const LHCb::STCluster* obj) const{ 
   const DeSTSector* sector = findSector(obj->channelID());
   return obj->totalCharge() / sector->noise(obj->channelID());
}



//template class TTOccupancy<LHCb::STCluster>;

DECLARE_NAMED_ALGORITHM_FACTORY( TTClusterOccupancy, TTClusterOccupancy )

#endif
