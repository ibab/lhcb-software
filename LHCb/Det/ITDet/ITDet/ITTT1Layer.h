#ifndef _ITTT1LAYER_H
#define _ITTT1LAYER_H 1

//
// This File contains the declaration of the TrSicbOTDetectionLayer-class
// detection layer means information we can ask the detection layer 
// object to provide.......eg z position
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: Matthew Needham
//   Created: 10-05-1999

#include <vector>
#include "ITDet/ITDetectionLayer.h"

class ITTT1Layer: public ITDetectionLayer{

public:

  //constructer
  ITTT1Layer(int station, int layer, double z, double stereoAngle, 
                   double pitch, double waferWidth, double waferHeight, 
		   double waferThickness, double waferOverlap, 
             unsigned int wafersX1,unsigned int wafersX2,
             std::vector<int> ladderSize1,
             std::vector<int> ladderSize2,
             double holeX, double holeY, double ladderDist, 
                   unsigned int numStrips);

  // Destructor
  virtual ~ITTT1Layer();

  virtual double centerX(const ITChannelID stripID) const;
  virtual double centerY(const ITChannelID stripID) const;
  virtual double centerZ(const ITChannelID stripID) const;
  virtual double halfLengthX(const ITChannelID stripID) const;
  virtual double halfLengthY(const ITChannelID stripID) const;
  virtual unsigned int numStrips() const;
  
private:

  int m_totStrips;
  
};  


#endif //_ITTT1LAYER














