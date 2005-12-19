// $Id: STOnlinePosition.cpp,v 1.1.1.1 2005-12-19 15:58:58 mneedham Exp $
 
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
  m_ErrorVec[0] = 0.04;
  m_ErrorVec[1] = 0.025;
  m_ErrorVec[2] = 0.045;

  this->declareProperty("errorVec",m_ErrorVec);

  declareInterface<ISTClusterPosition>(this);
}

STOnlinePosition::~STOnlinePosition() {
  //destructer
}

ISTClusterPosition::Measurement STOnlinePosition::estimate(const LHCb::STCluster* aCluster) const{
  return estimate(aCluster->digits());
}

ISTClusterPosition::Measurement STOnlinePosition::estimate(const SmartRefVector<LHCb::STDigit>& digits) const{
  
  double stripNum = STFun::position(digits);
  double interStripPos = stripNum - floor(stripNum);
  LHCb::STChannelID firstChan = digits.front()->channelID();
  LHCb::STChannelID theChan = LHCb::STChannelID(firstChan.station(),firstChan.layer(),
                                    firstChan.detRegion(),firstChan.sector(), (unsigned int)stripNum);

  // got to 2 bit precision on interstrip position
  double value  = double( LHCbMath::round(4*interStripPos))/4.0;
 
  return std::make_pair(std::make_pair(theChan,value),error(digits.size()));
}


double STOnlinePosition::error(const unsigned int nStrips) const{
 
 // estimate of error
 return (nStrips < m_ErrorVec.size() ? m_ErrorVec[nStrips-1] : m_ErrorVec.back());
}
