// Kernel
#include "LHCbMath/LHCbMath.h"

// Boost
#include <boost/assign/std/vector.hpp>

// Mathematical tools
#include <math.h> 
 
// Event
#include "Event/STDigit.h"
#include "Event/STCluster.h"

#include "Kernel/STFun.h"
#include "STOnlinePosition.h"

using namespace boost::assign;
using namespace LHCb;

DECLARE_TOOL_FACTORY( STOnlinePosition )
 
STOnlinePosition::STOnlinePosition( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  GaudiTool(type, name, parent)
{
  m_errorVec += 0.22, 0.14, 0.25;
  declareProperty("ErrorVec",m_errorVec);
  declareProperty("APE", m_APE = 0.0);

  declareInterface<ISTClusterPosition>(this);
}

STOnlinePosition::~STOnlinePosition()
{
  //destructer
}

ISTClusterPosition::Info STOnlinePosition::estimate( const LHCb::STCluster* 
                                                     aCluster ) const
{
  double stripNum = STFun::position(aCluster->stripValues()).first;
  double interStripPos = STFun::stripFraction(stripNum - floor(stripNum));

  // Increase strip number by one when interstrip fraction equals 1
  if (interStripPos > 0.99) {    
    stripNum +=1;
    interStripPos = 0;
  }

  STChannelID firstChan = aCluster->firstChannel();
  STChannelID theChan = STChannelID( firstChan.type(), firstChan.station(),
                                     firstChan.layer(), firstChan.detRegion(),
                                     firstChan.sector(), 
                                     (unsigned int)stripNum+firstChan.strip());

  ISTClusterPosition::Info theInfo; 
  theInfo.strip = theChan;
  theInfo.fractionalPosition = interStripPos;
  theInfo.fractionalError = error(aCluster->size());
  theInfo.clusterSize = aCluster->size();
  return theInfo;
}

ISTClusterPosition::Info STOnlinePosition::estimate(const STCluster* aCluster,
                                                    const StateVector& ) const
{  
  return this->estimate( aCluster );
}


ISTClusterPosition::Info 
STOnlinePosition::estimate(const SmartRefVector<LHCb::STDigit>& digits) const
{  
  double stripNum = STFun::position(digits).first;
  double interStripPos = STFun::stripFraction(stripNum - floor(stripNum));
  LHCb::STChannelID firstChan = digits.front()->channelID();
 
  // Increase strip number by one when interstrip fraction equals 1
  if (interStripPos > 0.99) { 
    stripNum +=1; 
    interStripPos = 0;
  }

  STChannelID theChan = STChannelID( firstChan.type(), firstChan.station(),
                                     firstChan.layer(), firstChan.detRegion(),
                                     firstChan.sector(), 
                                     (unsigned int)stripNum);

  ISTClusterPosition::Info theInfo; 
  theInfo.strip = theChan;
  theInfo.fractionalPosition = interStripPos;
  theInfo.fractionalError = error(digits.size());
  theInfo.clusterSize = digits.size();
  return theInfo;
}

double STOnlinePosition::error(const unsigned int nStrips) const
{ 
 // estimate of error
 double eValue =0.0;
 nStrips < m_errorVec.size() ?  eValue = m_errorVec[nStrips-1] : eValue = m_errorVec.back();
 if (m_APE > 0.0){
   eValue = sqrt(eValue*eValue + m_APE*m_APE );
 }
 return eValue;
}

double STOnlinePosition::error( const unsigned int nStrips, 
                                const StateVector&) const
{ 
  return this->error( nStrips );  
}
