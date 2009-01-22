
#include "Kernel/STChannelID.h"
#include "Kernel/TTNames.h"

// BOOST
#include "boost/lexical_cast.hpp"
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

#include <iostream>

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


std::string LHCb::TTNames::channelToString(const LHCb::STChannelID& chan) {
  std::string theStrip = UniqueSectorToString(chan) + "Strip" + boost::lexical_cast<std::string>(chan.strip());
  return theStrip;
}  

LHCb::STChannelID LHCb::TTNames::stringToChannel(const std::string& name)  {

  // convert string to channel

  // get the station, layer and box
  const std::vector<std::string> thestations = stations(); 
  const unsigned int station = findStationType(name, thestations);
 
  const std::vector<std::string> thelayers = layers(); 
  unsigned int layer = 0;
  // station 1, layers U and X
  if (station == 1){
    if (name.find("X") != std::string::npos){
      layer = 1;
    }
    else if (name.find("U") != std::string::npos){
      layer = 2;
    }
    else {
      return STChannelID();
    }
  }

  if (station == 2){
    if (name.find("X") != std::string::npos){
      layer = 2;
    }
    else if (name.find("V") != std::string::npos){
      layer = 1;
    }
    else {
      return STChannelID();
    }
  }

  const std::vector<std::string> theregions = detRegions(); 
  const unsigned int region = findRegionType(name, theregions);
 
  // sector and strip is different
  unsigned int strip; unsigned int sector;
  std::string::size_type startSector = name.find("Sector");
  std::string::size_type startStrip = name.find("Strip");

  if (startSector == std::string::npos){
    sector = 0;
    strip = 0;
  }
  else if (startStrip == std::string::npos){
    strip = 0;
    std::string sectorName = name.substr(startSector+6);
    sector = toInt(sectorName);
    if (sector == 0) {
      return STChannelID(); // invalid sector
    }
  }
  else {
    std::string stripName = name.substr(startStrip+5); 
    strip = toInt(stripName);
    std::string sectorName = name.substr(startSector+6,startStrip - startSector - 6);
    sector = toInt(sectorName); 
  }

  return LHCb::STChannelID(LHCb::STChannelID::typeIT, station, layer,
                           region, sector, strip);
}

unsigned int LHCb::TTNames::findStationType(const std::string& testname, 
                                          const std::vector<std::string>& names) {

  std::vector<std::string>::const_iterator theName = names.begin();
  for ( ; theName != names.end(); ++theName ){
    if ( testname.find(*theName) != std::string::npos) {
      break;
    }
  } // iterS
  return theName == names.end()  ? 0 : (unsigned int)StationToType(*theName);
}

unsigned int LHCb::TTNames::findRegionType(const std::string& testname, 
                                          const std::vector<std::string>& names) {

  std::vector<std::string>::const_iterator theName = names.begin();
  for ( ; theName != names.end(); ++theName ){
    if ( testname.find(*theName) != std::string::npos) {
      break;
    }
  } // iterS
  return theName == names.end()  ? 0 : (unsigned int)detRegionToType(*theName);
}


unsigned int LHCb::TTNames::toInt(const std::string& str) {
  unsigned int outValue = 0;
  try {
    outValue = boost::lexical_cast<unsigned int>(str);
  }
  catch(boost::bad_lexical_cast& e){
    outValue = 0;
    std::cerr << "ERROR " << e.what() << "** " << str << " **" << std::endl; 
  }
  return outValue;
}
