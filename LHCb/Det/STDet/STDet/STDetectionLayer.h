#ifndef _STDETECTIONLAYER_H
#define _STDETECTIONLAYER_H 1

#include <iostream>
#include "GaudiKernel/MsgStream.h"

//
// This File contains the declaration of the TrSicbOTDetectionLayer-class
// detection layer means information we can ask the detection layer 
// object to provide.......eg z position
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: Matthew Needham
//   Created: 10-05-1999

#include<cmath>
#include<vector>
#include <string>

class STWafer;
class ITChannelID;

#define SIDE_POS 1
#define CENTER_POS 2
#define CENTER_POS1 3
#define CENTER_POS2 4

#define SAFETY_BOUND 0.1

class STDetectionLayer{

public:

  virtual ~STDetectionLayer();

  STDetectionLayer(int station, int layer, double z, double stereoAngle,
                    double pitch, double waferThickness, double zStag);

  // Getters & setters
  unsigned int    absLayerID() const ;
  unsigned int    layerID() const ;
  unsigned int    stationID() const ;
  double z() const;
  double stereoAngle() const;
  double cosAngle() const;
  double sinAngle() const;  

  bool isInside(const double z) const;
  bool isInside(const double x, const double y) const;
  bool isInside(const double x, const double y, const double tol) const;
  bool isInside(const double x, const double y, const ITChannelID aChan, const double tol) const;

  void toUV(const double x, const double y,
            double& u, double& v) const;

  void toXY(const double u, const double v,
            double& u, double& v) const;

  virtual ITChannelID stripID(const double u, const double v) const;
  virtual double U(const ITChannelID stripID) const;
  virtual double centerX(const ITChannelID stripID) const = 0;
  virtual double centerY(const ITChannelID stripID) const = 0;
  virtual double centerZ(const ITChannelID stripID) const = 0;
  virtual double halfLengthX(const ITChannelID stripID) const = 0;
  virtual double halfLengthY(const ITChannelID stripID) const = 0;
  virtual unsigned int numStrips() const = 0;
  double pitch() const;
  unsigned int numWafers() const;
  STWafer* wafer(const unsigned int waferID) const;
  double waferThickness() const;

  ITChannelID nextRight(const ITChannelID testChan) const;
  ITChannelID nextLeft(const ITChannelID testChan) const;

  std::string type() const;
  double stagger() const;

  /// print 
  std::ostream& printOut( std::ostream& os ) const;
  MsgStream& printOut( MsgStream& os) const;

protected:
 
  std::vector<STWafer*> m_Wafers;
  std::string m_type;
  unsigned int m_WafersNum;

private:
  
  unsigned int m_StationID;
  unsigned int m_LayerID;
  double m_Z;
  double m_StereoAngle;
  double m_Cos;
  double m_Sin;
  double m_WaferThickness;
  double m_Pitch;
  double m_zStagger;  

};  

inline unsigned int STDetectionLayer::layerID() const{
  // get layer ID (relative to station number)
  return m_LayerID;
}

inline unsigned int STDetectionLayer::absLayerID() const{
  // get absolute layer number
  return m_StationID*100 + m_LayerID;
}

inline unsigned int STDetectionLayer::stationID() const{
  // get layer ID (relative to station number)
  return m_StationID;
}

inline double STDetectionLayer::z() const{
  //layer z position
  return m_Z;
}

inline double STDetectionLayer::stagger() const{
  //layer z position
  return m_zStagger;
}

inline double STDetectionLayer::stereoAngle() const{
  //layer z position
  return m_StereoAngle;
}

inline double STDetectionLayer::cosAngle() const{
  //layer z position
  return m_Cos;
}
 
inline double STDetectionLayer::sinAngle() const{
  //layer z position
  return m_Sin;
}

inline void STDetectionLayer::toUV(const double x, const double y,
		 double& u, double& v) const{

  u = x*m_Cos + y*m_Sin;
  v = y*m_Cos - x*m_Sin;
}

inline void STDetectionLayer::toXY(const double u, const double v,
		 double& x, double& y) const{

  y = u*m_Sin + v*m_Cos;
  x = u*m_Cos - v*m_Sin;
}


inline bool STDetectionLayer::isInside(const double z) const { 
  return (fabs(z - m_Z) < (0.5*(m_WaferThickness + m_zStagger) + SAFETY_BOUND)); 
}

inline double STDetectionLayer::pitch() const { 
  return m_Pitch; 
}

inline unsigned int STDetectionLayer::numWafers() const { 
  return m_WafersNum; 
}

inline double STDetectionLayer::waferThickness() const { 
  return m_WaferThickness; 
}

inline std::string STDetectionLayer::type() const {
  return m_type;
}

inline std::ostream& operator<<( std::ostream& os , const STDetectionLayer* aLayer )
{ return aLayer->printOut( os ); } 

inline MsgStream& operator<<( MsgStream& os , const STDetectionLayer* aLayer )
{ return aLayer->printOut( os ); } 

#endif //_STDETECTIONLAYER














