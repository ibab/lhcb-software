#ifndef ITClusterOccupancy_H 
#define ITClusterOccypancy_H !

#include "ITOccupancy.h"
#include "ITOccupancy.icpp"
#include "Event/STCluster.h"


typedef ITOccupancy<LHCb::STCluster> ITClusterOccupancy;

template<>
inline const std::string& ITClusterOccupancy::dataLocation() const{
  return LHCb::STClusterLocation::ITClusters;
}

template <>
inline const std::string ITClusterOccupancy::histoDirName() const{
  return "ITClusterOccupancy";
}

template <>
inline double ITClusterOccupancy::defaultThreshold() const{
  return 0.0; // no threshold for clusters
}

template <>
inline unsigned int ITClusterOccupancy::weight(const LHCb::STCluster* obj) const {
  return obj->size();
}

#include "STDet/DeSTSector.h"

template <>
inline double  ITClusterOccupancy::SN(const LHCb::STCluster* obj) const{ 
   const DeSTSector* sector = findSector(obj->channelID());
   return obj->totalCharge() / sector->noise(obj->channelID());
}



//template class ITOccupancy<LHCb::STCluster>;

DECLARE_NAMED_ALGORITHM_FACTORY( ITClusterOccupancy, ITClusterOccupancy )

#endif
