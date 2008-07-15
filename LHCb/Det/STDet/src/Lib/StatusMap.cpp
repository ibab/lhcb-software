
#include "STDet/StatusMap.h"
#include <boost/assign/list_of.hpp>

const StatusMap::StatusToStringMap& StatusMap::statusDescription() {

  static StatusMap::StatusToStringMap s_map;
  if (s_map.empty()){
    s_map = boost::assign::map_list_of(DeSTSector::OK, "OK")
                                      (DeSTSector::Open, "Open")
                                      (DeSTSector::Short,"Short")
                                      (DeSTSector::Pinhole,"Pinhole")
                                      (DeSTSector::LowGain, "LowGain")
                                      (DeSTSector::ReadoutProblems,"ReadoutProblems")
                                      (DeSTSector::OtherFault,"OtherFault")
                                      (DeSTSector::Dead,"Dead"); 
  }
  return s_map;
}


/** stream operator for status */
std::ostream& operator << (std::ostream& s, DeSTSector::Status e){
  const StatusMap::StatusToStringMap& theMap = StatusMap::statusDescription();
  StatusMap::StatusToStringMap::const_iterator iter = theMap.find(e);
  return (iter == theMap.end() ? s << "Unknown":s <<iter->second);  
}
