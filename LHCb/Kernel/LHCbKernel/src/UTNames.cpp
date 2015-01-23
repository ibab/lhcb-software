#include "Kernel/STChannelID.h"
#include "Kernel/UTNames.h"

#include <iostream>
#include <string>

std::string LHCb::UTNames::UniqueSectorToString(const LHCb::STChannelID& chan)
{
  return UniqueRegionToString(chan) + SectorToString(chan);
}

std::string LHCb::UTNames::SectorToString(const LHCb::STChannelID& chan)
{
  return "Sector"+ std::to_string(chan.sector());;
}

std::vector<std::string> LHCb::UTNames::stations()
{
  std::vector<std::string> stations; stations.reserve(s_StationTypMap().size());
  for (const auto& i : s_StationTypMap() ) {
    if (i.first != "Unknownstation") stations.emplace_back(i.first);
  }
  return stations;
}

std::vector<std::string> LHCb::UTNames::detRegions()
{
  std::vector<std::string> regions; regions.reserve(s_detRegionTypMap().size());
  for (const auto& i : s_detRegionTypMap() ) {
    if (i.first != "UnknownRegion") regions.emplace_back(i.first);
  }
  return regions;
}

const std::vector<std::string>& LHCb::UTNames::layers()
{
  //messy
  static const std::vector<std::string> layers = {"X", "U", "V"};
  return layers;
}

std::vector<std::string> LHCb::UTNames::allStations() {
  return stations();
}

std::vector<std::string> LHCb::UTNames::allDetRegions() {
  auto l = allLayers();
  auto r = detRegions();
  std::vector<std::string> tVector; tVector.reserve(r.size()*l.size());
  for (const auto& iL : l ) {
    for (const auto& iR : r ) {
      tVector.emplace_back( iL+iR );
    }
  }
  return tVector;
}

std::vector<std::string> LHCb::UTNames::allLayers() {

  auto stationVec = stations();
  return {stationVec[0]+"X", stationVec[0]+"U",
          stationVec[1]+"V", stationVec[1]+"X"};
}


std::string LHCb::UTNames::UniqueLayerToString(const LHCb::STChannelID& chan)
{
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

  return StationToString(chan) + layer;
}

std::string LHCb::UTNames::channelToString(const LHCb::STChannelID& chan)
{
  return UniqueSectorToString(chan) + "Strip" + std::to_string(chan.strip());
}

LHCb::STChannelID LHCb::UTNames::stringToChannel(const std::string& name)
{

  // convert string to channel

  // get the station, layer and box
  const std::vector<std::string> thestations = stations();
  const unsigned int station = findStationType(name, thestations);

  //  const std::vector<std::string> thelayers = layers();
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
      return STChannelID(LHCb::STChannelID::typeUT, station , 0u , 0u , 0u, 0u);
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
      return STChannelID(LHCb::STChannelID::typeUT, station , 0u , 0u , 0u, 0u);
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

  return LHCb::STChannelID(LHCb::STChannelID::typeUT, station, layer,
                           region, sector, strip);
}

unsigned int LHCb::UTNames::findStationType(const std::string& testname,
                                            const std::vector<std::string>& names)
{
  auto n = std::find_if( std::begin(names), std::end(names), 
                         [&](const std::string& s) {
      return testname.find(s) != std::string::npos;
  });
  return n != std::end(names) ?  (unsigned int)StationToType(*n) : 0 ;
}

unsigned int LHCb::UTNames::findRegionType(const std::string& testname,
                                           const std::vector<std::string>& names)
{
  auto n = std::find_if( std::begin(names), std::end(names), 
                         [&](const std::string& s) {
      return testname.find(s) != std::string::npos;
  });
  return n != names.end()  ? (unsigned int)detRegionToType(*n) : 0;
}


unsigned int LHCb::UTNames::toInt(const std::string& str)
{
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
