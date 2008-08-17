
#include "Kernel/STChannelID.h"
#include "Kernel/TTNames.h"
#include "boost/lexical_cast.hpp"
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

std::string LHCb::TTNames::UniqueSectorToString(const LHCb::STChannelID& chan) {
  std::string theString = UniqueRegionToString(chan) + "Sector" + boost::lexical_cast<std::string>(chan.sector());
  return theString;
}

std::vector<std::string> LHCb::TTNames::stations() {

  GaudiUtils::VectorMap<std::string,Station>::iterator iter = s_StationTypMap().begin();
  std::vector<std::string> stations; stations.reserve(s_StationTypMap().size());
  for ( ;iter != s_StationTypMap().end(); ++iter ){
    if (iter->first != "Unknownstation") stations.push_back(iter->first);
  } // iter

  return stations;
}

std::vector<std::string> LHCb::TTNames::detRegions() {

  GaudiUtils::VectorMap<std::string,detRegion>::iterator iter = s_detRegionTypMap().begin();
  std::vector<std::string> regions; regions.reserve(s_detRegionTypMap().size());
  for ( ;iter != s_detRegionTypMap().end(); ++iter ){
    if (iter->first != "UnknownRegion") regions.push_back(iter->first);
  } // iter

  return regions;
}

std::vector<std::string> LHCb::TTNames::layers() {
  //messy
  std::vector<std::string> layers = boost::assign::list_of("X")("U")("V");
  return layers;
}

std::vector<std::string> LHCb::TTNames::allStations() {

  return stations();
}

std::vector<std::string> LHCb::TTNames::allDetRegions() {

  typedef std::vector<std::string> Strings;
  Strings layers = allLayers();
  Strings regions = detRegions(); 
  Strings tVector; tVector.reserve(regions.size()*layers.size());
  for (Strings::iterator iterL = layers.begin(); iterL != layers.end(); ++iterL){
     for (Strings::iterator iterR = regions.begin(); iterR != regions.end(); ++iterR){
       std::string temp = (*iterL) + (*iterR) ;
       tVector.push_back(temp); 
     } // iterB
  } // iterS

  return tVector;
}

std::vector<std::string> LHCb::TTNames::allLayers() {

  typedef std::vector<std::string> Strings;
  Strings stationVec = stations(); 
   std::vector<std::string> layers = boost::assign::list_of(stationVec[0]+"X")(stationVec[0]+"U")
                                    (stationVec[1]+"V")(stationVec[1]+"X");
  return layers;
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
