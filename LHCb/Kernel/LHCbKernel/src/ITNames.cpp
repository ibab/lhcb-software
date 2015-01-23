#include "Kernel/STChannelID.h"
#include "Kernel/ITNames.h"


#include <iostream>
#include <string>

std::string LHCb::ITNames::UniqueSectorToString(const LHCb::STChannelID& chan) {
  return UniqueLayerToString(chan) + SectorToString(chan);
}

std::string LHCb::ITNames::SectorToString(const LHCb::STChannelID& chan) {
  return "Sector"+ std::to_string(chan.sector());
}

std::string LHCb::ITNames::channelToString(const LHCb::STChannelID& chan) {
  return UniqueSectorToString(chan) + "Strip" + std::to_string(chan.strip());
}  

std::vector<std::string> LHCb::ITNames::stations() {

  std::vector<std::string> stations; stations.reserve(s_StationTypMap().size());
  for (const auto& i : s_StationTypMap() ) {
    if (i.first != "UnknownStation" ) stations.emplace_back(i.first);
  }
  return stations;
}


std::vector<std::string> LHCb::ITNames::allStations() {

  return stations();
}


std::vector<std::string> LHCb::ITNames::allBoxes() {

  auto stationVec = stations();
  auto boxVec = boxes(); 
  std::vector<std::string> tVector; tVector.reserve(boxVec.size()*stationVec.size());
  for (const auto& iS : stationVec ) {
     for (const auto& iB : boxVec ) {
       tVector.emplace_back( iS + iB) ;
     } 
  }
  return tVector;
}

std::vector<std::string> LHCb::ITNames::allLayers() {

  auto layerVec = layers();
  auto boxVec = allBoxes(); 
  std::vector<std::string> tVector; tVector.reserve(boxVec.size()*layerVec.size());
  for (const auto& iB : boxVec ) {
     for (const auto& iL : layerVec ) {
       tVector.push_back(iB + iL) ;
     }
  }
  return tVector;
}


std::vector<std::string> LHCb::ITNames::boxes() {

  std::vector<std::string> boxes; boxes.reserve(s_BoxTypMap().size());
  for (const auto& i : s_BoxTypMap() ) {
    if (i.first != "UnknownBox") boxes.emplace_back(i.first);
  }
  return boxes;
}

std::vector<std::string> LHCb::ITNames::layers() {
  std::vector<std::string> layers; layers.reserve(s_LayerTypMap().size());
  for (const auto& i : s_LayerTypMap() ) {
    if (i.first != "UnknownLayer") layers.emplace_back(i.first);
  }
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

  auto n = std::find_if( std::begin(names), std::end(names), 
                         [&](const std::string& s ) {
                             return testname.find(s) != std::string::npos;
  });
  return n != std::end(names)  ?  (unsigned int)StationToType(*n) : 0 ;
}

unsigned int LHCb::ITNames::findLayerType(const std::string& testname, 
                                          const std::vector<std::string>& names) {
  auto n = std::find_if( std::begin(names), std::end(names), 
                         [&](const std::string& s ) {
                             return testname.find(s) != std::string::npos;
  });
  return n != std::end(names)  ?  (unsigned int)LayerToType(*n) : 0 ;
}

unsigned int LHCb::ITNames::findBoxType(const std::string& testname, 
                                          const std::vector<std::string>& names) {

  auto n = std::find_if( std::begin(names), std::end(names), 
                         [&](const std::string& s ) {
                             return testname.find(s) != std::string::npos;
  });
  return n != std::end(names)  ?  (unsigned int)BoxToType(*n) : 0 ;
}


unsigned int LHCb::ITNames::toInt(const std::string& str) {
  unsigned int outValue = 0;
  try {
    outValue = std::stoul(str);
  }
  catch(std::invalid_argument& e){
    outValue = 0;
    std::cerr << "ERROR " << e.what() << "** " << str << " **" << std::endl; 
  }
  return outValue;
}
