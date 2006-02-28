// $Id: STOnlinePosition.cpp,v 1.5 2006-02-28 15:37:05 mneedham Exp $
 
// Kernel
#include "GaudiKernel/ToolFactory.h"
#include "Kernel/LHCbMath.h"
 
// Event
#include "Event/STDigit.h"
#include "Event/STCluster.h"

#include "STFun.h" 

#include <math.h>

#include "STOnlinePosition.h" 


static ToolFactory<STOnlinePosition> s_factory;
const IToolFactory& STOnlinePositionFactory = s_factory;
 
STOnlinePosition::STOnlinePosition(const std::string& type, const std::string& name, const IInterface* parent) :
 GaudiTool(type, name, parent)
{
  // constructer

  m_ErrorVec.resize(3);
  m_ErrorVec[0] = 0.22;
  m_ErrorVec[1] = 0.14;
  m_ErrorVec[2] = 0.25;

  this->declareProperty("errorVec",m_ErrorVec);

  declareInterface<ISTClusterPosition>(this);
}

STOnlinePosition::~STOnlinePosition() {
  //destructer
}

ISTClusterPosition::Info STOnlinePosition::estimate(const LHCb::STCluster* aCluster) const{
  double stripNum = STFun::position(aCluster->stripValues());
  double interStripPos = stripNum - floor(stripNum);
  LHCb::STChannelID firstChan = aCluster->firstChannel();
  LHCb::STChannelID theChan = LHCb::STChannelID( firstChan.type(),
                                    firstChan.station(),firstChan.layer(),
                                    firstChan.detRegion(),firstChan.sector(), 
                                    (unsigned int)stripNum+firstChan.strip());

  ISTClusterPosition::Info theInfo; 
  theInfo.strip = theChan;
  theInfo.fractionalPosition = interStripPos;
  theInfo.fractionalError = error(aCluster->size());

  return theInfo;
}

ISTClusterPosition::Info STOnlinePosition::estimate(const SmartRefVector<LHCb::STDigit>& digits) const{
  
  double stripNum = STFun::position(digits);
  double interStripPos = stripNum - floor(stripNum);
  LHCb::STChannelID firstChan = digits.front()->channelID();
  LHCb::STChannelID theChan = LHCb::STChannelID( firstChan.type(),
                                    firstChan.station(),firstChan.layer(),
                                    firstChan.detRegion(),firstChan.sector(), 
                                    (unsigned int)stripNum);
 
  ISTClusterPosition::Info theInfo; 
  theInfo.strip = theChan;
  theInfo.fractionalPosition = interStripPos;
  theInfo.fractionalError = error(digits.size());

  return theInfo;
}

double STOnlinePosition::error(const unsigned int nStrips) const{
 
 // estimate of error
 return (nStrips < m_ErrorVec.size() ? m_ErrorVec[nStrips-1] : m_ErrorVec.back());
}

double STOnlinePosition::stripFraction(const double interStripPos) const{
  return (LHCbMath::round(4*interStripPos))/4.0;
}

