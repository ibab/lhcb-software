
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
                                      (DeSTSector::Dead,"Dead")
                                      (DeSTSector::UnknownStatus, "Unknown"); 
  }
  return s_map;
}


DeSTSector::Status Status::toStatus(const std::string& str) {

  const ::Status::StatusToStringMap& statusMap = ::Status::statusDescription();
  ::Status::StatusToStringMap::const_iterator iterMap = statusMap.begin();
  while (iterMap != statusMap.end()){
    if (iterMap->second == str) break;
      ++iterMap;
  } // iterMap   
  return (iterMap == statusMap.end() ? DeSTSector::UnknownStatus : iterMap->first);
} 

std::string Status::toString(const DeSTSector::Status& tstatus) {
 
  const ::Status::StatusToStringMap& statusMap = ::Status::statusDescription();
  ::Status::StatusToStringMap::const_iterator iterMap = statusMap.find(tstatus);
   return (iterMap == statusMap.end() ? "UnknownStatus" : iterMap->second);
} 


const Status::StatusVector& Status::validBeetleStates() {
  static StatusVector s_vec;
  s_vec = boost::assign::list_of(DeSTSector::OK)(DeSTSector::ReadoutProblems)
                                (DeSTSector::Dead)(DeSTSector::OtherFault);
  return s_vec;
}

const Status::StatusVector& Status::protectedStates() {
  static StatusVector s_vec;
  s_vec = boost::assign::list_of(DeSTSector::NotBonded);
  return s_vec;
}

/** stream operator for status */
std::ostream& operator << (std::ostream& s, DeSTSector::Status e){
  const Status::StatusToStringMap& theMap = Status::statusDescription();
  Status::StatusToStringMap::const_iterator iter = theMap.find(e);
  return (iter == theMap.end() ? s << "Unknown":s <<iter->second);  
}


