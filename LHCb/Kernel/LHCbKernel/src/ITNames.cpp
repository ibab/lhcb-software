#include "Kernel/STChannelID.h"
#include "Kernel/ITNames.h"
#include "boost/lexical_cast.hpp"
#include <iostream>

std::string LHCb::ITNames::UniqueSectorToString(const LHCb::STChannelID& chan) {
  std::string theString = UniqueLayerToString(chan) + "Sector" + boost::lexical_cast<std::string>(chan.sector());
  return theString;
}

std::string LHCb::ITNames::channelToString(const LHCb::STChannelID& chan) {
  std::string theStrip = UniqueSectorToString(chan) + "Strip" + boost::lexical_cast<std::string>(chan.strip());
  return theStrip;
}  

std::vector<std::string> LHCb::ITNames::stations() {

  GaudiUtils::VectorMap<std::string,Station>::iterator iter = s_StationTypMap().begin();
  std::vector<std::string> stations; stations.reserve(s_StationTypMap().size());
  for ( ;iter != s_StationTypMap().end(); ++iter ){
    if (iter->first != "UnknownStation" ) stations.push_back(iter->first);
  } // iter

  return stations;
}


std::vector<std::string> LHCb::ITNames::allStations() {

  return stations();
}


std::vector<std::string> LHCb::ITNames::allBoxes() {

  typedef std::vector<std::string> Strings;
  Strings stationVec = stations();
  Strings boxVec = boxes(); 
  Strings tVector; tVector.reserve(boxVec.size()*stationVec.size());
  for (Strings::iterator iterS = stationVec.begin(); iterS != stationVec.end(); ++iterS){
     for (Strings::iterator iterB = boxVec.begin(); iterB != boxVec.end(); ++iterB){
       std::string temp = (*iterS) + (*iterB) ;
       tVector.push_back(temp); 
     } // iterB
  } // iterS

  return tVector;
}

std::vector<std::string> LHCb::ITNames::allLayers() {

  typedef std::vector<std::string> Strings;
  Strings layerVec = layers();
  Strings boxVec = allBoxes(); 
  Strings tVector; tVector.reserve(boxVec.size()*layerVec.size());
  for (Strings::iterator iterB = boxVec.begin(); iterB != boxVec.end(); ++iterB){
     for (Strings::iterator iterL = layerVec.begin(); iterL != layerVec.end(); ++iterL){
       std::string temp = (*iterB) + (*iterL) ;
       tVector.push_back(temp); 
     } // iterB
  } // iterS

  return tVector;
}


std::vector<std::string> LHCb::ITNames::boxes() {

  GaudiUtils::VectorMap<std::string,Box>::iterator iter = s_BoxTypMap().begin();
  std::vector<std::string> boxes; boxes.reserve(s_BoxTypMap().size());
  for ( ;iter != s_BoxTypMap().end(); ++iter ){
    if (iter->first != "UnknownBox") boxes.push_back(iter->first);
  } // iter

  return boxes;
}

std::vector<std::string> LHCb::ITNames::layers() {

  GaudiUtils::VectorMap<std::string,Layer>::iterator iter = s_LayerTypMap().begin();
  std::vector<std::string> layers; layers.reserve(s_LayerTypMap().size());
  for ( ;iter != s_LayerTypMap().end(); ++iter ){
    if (iter->first != "UnknownLayer") layers.push_back(iter->first);
  } // iter

  return layers;
}

LHCb::STChannelID LHCb::ITNames::stringToChannel(const std::string& name)  {

  // convert string to channel

  // get the station, layer and box
  const std::vector<std::string> thestations = stations(); 
  const unsigned int station = findStationType(name, thestations);
 
  const std::vector<std::string> thelayers = layers(); 
  const unsigned int layer = findLayerType(name, thelayers );

  const std::vector<std::string> theboxes = boxes(); 
  const unsigned int box = findBoxType(name, theboxes);
 
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
                           box, sector, strip);
}

unsigned int LHCb::ITNames::findStationType(const std::string& testname, 
                                          const std::vector<std::string>& names) {

  std::vector<std::string>::const_iterator theName = names.begin();
  for ( ; theName != names.end(); ++theName ){
    if ( testname.find(*theName) != std::string::npos) {
      break;
    }
  } // iterS
  return theName == names.end()  ? 0 : (unsigned int)StationToType(*theName);
}

unsigned int LHCb::ITNames::findLayerType(const std::string& testname, 
                                          const std::vector<std::string>& names) {

  std::vector<std::string>::const_iterator theName = names.begin();
  for ( ; theName != names.end(); ++theName ){
    if ( testname.find(*theName) != std::string::npos) {
      break;
    }
  } // iterS
  return theName == names.end()  ? 0 : (unsigned int)LayerToType(*theName);
}

unsigned int LHCb::ITNames::findBoxType(const std::string& testname, 
                                          const std::vector<std::string>& names) {

  std::vector<std::string>::const_iterator theName = names.begin();
  for ( ; theName != names.end(); ++theName ){
    if ( testname.find(*theName) != std::string::npos) {
      break;
    }
  } // iterS
  return theName == names.end()  ? 0 : (unsigned int)BoxToType(*theName);
}


unsigned int LHCb::ITNames::toInt(const std::string& str) {
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
