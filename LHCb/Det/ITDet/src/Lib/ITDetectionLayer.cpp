// $Id: ITDetectionLayer.cpp,v 1.4 2003-01-17 14:07:15 sponce Exp $
//
// This File contains the definition of the ITDetectionLayer-class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: Matthew Needham
//   Created: 10-05-1999

//_________________________________________________
// ITDetectionLayer
// detection layer properties 

#include "ITDet/ITDetectionLayer.h"
#include "Kernel/ITChannelID.h"
#include "ITDet/ITWafer.h"


ITDetectionLayer::ITDetectionLayer(int stationID, int layerID, double z, 
                    double stereoAngle, double pitch, 
		    double waferThickness, double ladderDist):
  m_StationID(stationID),
  m_LayerID(layerID),
  m_Z(z),
  m_StereoAngle(stereoAngle),
  m_Cos(cos(stereoAngle)),
  m_Sin(sin(stereoAngle)),
  m_Thickness(waferThickness + ladderDist),
  m_WaferThickness(waferThickness),
  m_Pitch(pitch)
{

  //constructer
}

ITDetectionLayer::~ITDetectionLayer(){
  //destructer
  /*
  if (m_Wafers.size() != 0) {
    std::vector<ITWafer*>::iterator iWafer = m_Wafers.begin();
    while (iWafer != m_Wafers.end()) {
      ITWafer* aWafer = *iWafer;
      delete aWafer;
      m_Wafers.erase(iWafer);
    }
  }
  */
}

bool ITDetectionLayer::isInside(const double x, const double y) const {

  const double u = x*m_Cos + y*m_Sin;
  const double v = y*m_Cos - x*m_Sin;
  for (unsigned int iWafer = 1; iWafer <= m_WafersNum; iWafer++) {
    if (m_Wafers[iWafer-1]->isInside(u,v)) return true;
  }
  return false;
}

bool ITDetectionLayer::isInside(const double x, const double y, const double tol) const {

  const double u = x*m_Cos + y*m_Sin;
  const double v = y*m_Cos - x*m_Sin;
  for (unsigned int iWafer = 1; iWafer <= m_WafersNum; iWafer++) {
    if (m_Wafers[iWafer-1]->isInside(u,v,tol)) return true;
  }
  return false;
}

bool ITDetectionLayer::isInside(const double x, const double y, 
                                const ITChannelID aChan, const double tol) const {

  // check whether chan is inside given wafer
  bool isIn = false;

  const unsigned int waferNum = aChan.wafer();
  if ((waferNum>=1)&&(waferNum<=m_Wafers.size())){
    isIn = m_Wafers[waferNum-1]->isInside(x*m_Cos + y*m_Sin,y*m_Cos - x*m_Sin,tol);
  }

  return isIn;
}

ITChannelID ITDetectionLayer::stripID(const double u, const double v) const {

  unsigned int strip = ITChannelID::nullStrip;
  unsigned int iWafer = 1;

  while ((iWafer <= m_WafersNum )&&(strip == ITChannelID::nullStrip)){
    strip = m_Wafers[iWafer-1]->stripNum(u, v);
    iWafer++;
  } // iWafer

  return ITChannelID(this->stationID(),this->layerID(), iWafer, strip);
}

double ITDetectionLayer::U(const ITChannelID stripID) const {

  double u = 0.;
  const unsigned int waferNum = stripID.wafer();
  if (waferNum>=1 && waferNum <= m_Wafers.size()) {
    u = m_Wafers[waferNum-1]->U(stripID.strip());
  }

  return u;
}


unsigned int ITDetectionLayer::numStrips() const {
  return m_Wafers[m_WafersNum-1]->lastStrip() + 1;
}

ITWafer* ITDetectionLayer::wafer(const unsigned int iWafer) const{
 ITWafer* aWafer = 0;
 if (iWafer>=1 && iWafer <= m_Wafers.size()) {
    aWafer =  m_Wafers[iWafer-1];
 }
 return aWafer;
}

std::ostream& ITDetectionLayer::printOut( std::ostream& os ) const{

  os << " Layer: "  << m_LayerID << std::endl;
  os << "z " << m_Z 
     <<" n wafer "  << numWafers() 
     <<" Thickness "<< m_Thickness 
     << "Stereo angle  " << m_StereoAngle << std::endl;
        
  return os;
}


MsgStream& ITDetectionLayer::printOut( MsgStream& os ) const{

  os << " Layer: "  << m_LayerID << endreq;
  os << "z " << m_Z 
     << " n wafer "  << numWafers() 
     << " Thickness "<< m_Thickness  
     << "Stereo angle  " << m_StereoAngle << endreq;

  return os;
}

ITChannelID ITDetectionLayer::nextRight(const ITChannelID testChan) const{
  const unsigned int aStrip = testChan.strip();
  if (aStrip<numStrips()){
    return ITChannelID(testChan.station(),testChan.layer(),testChan.wafer(),aStrip+1u);
  }
  else {
   return ITChannelID(0,0,0,0);
  }
}

ITChannelID ITDetectionLayer::nextLeft(const ITChannelID testChan) const{
  const unsigned int aStrip = testChan.strip();
  if (aStrip>1u){
    return ITChannelID(testChan.station(),testChan.layer(),testChan.wafer(),aStrip-1u);
  }
  else {
   return ITChannelID(0,0,0,0);
  }
}











