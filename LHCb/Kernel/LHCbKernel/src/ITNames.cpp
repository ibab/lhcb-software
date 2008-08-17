#include "Kernel/STChannelID.h"
#include "Kernel/ITNames.h"
#include "boost/lexical_cast.hpp"

std::string LHCb::ITNames::UniqueSectorToString(const LHCb::STChannelID& chan) {
  std::string theString = UniqueLayerToString(chan) + "Sector" + boost::lexical_cast<std::string>(chan.sector());
  return theString;
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





