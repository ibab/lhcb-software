// $Id: STOfflinePosition.cpp,v 1.7 2006-03-22 13:02:16 mneedham Exp $

 
// Kernel
#include "GaudiKernel/ToolFactory.h"

 
// Event
#include "Event/STDigit.h"
#include "Event/STCluster.h"

#include <math.h> 

#include "STFun.h"
#include "STOfflinePosition.h" 

#include <boost/assign/std/vector.hpp>

using namespace boost::assign;
using namespace boost;

static ToolFactory<STOfflinePosition> s_factory;
const IToolFactory& STOfflinePositionFactory = s_factory;
 
STOfflinePosition::STOfflinePosition(const std::string& type, const std::string& name, const IInterface* parent) :
 GaudiTool(type, name, parent)
{
  // constructer
  m_ErrorVec += 0.22, 0.12, 0.24, 0.21;
  this->declareProperty("errorVec",m_ErrorVec);

  this->declareProperty("sharingCorr",m_sharingCorr = 112.);
  this->declareProperty("maxNtoCorr",m_MaxNtoCorr = 4);

  declareInterface<ISTClusterPosition>(this);
}

STOfflinePosition::~STOfflinePosition() {
  //destructer
}

ISTClusterPosition::Info STOfflinePosition::estimate(const LHCb::STCluster* aCluster) const{

  double stripNum = STFun::position(aCluster->stripValues());
  
  LHCb::STChannelID firstChan = aCluster->firstChannel();
  LHCb::STChannelID theChan = LHCb::STChannelID(firstChan.type(), firstChan.station(),
                                    firstChan.layer(), firstChan.detRegion(),
                                    firstChan.sector(), 
                                    (unsigned int)stripNum+firstChan.strip());
  
  ISTClusterPosition::Info theInfo; 
  theInfo.strip = theChan;
  theInfo.fractionalPosition = stripFraction(stripNum - floor(stripNum),aCluster->size());
  theInfo.fractionalError = error(aCluster->size());
                                                                             
  return theInfo;
}

ISTClusterPosition::Info STOfflinePosition::estimate(const SmartRefVector<LHCb::STDigit>& digits) const{
  
  double stripNum = STFun::position(digits);
 
  LHCb::STChannelID firstChan = digits.front()->channelID();
  LHCb::STChannelID theChan = LHCb::STChannelID(firstChan.type(), firstChan.station(),
                                    firstChan.layer(), firstChan.detRegion(),
                                    firstChan.sector(), (unsigned int)stripNum);
                                                                             
  ISTClusterPosition::Info theInfo; 
  theInfo.strip = theChan;
  theInfo.fractionalPosition = stripFraction(stripNum - floor(stripNum),digits.size());
  theInfo.fractionalError = error(digits.size());

  return theInfo;
}

double STOfflinePosition::error(const unsigned int nStrips) const{
 
 // estimate of error
 return (nStrips < m_ErrorVec.size() ? m_ErrorVec[nStrips-1] : m_ErrorVec.back());
}

double STOfflinePosition::stripFraction(const double stripNum,
                                        const unsigned int clusterSize) const{

  // 'S- shape correction' for non-linear charge sharing
  double corStripPos = stripNum - floor(stripNum);
  if ((clusterSize>1)&&(clusterSize<(unsigned)m_MaxNtoCorr)) {
     corStripPos = this->chargeSharingCorr(corStripPos);
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


