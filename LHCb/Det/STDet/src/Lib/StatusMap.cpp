
#include "STDet/StatusMap.h"
#include <boost/assign/list_of.hpp>

const Status::StatusToStringMap& Status::statusDescription() {

  static Status::StatusToStringMap s_map;
  if (s_map.empty()){
    s_map = boost::assign::map_list_of(DeSTSector::OK, "OK")
                                      (DeSTSector::Open, "Open")
                                      (DeSTSector::Short,"Short")
                                      (DeSTSector::Pinhole,"Pinhole")
                                      (DeSTSector::NotBonded, "NotBonded")
                                      (DeSTSector::LowGain, "LowGain")
                                      (DeSTSector::Noisy, "Noisy")
                                      (DeSTSector::ReadoutProblems,"ReadoutProblems")
                                      (DeSTSector::OtherFault,"OtherFault")
                                      (DeSTSector::Dead,"Dead"); 
  }
  return s_map;
}

const Status::StatusVector& Status::validBeetleStates() {
  static StatusVector s_vec;
  s_vec = boost::assign::list_of(DeSTSector::OK)(DeSTSector::ReadoutProblems)
                                (DeSTSector::Dead)(DeSTSector::OtherFault);
  return s_vec;
}

const Status::StatusVector& Status::protectedStates() {
  static StatusVector s_vec;
  s_vec = boost::assign::list_of(DeSTSector::Open)(DeSTSector::Short)
                                (DeSTSector::Pinhole)(DeSTSector::NotBonded);
  return s_vec;
}

/** stream operator for status */
std::ostream& operator << (std::ostream& s, DeSTSector::Status e){
  const Status::StatusToStringMap& theMap = Status::statusDescription();
  Status::StatusToStringMap::const_iterator iter = theMap.find(e);
  return (iter == theMap.end() ? s << "Unknown":s <<iter->second);  
}


