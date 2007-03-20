// $Id: STOfflinePosition.cpp,v 1.10 2007-03-20 17:36:56 jvantilb Exp $
 
// Kernel
#include "GaudiKernel/ToolFactory.h"
 
// Event
#include "Event/STDigit.h"
#include "Event/STCluster.h"

// Boost
#include <boost/assign/std/vector.hpp>

// Mathematical tools
#include <math.h> 

// local
#include "STFun.h"
#include "STOfflinePosition.h" 

using namespace boost::assign;
using namespace LHCb;

DECLARE_TOOL_FACTORY( STOfflinePosition );
 
STOfflinePosition::STOfflinePosition(const std::string& type, 
                                     const std::string& name,
                                     const IInterface* parent) :
  GaudiTool(type, name, parent)
{
  m_errorVec += 0.22, 0.12, 0.24, 0.21;
  declareProperty("ErrorVec",m_errorVec);
  declareProperty("SharingCorr",m_sharingCorr = 112.);
  declareProperty("MaxNtoCorr",m_maxNtoCorr = 4);

  declareInterface<ISTClusterPosition>(this);
}

STOfflinePosition::~STOfflinePosition() 
{
  //destructer
}

ISTClusterPosition::Info STOfflinePosition::estimate(const STCluster* 
                                                     aCluster) const 
{
  double stripNum = STFun::position(aCluster->stripValues());
  
  STChannelID firstChan = aCluster->firstChannel();
  STChannelID theChan = STChannelID(firstChan.type(), firstChan.station(),
                                    firstChan.layer(), firstChan.detRegion(),
                                    firstChan.sector(), 
                                    (unsigned int)stripNum+firstChan.strip());
  
  ISTClusterPosition::Info theInfo; 
  theInfo.strip = theChan;
  theInfo.fractionalPosition = stripFraction(stripNum, aCluster->size());
  theInfo.fractionalError = error(aCluster->size());
                                                                             
  return theInfo;
}

ISTClusterPosition::Info
STOfflinePosition::estimate(const SmartRefVector<STDigit>& digits) const
{  
  double stripNum = STFun::position(digits);
 
  STChannelID firstChan = digits.front()->channelID();
  STChannelID theChan = STChannelID(firstChan.type(), firstChan.station(),
                                    firstChan.layer(), firstChan.detRegion(),
                                    firstChan.sector(), (unsigned int)stripNum);
                                                                             
  ISTClusterPosition::Info theInfo; 
  theInfo.strip = theChan;
  theInfo.fractionalPosition = stripFraction( stripNum, digits.size() );
  theInfo.fractionalError = error(digits.size());

  return theInfo;
}

double STOfflinePosition::error(const unsigned int nStrips) const
{ 
 // estimate of error
 return (nStrips < m_errorVec.size() ? 
         m_errorVec[nStrips-1] : m_errorVec.back());
}

double STOfflinePosition::stripFraction(const double stripNum,
                                        const unsigned int clusterSize) const
{
  // 'S- shape correction' for non-linear charge sharing
  double corStripPos = stripNum - floor(stripNum);
  if ( (clusterSize>1) && ((int)clusterSize < m_maxNtoCorr) ) {
    corStripPos = this->chargeSharingCorr(corStripPos);
  }

  return corStripPos;
}

double STOfflinePosition::chargeSharingCorr(const double origDist) const
{ 
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
