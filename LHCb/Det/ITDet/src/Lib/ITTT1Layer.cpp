// $
//
// This File contains the definition of the ITSTLayer-class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: Matthew Needham
//   Created: 10-05-1999

//_________________________________________________
// ITSTLayer
// ST layer properties 

#include "ITDet/ITTT1Layer.h"
#include "Kernel/ITChannelID.h"
#include "ITDet/ITWafer.h"

#include <iterator>


ITTT1Layer::ITTT1Layer(int stationID, int layerID, double z, 
                    double stereoAngle, double pitch, double waferWidth, 
		    double waferHeight, double waferThickness,
		    double waferOverlap, unsigned int wafersX1,
		    unsigned int wafersX2,
                    std::vector<int> ladderSize1,
                    std::vector<int> ladderSize2,
                    double holeX, double holeY, 
		    double ladderDist,
		    unsigned int numStrips):
  ITDetectionLayer(stationID, layerID, z, 
                   stereoAngle, pitch,waferThickness,waferOverlap)
{
  // constructer


  // total number of wafers
  m_WafersNum = 2u*((ladderSize1.size()*wafersX1)+(ladderSize2.size()*wafersX2));
  m_Wafers.resize(m_WafersNum);
  
  // sensitive area of wafer
  double sensWaferWidth = pitch*(double)numStrips;
  double guardRingSize = 0.5*(waferWidth -sensWaferWidth);
 
  unsigned int nSensorHigh=0;
  // max dimensions
  for (unsigned int iLad1=0; iLad1<ladderSize1.size();iLad1++){
    nSensorHigh += ladderSize1[iLad1]; 
  } // iLad

  // layout top and bottom boxes...
  double yLad = holeY + 0.5*((nSensorHigh*waferHeight*cosAngle())
                             + (waferWidth*fabs(sinAngle())));
  double xLad = -yLad*sinAngle()/cosAngle();
  // go to u,v
  double uLad = xLad*cosAngle() + yLad*sinAngle();
  double vLad = yLad*cosAngle() - xLad*sinAngle();
  double vMax = vLad + (0.5*(double)nSensorHigh*waferHeight);

  // bottom box
  int currStrip = 0;
  double v = -vMax;
  unsigned int startWafer = wafersX2;
  for (unsigned int iBWafer=1; iBWafer<=ladderSize1.size();iBWafer++){

    unsigned int waferOffset = startWafer+(iBWafer-1)*((2*wafersX2)+wafersX1);
    v += (waferHeight*(double)ladderSize1[iBWafer-1])/2.0; 
    double ladderHeight = (ladderSize1[iBWafer-1]*waferHeight)
                          -2.0*guardRingSize;

    for (unsigned int iWafer=1;iWafer<=wafersX1;iWafer++){
      double dz = -(1.-(2.*(iWafer%2)))*0.5*ladderDist;
      ITWafer* aWafer = new ITWafer(pitch, 1,ladderSize1[iBWafer-1],
				    stationID,layerID,waferOffset+iWafer,
                                    uLad-0.5*sensWaferWidth, uLad+0.5*sensWaferWidth, 
	   		            v - 0.5*ladderHeight, 
		 	            v + 0.5*ladderHeight, 
                                    dz,
                                    guardRingSize);

       m_Wafers[waferOffset+iWafer-1] = aWafer;
       currStrip += aWafer->lastStrip();

    }  // iWafer
    v+= (waferHeight*(double)ladderSize1[iBWafer-1])/2.0;

  } // iBWafer

  // top box
  // wafer number offset
  startWafer = wafersX2+((1+(ladderSize2.size()/2))*wafersX2*2)
               + (wafersX1*ladderSize1.size());

  v = vMax - ((double)nSensorHigh*waferHeight);
  for (unsigned int iTWafer=1; iTWafer<=ladderSize1.size();iTWafer++){

    unsigned waferOffset = startWafer+(iTWafer-1)*((2*wafersX2)+wafersX1);
    
    v += (waferHeight*(double)ladderSize1[ladderSize1.size()-iTWafer])/2.0;
    double ladderHeight = ladderSize1[ladderSize1.size()-iTWafer]*waferHeight
                          -2.0*guardRingSize;

    for (unsigned int iWafer=1;iWafer<=wafersX1;iWafer++){
      double dz = -(1.-(2.*(iWafer%2)))*0.5*ladderDist;
      ITWafer* aWafer = new ITWafer(pitch, 1,ladderSize1[ladderSize1.size()-iTWafer],
				    stationID,layerID,waferOffset+iWafer,
                        uLad-0.5*sensWaferWidth,uLad+0.5*sensWaferWidth, 
			v - 0.5*ladderHeight, 
			v + 0.5*ladderHeight,
                        dz,
                        guardRingSize);

       m_Wafers[waferOffset+iWafer-1] = aWafer;
       currStrip += aWafer->lastStrip();

    } //iWafer

    v += (waferHeight*(double)ladderSize1[ladderSize1.size()-iTWafer])/2.0;

  } // iTWafer

  // recalculate vmax
  // max dimensions
  nSensorHigh = 0;
  unsigned int iLad2=0;
  for (iLad2=0; iLad2<ladderSize2.size();iLad2++){
    nSensorHigh += ladderSize2[iLad2]; 
  } // iLad

  // cache the x centers of ladders
  std::vector<double> xSideLad; 
  for (iLad2=1;iLad2<=wafersX2;iLad2++){
    xSideLad.push_back(holeX +
     ((double)(wafersX2-iLad2)*(waferWidth-waferOverlap)/cosAngle())+
     (0.5*waferWidth*cosAngle()));
  } //

  startWafer = 0;
  double ySideLad = 0.;
  // left box
  for (unsigned int iLWafer=1;iLWafer<=wafersX2;iLWafer++){

    double dz = (1.-(2.*(iLWafer%2)))*0.5*ladderDist;
    double xLad = -xSideLad[iLWafer-1]; 
    double uLad = xLad*cosAngle() + ySideLad*sinAngle();
    double vLad = ySideLad*cosAngle() - xLad*sinAngle();
    double v = vLad-((double)(nSensorHigh)/2.0)*waferHeight;

    for (unsigned int iWafer=1; iWafer<=ladderSize2.size();iWafer++){

      // wafer center in V
      v += 0.5*(waferHeight*(double)ladderSize2[iWafer-1]); 

      // ladder height
      double ladderHeight = waferHeight*(double)ladderSize2[iWafer-1]
	-2.*guardRingSize; 
               
      // wafer offset
      unsigned int waferOffset;
      if (iWafer<=(1+(ladderSize2.size()/2))){
        waferOffset = startWafer+((iWafer-1)*((2*wafersX2)+wafersX1));
      }
      else {
       waferOffset = startWafer+((iWafer-1)*((2*wafersX2)+wafersX1))-1;
      }

      ITWafer* aWafer = new ITWafer(pitch, 1, ladderSize2[iWafer-1],  
				    stationID,layerID,iLWafer+waferOffset,
                        uLad-0.5*sensWaferWidth,uLad+0.5*sensWaferWidth, 
			v - 0.5*ladderHeight, 
		        v + 0.5*ladderHeight, 
                        dz,
                        guardRingSize);
      m_Wafers[iLWafer+waferOffset-1] = aWafer;
      currStrip += aWafer->lastStrip();

      v += 0.5*(waferHeight*(double)ladderSize2[iWafer-1]); 

    } //iWafer
  } // iLWafer
    
  // right box 
  startWafer = wafersX1+wafersX2;
  for (unsigned int iRWafer=1;iRWafer<=wafersX2;iRWafer++){

    double dz = (1.-(2.*(iRWafer%2)))*0.5*ladderDist;
    double xLad = xSideLad[wafersX2-iRWafer];
    double uLad = xLad*cosAngle() + ySideLad*sinAngle();
    double vLad = ySideLad*cosAngle() - xLad*sinAngle();
    double v = vLad-((double)(nSensorHigh)/2.0)*waferHeight;

    for (unsigned int iWafer=1; iWafer<=ladderSize2.size();iWafer++){

      // wafer center in V
      v += 0.5*(waferHeight*(double)ladderSize2[iWafer-1]); 
      
      // ladder height
      double ladderHeight = waferHeight*(double)ladderSize2[iWafer-1]
	-2.*guardRingSize; 
       
      // wafer offset
      unsigned int waferOffset;
      if (iWafer<=ladderSize2.size()/2){
        waferOffset = startWafer+((iWafer-1)*((2*wafersX2)+wafersX1));
      }
      else {
       waferOffset = startWafer+((iWafer-1)*((2*wafersX2)+wafersX1))-1;
      }

      ITWafer* aWafer = new ITWafer(pitch, 1, ladderSize2[iWafer-1],  
				    stationID,layerID,iRWafer+waferOffset,
                        uLad-0.5*sensWaferWidth,uLad+0.5*sensWaferWidth, 
			v - 0.5*ladderHeight, 
		        v + 0.5*ladderHeight, 
                        dz,
                        guardRingSize);
      m_Wafers[iRWafer+waferOffset-1] = aWafer;
      currStrip += aWafer->lastStrip();
      v += 0.5*(waferHeight*(double)ladderSize2[iWafer-1]); 
    } //iWafer
  } // iLWafer

  m_totStrips = currStrip;

  // rows stuff
  m_rowsVector.reserve(ladderSize2.size());
  unsigned int lastWaferInRow;
  for (unsigned int iRow = 1; iRow<= ladderSize2.size(); iRow++){
    if (iRow<= ladderSize2.size()/2){
      lastWaferInRow = iRow*((2*wafersX2)+wafersX1);
    }
    else {
      lastWaferInRow = iRow*((2*wafersX2)+wafersX1) -1;
    }
    m_rowsVector.push_back(lastWaferInRow);
  } //iRow 
}

ITTT1Layer::~ITTT1Layer(){
  //des
  if (m_Wafers.size() != 0) {
    std::vector<ITWafer*>::iterator iWafer = m_Wafers.begin();
    while (iWafer != m_Wafers.end()) {
      ITWafer* aWafer = *iWafer;
      delete aWafer;
      m_Wafers.erase(iWafer);
    }
  }
}

double ITTT1Layer::centerX(const ITChannelID stripID) const {
  double xC = 0.;
  const unsigned int waferNum = stripID.wafer();
  if (waferNum>=1 && waferNum <= m_Wafers.size()) {
    const double u = m_Wafers[waferNum-1]->U(stripID.strip());
    const double v = m_Wafers[waferNum-1]->centerV();
    xC = u*cosAngle() - v*sinAngle();
  }

  return xC;
}

double ITTT1Layer::centerY(const ITChannelID stripID) const {

  double yC = 0.;
  const unsigned int waferNum = stripID.wafer();
  if (waferNum>=1 && waferNum <= m_Wafers.size()) {
    const double u = m_Wafers[waferNum-1]->U(stripID.strip());
    const double v = m_Wafers[waferNum-1]->centerV();
    yC = v*cosAngle() + u*sinAngle();
  }

  return yC;
}

double ITTT1Layer::centerZ(const ITChannelID stripID) const {

  // z center of channel
  double zC =0.;
  const unsigned int waferNum = stripID.wafer();
  if (waferNum>=1 && waferNum <= m_WafersNum) {
    zC = this->z() + m_Wafers[waferNum-1]->dZ();
  }

  return zC;
}

double ITTT1Layer::halfLengthX(const ITChannelID stripID) const {
  return -this->halfLengthY(stripID)*sinAngle()/cosAngle(); 
}

double ITTT1Layer::halfLengthY(const ITChannelID stripID) const {

  double halfLengthY = 0;
  const unsigned int waferNum = stripID.wafer();
  if (waferNum>=1 && waferNum <= m_WafersNum) {
    halfLengthY = m_Wafers[waferNum-1]->halfLengthV()*cosAngle();
  }

  return halfLengthY;
}

unsigned int ITTT1Layer::numStrips() const {
  return m_totStrips;
}

unsigned int ITTT1Layer::rowID(const unsigned int iWafer) const {

  // return row number
  unsigned int iRow = 0u;

  if (iWafer>=1 && iWafer <= m_Wafers.size()){

    std::vector<unsigned int>::const_iterator iter = m_rowsVector.begin();
    while ((iter != m_rowsVector.end())&&(iWafer>*iter)){
      ++iter;
    } 
  
    std::vector<double>::difference_type n = 0;
    n = std::distance(m_rowsVector.begin(),iter);
    iRow = (unsigned int)(n+1u);
  }  // valid wafer

  return iRow;
}

unsigned int ITTT1Layer::numStripsInRow(const unsigned int iRow) const {

  unsigned int nStrip = 0u;
 
  if ((iRow<=nRows())&&(iRow>0u)){

    unsigned int lastWafer = lastWaferInRow(iRow);
    unsigned int iWafer = firstWaferInRow(iRow);

    while (iWafer <= lastWafer){
      nStrip += wafer(iWafer)->numStrips(); 
      ++iWafer;
    }
  } // valid row

  return nStrip;
}

unsigned int ITTT1Layer::firstWaferInRow(const unsigned int iRow) const {

 unsigned int firstWafer=0;
 if ((iRow<=nRows())){
   if (iRow == 1){
     firstWafer = 1;
   }
   else {
     firstWafer = m_rowsVector[iRow-2]+1; 
   }
 }

 
 return firstWafer;  
} 

unsigned int ITTT1Layer::lastWaferInRow(const unsigned int iRow) const {

 unsigned int lastWafer=0;
 if ((iRow<=nRows())&&(iRow>0u)){
   lastWafer = m_rowsVector[iRow-1]; 
 }
 
 return lastWafer;  
} 



