#ifndef _STDETECTIONSTATION_
#define _STDETECTIONSTATION_ 1

// Declaration of STDetectionStation class
//   Inner tracker station description
//
//   Author: Anton Polouektov
//   Created: 11-5-2000
//   Last update: 08-10-2001
//   Version: V1.0

class STDetectionLayer;


#include <iostream>
#include "GaudiKernel/MsgStream.h"
#include <vector>

class STDetectionStation {

  public:

    /// Default constructor (DZ=0)
    STDetectionStation() {}

    /// Standard constructor.
    STDetectionStation(double z, double thickness, 
              unsigned int numLayers,  
	      unsigned int stationID);

    // destructer
    virtual ~STDetectionStation();

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
    STDetectionLayer* layer(const unsigned int n) const;

    /// Set pointer to N-th detection layer    
    void setLayer(STDetectionLayer* layer, unsigned int layerNum);

    /// get pointer to N-th detection layer  
    STDetectionLayer* layerByZ(const double testZ) const;

    /// print 
    std::ostream& printOut( std::ostream& os ) const;
    MsgStream& printOut( MsgStream& os) const;

  private:

    double m_Z;
    double m_Thickness;
    unsigned int m_StationID;

    /// Array of layers.
    std::vector<STDetectionLayer*> m_Layers;

};


inline void STDetectionStation::setZ(double z) { 
  m_Z = z; 
}
    
inline double STDetectionStation::z() const { 
 return m_Z; 
}

inline unsigned int STDetectionStation::stationID() const { 
 return m_StationID; 
}

inline bool STDetectionStation::isInside(const double z) const { 
  return (z < m_Z + 0.5*m_Thickness && z > m_Z - 0.5*m_Thickness); 
}

inline unsigned int STDetectionStation::numLayers() const { 
  return m_Layers.size(); 
}

inline STDetectionLayer* STDetectionStation::layer(const unsigned int n) const {

 STDetectionLayer* aLayer = 0;
 if (n>0 && n<=m_Layers.size()) {
   aLayer = m_Layers[n-1];
 }
 return aLayer;
}

inline void STDetectionStation::setLayer(STDetectionLayer* layer, 
                        unsigned int layerNum) {

 if (layerNum>0 && layerNum<=m_Layers.size()) {
   m_Layers[layerNum-1] = layer;
 } 

}

inline std::ostream& operator<<( std::ostream& os , const STDetectionStation* aStation )
{ return aStation->printOut( os ); } 

inline MsgStream& operator<<( MsgStream& os , const STDetectionStation* aStation )
{ return aStation->printOut( os ); } 

#endif






