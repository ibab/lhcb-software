#ifndef _TTLAYER_H
#define _TTLAYER_H 1

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
#include "STDet/STDetectionLayer.h"

class TTLayer: public STDetectionLayer{

public:

  //constructer
  TTLayer(int station, int layer, double z, double stereoAngle, 
             double pitch, double waferWidth, double waferHeight, 
 	     double waferThickness, double waferOverlap1,
             double waferOverlap2, unsigned int nOverlap, unsigned int nFine, 
             unsigned int wafersX1,unsigned int wafersX2,
             std::vector<int> ladderSize1,
             std::vector<int> ladderSize2,
             double holeX, double holeY, double ladderDist, 
             unsigned int numStrips, double vertGuardRing);

  // Destructor
  virtual ~TTLayer();

  virtual double centerX(const ITChannelID stripID) const;
  virtual double centerY(const ITChannelID stripID) const;
  virtual double centerZ(const ITChannelID stripID) const;
  virtual double halfLengthX(const ITChannelID stripID) const;
  virtual double halfLengthY(const ITChannelID stripID) const;
  virtual unsigned int numStrips() const;

  unsigned int nColumns() const;
  unsigned int columnID(const unsigned int iWafer) const;
  unsigned int numStripsInColumn(const unsigned int iColumn) const;
  unsigned int firstWaferInColumn(const unsigned int iColumn) const;
  unsigned int lastWaferInColumn(const unsigned int iColumn) const;

  unsigned int rowID(const unsigned int iWafer) const ;

 
private:

  std::vector<unsigned int> m_firstInColumn;
  int m_totStrips;

  unsigned int m_firstInnerColumn;  
  unsigned int m_lastInnerColumn;
  unsigned int m_outerHalfSize;
  unsigned int m_outerRowOffset;

};  

inline unsigned int TTLayer::nColumns() const{
  return m_firstInColumn.size();
}

#endif //_TTLAYER














