#ifndef _ITSTLAYER_H
#define _ITSTLAYER_H 1

//
// This File contains the declaration of the TrSicbOTDetectionLayer-class
// detection layer means information we can ask the detection layer 
// object to provide.......eg z position
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: Matthew Needham
//   Created: 10-05-1999

#include "ITDet/ITDetectionLayer.h"

class ITSTLayer: public ITDetectionLayer{

public:

  //constructer
  ITSTLayer(int station, int layer, double z, double stereoAngle, 
                   double pitch, double waferWidth, double waferHeight, 
		   double waferThickness, double waferOverlap, 
		   unsigned int wafersX, unsigned int wafersY, double holeX, 
		   double holeY, double ladderDist, unsigned int numStrips, 
		   int position);

  // Destructor
  virtual ~ITSTLayer();

  virtual double centerX(const ITChannelID stripID) const;
  virtual double centerY(const ITChannelID stripID) const;
  virtual double centerZ(const ITChannelID stripID) const;
  virtual double halfLengthX(const ITChannelID stripID) const;
  virtual double halfLengthY(const ITChannelID stripID) const;
  virtual unsigned int numStrips() const;
  
private:
  
};  


#endif //_ITSTLAYER














