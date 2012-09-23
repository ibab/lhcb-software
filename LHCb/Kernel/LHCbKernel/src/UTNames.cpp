
#include "Kernel/STChannelID.h"
#include "Kernel/UTNames.h"

// BOOST
#ifdef __INTEL_COMPILER       // Disable ICC remark from Boost
#pragma warning(disable:2259) // non-pointer conversion from "int" to "char" may lose significant bits
#endif
#include "boost/lexical_cast.hpp"
#include <boost/assign/std/vector.hpp>
#if !(defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L)
#include <boost/assign/list_of.hpp>
#endif

#include <iostream>

std::string LHCb::UTNames::UniqueSectorToString(const LHCb::STChannelID& chan)
{
  std::string theString = UniqueRegionToString(chan) + SectorToString(chan);
  return theString;
}

std::string LHCb::UTNames::SectorToString(const LHCb::STChannelID& chan)
{
  return "Sector"+ boost::lexical_cast<std::string>(chan.sector());;
}

std::vector<std::string> LHCb::UTNames::stations()
{
  GaudiUtils::VectorMap<std::string,Station>::iterator iter = s_StationTypMap().begin();
  std::vector<std::string> stations; stations.reserve(s_StationTypMap().size());
  for ( ;iter != s_StationTypMap().end(); ++iter ){
    if (iter->first != "Unknownstation") stations.push_back(iter->first);
  } // iter

  return stations;
}

std::vector<std::string> LHCb::UTNames::detRegions()
{
  GaudiUtils::VectorMap<std::string,detRegion>::iterator iter = s_detRegionTypMap().begin();
  std::vector<std::string> regions; regions.reserve(s_detRegionTypMap().size());
  for ( ;iter != s_detRegionTypMap().end(); ++iter )
  {
    if (iter->first != "UnknownRegion") regions.push_back(iter->first);
  } // iter
  return regions;
}

const std::vector<std::string>& LHCb::UTNames::layers()
{
  //messy
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
  static const std::vector<std::string> layers = {"X", "U", "V"};
#else
  static const std::vector<std::string> layers = boost::assign::list_of("X")("U")("V");
#endif
  return layers;
}

std::vector<std::string> LHCb::UTNames::allStations() {

  return stations();
}

std::vector<std::string> LHCb::UTNames::allDetRegions() {

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

std::vector<std::string> LHCb::UTNames::allLayers() {

  typedef std::vector<std::string> Strings;
  Strings stationVec = stations();
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
  std::vector<std::string> layers = {stationVec[0]+"X", stationVec[0]+"U",
                                     stationVec[1]+"V", stationVec[1]+"X"};
#else
  std::vector<std::string> layers = boost::assign::list_of(stationVec[0]+"X")(stationVec[0]+"U")
    (stationVec[1]+"V")(stationVec[1]+"X");
#endif
  return layers;
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

  std::string theString = StationToString(chan) + layer;

  return theString;
}

std::string LHCb::UTNames::channelToString(const LHCb::STChannelID& chan)
{
  std::string theStrip = UniqueSectorToString(chan) + "Strip" + boost::lexical_cast<std::string>(chan.strip());
  return theStrip;
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

  std::vector<std::string>::const_iterator theName = names.begin();
  for ( ; theName != names.end(); ++theName )
  {
    if ( testname.find(*theName) != std::string::npos)
    {
      break;
    }
  } // iterS
  return theName == names.end()  ? 0 : (unsigned int)StationToType(*theName);
}

unsigned int LHCb::UTNames::findRegionType(const std::string& testname,
                                           const std::vector<std::string>& names)
{
  std::vector<std::string>::const_iterator theName = names.begin();
  for ( ; theName != names.end(); ++theName ){
    if ( testname.find(*theName) != std::string::npos) {
      break;
    }
  } // iterS
  return theName == names.end()  ? 0 : (unsigned int)detRegionToType(*theName);
}


unsigned int LHCb::UTNames::toInt(const std::string& str)
{
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
