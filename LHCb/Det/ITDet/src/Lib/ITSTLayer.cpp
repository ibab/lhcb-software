// $Id: ITSTLayer.cpp,v 1.3 2002-09-05 07:10:34 mneedham Exp $
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

#include "ITDet/ITSTLayer.h"
#include "Kernel/ITChannelID.h"
#include "ITDet/ITWafer.h"


ITSTLayer::ITSTLayer(int stationID, int layerID, double z, 
                    double stereoAngle, double pitch, double waferWidth, 
		    double waferHeight, double waferThickness,
		    double waferOverlap, unsigned int wafersX, 
		    unsigned int wafersY, double holeX, double holeY, 
		    double ladderDist,
		     unsigned int numStrips, int position):
  ITDetectionLayer(stationID, layerID, z, 
                   stereoAngle, pitch,waferThickness,waferOverlap)
{


  double xOverlap = waferOverlap/cosAngle();
  double xStep = waferWidth/cosAngle() - xOverlap;

  if (wafersY <= 2) {
    m_WafersNum = 2*wafersX;
  } else {
    unsigned int numLadders;
    if (position == CENTER_POS1) {
      numLadders = (wafersY + 3)/4;
    } else if (position == CENTER_POS2) {
      numLadders = (wafersY + 1)/4;
    } else {
      numLadders = 0;
    }
    m_WafersNum = wafersX*numLadders*2;
  }
  double sensWaferWidth = pitch*(double)numStrips;

  m_Wafers.resize(m_WafersNum);

  double ladderHeight = wafersY*waferHeight;

  double topX = -(double)(wafersX-1)*xStep/2.;
  double topY1, topY2;
  double sideX1, sideX2;

    if (position != SIDE_POS && wafersY % 2 == 1) {
      topY1 = holeY + (waferHeight*cosAngle() + waferWidth*fabs(sinAngle()))/2.;
    } else {
      topY1 = holeY + waferHeight*cosAngle() + waferWidth*fabs(sinAngle())/2.;
    }
    topY2 = -topY1;
    sideX1 = -holeX - (waferWidth*cosAngle()+ladderHeight*fabs(sinAngle()))/2. - 
                    (wafersX-1)*xStep;
    sideX2 = holeX + (waferWidth*cosAngle()+ladderHeight*fabs(sinAngle()))/2.;

  unsigned int currWafer = 0;
  unsigned int currStrip = 0;

  unsigned int iWafer  = 0; // Here due to different scope rules on Windows
  unsigned int iLadder = 0; // Here due to different scope rules on Windows

  if (position == SIDE_POS) {
    double currSideX = sideX1;
    for(iWafer = 1; iWafer <= wafersX; iWafer++) {
      currWafer++;
      double u = currSideX*cosAngle();
      double v = -currSideX*sinAngle();
      double dz = ((double)((iWafer - 1)% 2) - 0.5)*ladderDist;

      m_Wafers[currWafer-1] = new ITWafer(pitch, 1, 
					  stationID,layerID,currWafer,
                        u - sensWaferWidth/2.,  u + sensWaferWidth/2., 
			v - ladderHeight/2., 
			v + ladderHeight/2., dz);
      currStrip += m_Wafers[currWafer-1]->lastStrip();
      currSideX += xStep;
    }
    currSideX = sideX2;
    for(iWafer = 1; iWafer <= wafersX; iWafer++) {
      currWafer++;
      double u = currSideX*cosAngle();
      double v = -currSideX*sinAngle();
      double dz = ((double)((wafersX - iWafer - 1) % 2) - 0.5)*ladderDist;

      m_Wafers[currWafer-1] = new ITWafer(pitch, 1, 
					  stationID,layerID,currWafer,
                        u - sensWaferWidth/2.,  u + sensWaferWidth/2., 
			v - ladderHeight/2., 
			v + ladderHeight/2., dz);
      currStrip += m_Wafers[currWafer-1]->lastStrip();
      currSideX += xStep;
    }
  } else if (position == CENTER_POS) {
    double currTopX = topX;
    for(iWafer = 1; iWafer <= wafersX; iWafer++) {
      currWafer++;
      double u = currTopX *cosAngle() + topY1*sinAngle();
      double v = topY1*cosAngle() - currTopX *sinAngle();
      double dz = ((double)((iWafer+1) % 2) - 0.5)*ladderDist;

      m_Wafers[currWafer-1] = new ITWafer(pitch, 1,
					  stationID,layerID,currWafer, 
                           u - sensWaferWidth/2.,  u + sensWaferWidth/2., 
			   v - ladderHeight/2., v + ladderHeight/2., dz);
      currStrip += m_Wafers[currWafer-1]->lastStrip() + 1;
      currTopX += xStep;
    }
    currTopX = topX;
    for(iWafer = 1; iWafer <= wafersX; iWafer++) {
      currWafer++;
      double u = currTopX *cosAngle() + topY2*sinAngle();
      double v = topY2*cosAngle() - currTopX *sinAngle();

      double dz = ((double)((iWafer+1) % 2) - 0.5)*ladderDist;

      m_Wafers[currWafer-1] = new ITWafer(pitch, 1, 
					  stationID,layerID, currWafer,
                           u - sensWaferWidth/2.,  u + sensWaferWidth/2., 
			   v - ladderHeight/2., v + ladderHeight/2., dz);
      currStrip += m_Wafers[currWafer-1]->lastStrip();
      currTopX += xStep;
    }
  } else if (position == CENTER_POS1) {
    unsigned int numLadders = (wafersY+3)/4;
    double lowerEdgeY = topY1;
    if (wafersY % 2 == 1) {
      lowerEdgeY -= waferHeight/2.;
    } else {
      lowerEdgeY -= waferHeight;
    }
    for(iLadder = 1; iLadder <= numLadders; iLadder++) {
      double currTopX = topX;
      double ladderHeight = 2*waferHeight;
      if (iLadder == 1 && wafersY % 2 == 1) {
        // first ladder if num wafers in Y is odd
        ladderHeight = waferHeight;
      }
      double currTopY = lowerEdgeY + ladderHeight/2.;
      lowerEdgeY += ladderHeight + 2*(waferHeight-waferOverlap);
      for(iWafer = 1; iWafer <= wafersX; iWafer++) {
        currWafer++;
        double u = currTopX*cosAngle() + currTopY*sinAngle();
        double v = currTopY*cosAngle() - currTopX*sinAngle();
        double dz = ((double)((iWafer+1) % 2) - 0.5)*ladderDist;

        m_Wafers[currWafer-1] = new ITWafer(pitch, 1, 
					    stationID,layerID, currWafer,
                           u - sensWaferWidth/2.,  u + sensWaferWidth/2., 
			   v - ladderHeight/2., v + ladderHeight/2., dz);
        currStrip += m_Wafers[currWafer-1]->lastStrip();
        currTopX += xStep;
      } // loop wafers
    } // loop ladders
    lowerEdgeY = topY2;
    if (wafersY % 2 == 1) {
      lowerEdgeY += waferHeight/2.;
    } else {
      lowerEdgeY += waferHeight;
    }
    for(iLadder = 1; iLadder <= numLadders; iLadder++) {
      double currTopX = topX;
      double ladderHeight = 2*waferHeight;
      if (iLadder == 1 && wafersY % 2 == 1) {
        // first ladder if num wafers in Y is odd
        ladderHeight = waferHeight;
      }
      double currTopY = lowerEdgeY - ladderHeight/2.;
      lowerEdgeY -= ladderHeight+2*(waferHeight-waferOverlap);
      currTopX = topX;
      for(iWafer = 1; iWafer <= wafersX; iWafer++) {
        currWafer++;
        double u = currTopX*cosAngle() + currTopY*sinAngle();
        double v = currTopY*cosAngle() - currTopX*sinAngle();

        double dz = ((double)((iWafer+1) % 2) - 0.5)*ladderDist;

        m_Wafers[currWafer-1] = new ITWafer(pitch, 1, 
					    stationID,layerID, currWafer,
                           u - sensWaferWidth/2.,  u + sensWaferWidth/2., 
			   v - ladderHeight/2., v + ladderHeight/2., dz);
        currStrip += m_Wafers[currWafer-1]->lastStrip();
        currTopX += xStep;
      } // loop wafers
    } // loop ladders
  } else if (position == CENTER_POS2) {
    unsigned int numLadders = (wafersY+1)/4;

    double lowerEdgeY = topY1;
    if (wafersY % 2 == 1) {
      lowerEdgeY -= waferHeight/2. - (waferHeight-waferOverlap);
    } else {
      lowerEdgeY -= waferHeight - (2*waferHeight-waferOverlap);
    }

    for(iLadder = 1; iLadder <= numLadders; iLadder++) {
      double currTopX = topX;
      double ladderHeight = 2*waferHeight;
      double currTopY = lowerEdgeY + ladderHeight/2.;
      lowerEdgeY += 2*(ladderHeight-waferOverlap);
      for(iWafer = 1; iWafer <= wafersX; iWafer++) {
        currWafer++;
        double u = currTopX*cosAngle() + currTopY*sinAngle();
        double v = currTopY*cosAngle() - currTopX*sinAngle();
        double dz = ((double)((iWafer+1) % 2) - 0.5)*ladderDist;

        m_Wafers[currWafer-1] = new ITWafer(pitch, 1, 
					    stationID,layerID,currWafer,
                           u - sensWaferWidth/2.,  u + sensWaferWidth/2., 
			   v - ladderHeight/2., v + ladderHeight/2., dz);
        currStrip += m_Wafers[currWafer-1]->lastStrip();
        currTopX += xStep;
      } // loop wafers
    } // loop ladders

    lowerEdgeY = topY2;
    if (wafersY % 2 == 1) {
      lowerEdgeY += waferHeight/2. - (waferHeight-waferOverlap);
    } else {
      lowerEdgeY += waferHeight - (2*waferHeight-waferOverlap);
    }

    for(iLadder = 1; iLadder <= numLadders; iLadder++) {
      double currTopX = topX;
      double ladderHeight = 2*waferHeight;
      double currTopY = lowerEdgeY - ladderHeight/2.;
      lowerEdgeY -= 2*(ladderHeight-waferOverlap);
      currTopX = topX;
      for(iWafer = 1; iWafer <= wafersX; iWafer++) {
        currWafer++;
        double u = currTopX*cosAngle() + currTopY*sinAngle();
        double v = currTopY*cosAngle() - currTopX*sinAngle();

        double dz = ((double)((iWafer+1) % 2) - 0.5)*ladderDist;

        m_Wafers[currWafer-1] = new ITWafer(pitch, 1,
					    stationID,layerID, currWafer,
                           u - sensWaferWidth/2.,  u + sensWaferWidth/2., 
			   v - ladderHeight/2., v + ladderHeight/2., dz);
        currStrip += m_Wafers[currWafer-1]->lastStrip();
        currTopX += xStep;
      } // loop wafers
    } // loop ladders
  }
  
  m_totNumStrips = currStrip; 
}

ITSTLayer::~ITSTLayer(){
  //destructer

  if (m_Wafers.size() != 0) {
    std::vector<ITWafer*>::iterator iWafer = m_Wafers.begin();
    while (iWafer != m_Wafers.end()) {
      ITWafer* aWafer = *iWafer;
      delete aWafer;
      m_Wafers.erase(iWafer);
    }
  }
}

double ITSTLayer::centerX(const ITChannelID stripID) const {
  return (this->U(stripID) - this->centerY(stripID)*sinAngle())/cosAngle();
}

double ITSTLayer::centerY(const ITChannelID stripID) const {

  double yC = 0.;
  const unsigned int waferNum = stripID.wafer();
  if (waferNum>=1 && waferNum <= m_Wafers.size()) {
    const double u = m_Wafers[waferNum-1]->U(stripID.strip());
    const double v = m_Wafers[waferNum-1]->centerV();
    yC = v*cosAngle() + u*sinAngle();
  }

  return yC;
}

double ITSTLayer::centerZ(const ITChannelID stripID) const {
  // z center of channel

  double zC = 0.;
  const unsigned int waferNum = stripID.wafer();
  if (waferNum>=1 && waferNum <= m_Wafers.size()) {
      zC = this->z() + m_Wafers[waferNum-1]->dZ();
  } 
  return zC;
}

double ITSTLayer::halfLengthX(const ITChannelID stripID) const {
  return -this->halfLengthY(stripID)*sinAngle()/cosAngle();
}

double ITSTLayer::halfLengthY(const ITChannelID stripID) const {
  const unsigned int stripNum = stripID.strip();
  for (unsigned int iWafer = 1; iWafer <= m_WafersNum; iWafer++) {
    if (m_Wafers[iWafer-1]->isStrip(stripNum)) {
      return m_Wafers[iWafer-1]->halfLengthV()*cosAngle();
    }
  }
  return 0.;
}

unsigned int ITSTLayer::numStrips() const {
  return m_totNumStrips;
}








