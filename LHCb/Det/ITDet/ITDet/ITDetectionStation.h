#ifndef _ITDETECTIONSTATION_
#define _ITDETECTIONSTATION_ 1

// Declaration of ITDetectionStation class
//   Inner tracker station description
//
//   Author: Anton Polouektov
//   Created: 11-5-2000
//   Last update: 08-10-2001
//   Version: V1.0

class ITDetectionLayer;

#include <vector>

class ITDetectionStation {

  public:

    /// Default constructor (DZ=0)
    ITDetectionStation() {}

    /// Standard constructor.
    ITDetectionStation(double z, double thickness, 
              unsigned int numLayers,  
	      unsigned int stationID);

    // destructer
    virtual ~ITDetectionStation();

    /// Set Z value
    void setZ(double z);
    
    /// Get Z value
    double z() const;

    /// Get a number of this station
    unsigned int stationID() const;

    bool isInside(const double z) const;

    /// Get number of detection layers
    unsigned int numLayers() const;

    /// Get pointer to N-th detection layer
    ITDetectionLayer* layer(const unsigned int n) const;

    /// Set pointer to N-th detection layer    
    void setLayer(ITDetectionLayer* layer, unsigned int layerNum);

    /// get pointer to N-th detection layer  
    ITDetectionLayer* layerByZ(const double testZ) const;

  private:

    double m_Z;
    double m_Thickness;
    unsigned int m_StationID;

    /// Array of layers.
    std::vector<ITDetectionLayer*> m_Layers;

};


inline void ITDetectionStation::setZ(double z) { 
  m_Z = z; 
}
    
inline double ITDetectionStation::z() const { 
 return m_Z; 
}

inline unsigned int ITDetectionStation::stationID() const { 
 return m_StationID; 
}

inline bool ITDetectionStation::isInside(const double z) const { 
  return (z < m_Z + 0.5*m_Thickness && z > m_Z - 0.5*m_Thickness); 
}

inline unsigned int ITDetectionStation::numLayers() const { 
  return m_Layers.size(); 
}

inline ITDetectionLayer* ITDetectionStation::layer(const unsigned int n) const {

 ITDetectionLayer* aLayer = 0;
 if (n>0 && n<=m_Layers.size()) {
   aLayer = m_Layers[n-1];
 }
   return aLayer;
}

inline void ITDetectionStation::setLayer(ITDetectionLayer* layer, 
                        unsigned int layerNum) {

 if (layerNum>0 && layerNum<=m_Layers.size()) {
   m_Layers[layerNum-1] = layer;
 } 

}


#endif






