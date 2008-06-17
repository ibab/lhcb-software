
#include "Kernel/STChannelID.h"
#include "Kernel/TTNames.h"
#include "boost/lexical_cast.hpp"

std::string LHCb::TTNames::UniqueSectorToString(const LHCb::STChannelID& chan) {
  std::string theString = UniqueRegionToString(chan) + "Sector" + boost::lexical_cast<std::string>(chan.sector());
  return theString;
}


std::string LHCb::TTNames::UniqueLayerToString(const LHCb::STChannelID& chan) {

  std::string layer = "UnknownLayer";
  if (chan.station() == 1){
    if ( chan.layer()  == 1){
      layer = "X" ;     
    } 
    else if (chan.layer() == 2) {
      layer = "U";
    }
  } 
  else if (chan.station() ==2 ) {
    if ( chan.layer()  == 1){
      layer = "V" ;     
    } 
    else if (chan.layer() == 2) {
      layer = "X";
    }
  } 
  else {
    // nothing
  }

  std::string theString = StationToString(chan) + layer;
  
  return theString;
}
