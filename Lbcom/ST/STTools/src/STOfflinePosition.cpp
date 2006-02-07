// $Id: STOfflinePosition.cpp,v 1.3 2006-02-07 08:46:28 mneedham Exp $

 
// Kernel
#include "GaudiKernel/ToolFactory.h"

 
// Event
#include "Event/STDigit.h"
#include "Event/STCluster.h"

#include <math.h> 

#include "STFun.h"
#include "STOfflinePosition.h" 

static ToolFactory<STOfflinePosition> s_factory;
const IToolFactory& STOfflinePositionFactory = s_factory;
 
STOfflinePosition::STOfflinePosition(const std::string& type, const std::string& name, const IInterface* parent) :
 GaudiTool(type, name, parent)
{
  // constructer
  m_ErrorVec.resize(4);
  m_ErrorVec[0] = 0.04;
  m_ErrorVec[1] = 0.02;
  m_ErrorVec[2] = 0.044;
  m_ErrorVec[3] = 0.039;
  this->declareProperty("errorVec",m_ErrorVec);

  this->declareProperty("sharingCorr",m_sharingCorr = 112.);
  this->declareProperty("maxNtoCorr",m_MaxNtoCorr = 4);

  declareInterface<ISTClusterPosition>(this);
}

STOfflinePosition::~STOfflinePosition() {
  //destructer
}

ISTClusterPosition::Measurement STOfflinePosition::estimate(const LHCb::STCluster* aCluster) const{
  double stripNum = STFun::position(aCluster->stripValues());
  
  LHCb::STChannelID firstChan = aCluster->firstChannel();
  LHCb::STChannelID theChan = LHCb::STChannelID(firstChan.type(), firstChan.station(),
                                    firstChan.layer(), firstChan.detRegion(),
                                    firstChan.sector(), (unsigned int)stripNum);
                                                                             
  return std::make_pair(std::make_pair(theChan,
                        stripFraction(stripNum - floor(stripNum),aCluster->size())),
                        error(aCluster->size()));
}

ISTClusterPosition::Measurement STOfflinePosition::estimate(const SmartRefVector<LHCb::STDigit>& digits) const{
  
  double stripNum = STFun::position(digits);
 
  LHCb::STChannelID firstChan = digits.front()->channelID();
  LHCb::STChannelID theChan = LHCb::STChannelID(firstChan.type(), firstChan.station(),
                                    firstChan.layer(), firstChan.detRegion(),
                                    firstChan.sector(), (unsigned int)stripNum);
                                                                             
  return std::make_pair(std::make_pair(theChan,
                        stripFraction(stripNum - floor(stripNum),digits.size())),
                        error(digits.size()));
}

double STOfflinePosition::error(const unsigned int nStrips) const{
 
 // estimate of error
 return (nStrips < m_ErrorVec.size() ? m_ErrorVec[nStrips-1] : m_ErrorVec.back());
}

double STOfflinePosition::stripFraction(const double stripNum,
                                       const unsigned int clusterSize) const{

  // 'S- shape correction' for non-linear charge sharing
  double interStripPos = stripNum - floor(stripNum);
  const unsigned int nDigits = clusterSize;
  double corStripPos = interStripPos;
  if ((nDigits>1)&&(nDigits<(unsigned)m_MaxNtoCorr)) {
     corStripPos = this->chargeSharingCorr(interStripPos);
  }

  return corStripPos;
}

double STOfflinePosition::chargeSharingCorr(const double origDist) const{
 
  // non-linear charge sharing correction

  double newDist = origDist-0.5;

  if (m_sharingCorr > 0.0) {
    if (newDist<0.) {
      newDist = -pow(-newDist/m_sharingCorr, 1.0/3.0);
    }
    else {
      newDist = pow(newDist/m_sharingCorr, 1.0/3.0);
    }
  } // apply corr

  return newDist+0.5;
}


