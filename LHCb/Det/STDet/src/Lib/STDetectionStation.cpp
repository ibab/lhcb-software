
#include "STDet/STDetectionStation.h"
#include "STDet/STDetectionLayer.h"

STDetectionStation::STDetectionStation(double z, double thickness, 
                     unsigned int numLayers, 
		     unsigned int stationID):
  m_Z(z),
  m_Thickness(thickness),
  m_StationID(stationID)
 {
  // constructer
  m_Layers.reserve(numLayers);
  for (unsigned int iLayer = 0; iLayer < numLayers; ++iLayer){
    m_Layers.push_back(0);
  } // iLayer
}

STDetectionStation::~STDetectionStation(){
   // Delete stations
  std::vector<STDetectionLayer*>::iterator first = m_Layers.begin();
  while (first != m_Layers.end()) {
    STDetectionLayer* aLayer = *first;
    delete aLayer;
    first = m_Layers.erase(first);
  } // loop stations
}

STDetectionLayer* STDetectionStation::layerByZ(const double testZ) const {

  STDetectionLayer* aLayer = 0;
  bool isIn = false;
  unsigned int iLayer = 1;
  while ((iLayer <= numLayers())
	 &&(isIn==false)){
    aLayer = layer(iLayer);  
    if (0 != aLayer){
      isIn = aLayer->isInside(testZ);
    }

    iLayer++;
  } // iLayer

  return aLayer;
}

std::ostream& STDetectionStation::printOut( std::ostream& os ) const{
 
  os << " Station : "  << m_StationID << std::endl;
  os << "z " << m_Z  
     << " n layer "  << numLayers() 
     << " Thickness "<< m_Thickness << std::endl;
   
  return os;
}

MsgStream& STDetectionStation::printOut( MsgStream& os ) const{

  os << " Station : "  << m_StationID << endreq;
  os << "z " << m_Z  
     << " n layer "  << numLayers() 
     << " Thickness "<< m_Thickness << endreq;

  return os;
}









