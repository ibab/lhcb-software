#ifndef _ITDETECTIONLAYER_H
#define _ITDETECTIONLAYER_H 1

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

class ITWafer;
class ITChannelID;

#define SIDE_POS 1
#define CENTER_POS 2
#define CENTER_POS1 3
#define CENTER_POS2 4

#define SAFETY_BOUND 0.1

class ITDetectionLayer{

public:

  virtual ~ITDetectionLayer();

  ITDetectionLayer(int station, int layer, double z, double stereoAngle,
                    double pitch, double waferThickness, double ladderDist);

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
  ITWafer* wafer(const unsigned int waferID) const;
  double waferThickness() const;

  ITChannelID nextRight(const ITChannelID testChan) const;
  ITChannelID nextLeft(const ITChannelID testChan) const;

  std::string type() const;

  /// print 
  std::ostream& printOut( std::ostream& os ) const;
  MsgStream& printOut( MsgStream& os) const;

protected:
 
  std::vector<ITWafer*> m_Wafers;
  std::string m_type;
  unsigned int m_WafersNum;

private:
  
  unsigned int m_StationID;
  unsigned int m_LayerID;
  double m_Z;
  double m_StereoAngle;
  double m_Cos;
  double m_Sin;
  double m_Thickness;
  double m_WaferThickness;
  double m_Pitch;
  

};  

inline unsigned int ITDetectionLayer::layerID() const{
  // get layer ID (relative to station number)
  return m_LayerID;
}

inline unsigned int ITDetectionLayer::absLayerID() const{
  // get absolute layer number
  return m_StationID*100 + m_LayerID;
}

inline unsigned int ITDetectionLayer::stationID() const{
  // get layer ID (relative to station number)
  return m_StationID;
}

inline double ITDetectionLayer::z() const{
  //layer z position
  return m_Z;
}

inline double ITDetectionLayer::stereoAngle() const{
  //layer z position
  return m_StereoAngle;
}

inline double ITDetectionLayer::cosAngle() const{
  //layer z position
  return m_Cos;
}
 
inline double ITDetectionLayer::sinAngle() const{
  //layer z position
  return m_Sin;
}

inline bool ITDetectionLayer::isInside(const double z) const { 
  return (fabs(z - m_Z) < (0.5*m_Thickness + SAFETY_BOUND)); 
}

inline double ITDetectionLayer::pitch() const { 
  return m_Pitch; 
}

inline unsigned int ITDetectionLayer::numWafers() const { 
  return m_Wafers.size(); 
}

inline double ITDetectionLayer::waferThickness() const { 
  return m_WaferThickness; 
}

inline std::string ITDetectionLayer::type() const {
  return m_type;
}

inline std::ostream& operator<<( std::ostream& os , const ITDetectionLayer* aLayer )
{ return aLayer->printOut( os ); } 

inline MsgStream& operator<<( MsgStream& os , const ITDetectionLayer* aLayer )
{ return aLayer->printOut( os ); } 

#endif //_ITDETECTIONLAYER














