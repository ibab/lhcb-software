
#include "ITDet/ITDetectionStation.h"
#include "ITDet/ITDetectionLayer.h"

ITDetectionStation::ITDetectionStation(double z, double thickness, 
                     unsigned int numLayers, 
		     unsigned int stationID):
  m_Z(z),
  m_Thickness(thickness),
  m_StationID(stationID)
 {
  // constructer
  m_Layers.resize(numLayers);
}

ITDetectionStation::~ITDetectionStation(){
   // Delete stations
  std::vector<ITDetectionLayer*>::iterator first = m_Layers.begin();
  while (first != m_Layers.end()) {
    ITDetectionLayer* aLayer = *first;
    delete aLayer;
    first = m_Layers.erase(first);
  } // loop stations
}

ITDetectionLayer* ITDetectionStation::layerByZ(const double testZ) const {

  ITDetectionLayer* aLayer = 0;
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



std::ostream& ITDetectionStation::printOut( std::ostream& os ) const{
 
  os << " Station : "  << m_StationID << std::endl;
  os << "z " << m_Z  
     << " n layer "  << numLayers() 
     << " Thickness "<< m_Thickness << std::endl;
   
  return os;
}


MsgStream& ITDetectionStation::printOut( MsgStream& os ) const{

  os << " Station : "  << m_StationID << endreq;
  os << "z " << m_Z  
     << " n layer "  << numLayers() 
     << " Thickness "<< m_Thickness << endreq;

  return os;
}









