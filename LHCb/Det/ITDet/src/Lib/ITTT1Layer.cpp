// $Id: ITTT1Layer.cpp,v 1.4 2002-09-05 07:10:34 mneedham Exp $
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
  double guardRingSize = (waferWidth -sensWaferWidth);
 
  unsigned int nSensorHigh=0;
  // max dimensions
  for (unsigned int iLad1=0; iLad1<ladderSize1.size();iLad1++){
    nSensorHigh += ladderSize1[iLad1]; 
  } // iLad

  double vMax = holeY + (double)nSensorHigh*waferHeight;
  double uMax = holeX + (double)wafersX2*(waferWidth - waferOverlap);

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

      ITWafer* aWafer = new ITWafer(pitch, 1,
				    stationID,layerID,waferOffset+iWafer,
                                    -0.5*sensWaferWidth, 0.5*sensWaferWidth, 
	   		            v - 0.5*ladderHeight, 
		 	            v + 0.5*ladderHeight, 0.5*pow(-1.,iWafer-1)*ladderDist);

       m_Wafers[waferOffset+iWafer-1] = aWafer;
       currStrip += aWafer->lastStrip();

    }  // iWafer
    v+= (waferHeight*(double)ladderSize1[iBWafer-1])/2.0;

  } // iBWafer

  // top box
  // wafer number offset
  startWafer = wafersX2+((1+(ladderSize2.size()/2))*wafersX2*2)
               + (wafersX1*ladderSize1.size());
  v = holeX;
  for (unsigned int iTWafer=1; iTWafer<=ladderSize1.size();iTWafer++){

    unsigned waferOffset = startWafer+(iTWafer-1)*((2*wafersX2)+wafersX1);
    
    v += (waferHeight*(double)ladderSize1[ladderSize1.size()-iTWafer])/2.0;
    double ladderHeight = ladderSize1[ladderSize1.size()-iTWafer]*waferHeight
                          -guardRingSize;

    for (unsigned int iWafer=1;iWafer<=wafersX1;iWafer++){
      ITWafer* aWafer = new ITWafer(pitch, 1,
				    stationID,layerID,waferOffset+iWafer,
                        -0.5*sensWaferWidth,0.5*sensWaferWidth, 
			v - 0.5*ladderHeight, 
			v + 0.5*ladderHeight,0.5*pow(-1.,iWafer-1)*ladderDist);

       m_Wafers[waferOffset+iWafer-1] = aWafer;
       currStrip += aWafer->lastStrip();

    } //iWafer

    v += (waferHeight*(double)ladderSize1[ladderSize1.size()-iTWafer])/2.0;

  } // iTWafer

  int iStart;
  if (wafersX2%2 == 0) iStart = 1;

  // left box

  // recalculate vmax
  // max dimensions
  nSensorHigh = 0;
  for (unsigned int iLad2=0; iLad2<ladderSize2.size()/2;iLad2++){
    nSensorHigh += ladderSize2[iLad2]; 
  } // iLad
  vMax = ((double)nSensorHigh+0.5)*waferHeight;

  startWafer = 0;
  v = -vMax;
  for (unsigned int iLWafer=1; iLWafer<=ladderSize2.size();iLWafer++){

    unsigned int waferOffset;
    if (iLWafer<=(1+(ladderSize2.size()/2))){
      waferOffset = startWafer+((iLWafer-1)*((2*wafersX2)+wafersX1));
    }
    else {
      waferOffset = startWafer+((iLWafer-1)*((2*wafersX2)+wafersX1))-1;
    }

    v += (waferHeight*(double)ladderSize2[iLWafer-1])/2.0; 
    double ladderHeight = (ladderSize2[iLWafer-1]*waferHeight)
                          -guardRingSize;

    for (unsigned int iWafer=1;iWafer<=wafersX2;iWafer++){
 
      double u;
      if (iWafer == 1){
        u = -uMax+(((double)iWafer-0.5)*waferWidth);
      }
      else {
        u = -uMax+(((double)iWafer-0.5)*(waferWidth-waferOverlap));
      }

      ITWafer* aWafer = new ITWafer(pitch, 1,  
				    stationID,layerID,iWafer+waferOffset,
                        u-0.5*sensWaferWidth,u+0.5*sensWaferWidth, 
			v - 0.5*ladderHeight, 
			v + 0.5*ladderHeight, 0.5*pow(-1,iStart+iWafer)*ladderDist);
      m_Wafers[iWafer+waferOffset-1] = aWafer;
      currStrip += aWafer->lastStrip();
  
    } // iWafer

    v += (waferHeight*(double)ladderSize2[iLWafer-1])/2.0; 

  } // iLWafer
  
  // right box
  v = -vMax;
  startWafer = wafersX1+wafersX2;
  //double uOffset = (double)(wafersX1)*sensWaferWidth;
  for (unsigned int iRWafer=1; iRWafer<=ladderSize2.size();iRWafer++){

    unsigned int waferOffset;
    if (iRWafer<=(ladderSize2.size()/2)){
      waferOffset =startWafer+((iRWafer-1)*((2*wafersX2)+wafersX1));
    }
    else {
      waferOffset =startWafer+((iRWafer-1)*((2*wafersX2)+wafersX1))-1;
    }    

    v += (waferHeight*(double)ladderSize2[iRWafer-1])/2.0; 
    double ladderHeight = ladderSize2[iRWafer-1]*waferHeight 
                          - 2.0*guardRingSize;


    for (unsigned int iWafer=1;iWafer<=wafersX2;iWafer++){
 
      double u = holeX+(((double)iWafer-0.5)*(waferWidth-waferOverlap));

      ITWafer* aWafer = new ITWafer(pitch,1,  
				    stationID,layerID,iWafer+waferOffset,
                        u-0.5*sensWaferWidth,u+0.5*sensWaferWidth, 
			v - 0.5*ladderHeight, 
			v + 0.5*ladderHeight,0.5*pow(-1,iStart+iWafer)*ladderDist);
      m_Wafers[iWafer+waferOffset-1] = aWafer;
      currStrip += aWafer->lastStrip();

    } // iWafer

    v += (waferHeight*(double)ladderSize2[iRWafer-1])/2.0; 

  } // iLWafer

  m_totStrips = currStrip;
 
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








