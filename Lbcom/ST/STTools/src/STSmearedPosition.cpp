// Kernel
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "LHCbMath/LHCbMath.h"

// Boost
#include <boost/assign/std/vector.hpp>

// Mathematical tools
#include <math.h> 
 
// Event
#include "Event/STDigit.h"
#include "Event/STCluster.h"

#include "Kernel/STFun.h"
#include "STSmearedPosition.h"

using namespace boost::assign;
using namespace LHCb;

DECLARE_TOOL_FACTORY( STSmearedPosition )
 
STSmearedPosition::STSmearedPosition( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent)
{
  m_corrections += 0.0 , 0.0 , 0.0 , 0.0;
  declareProperty("CorrectionsVec",m_corrections);
  declareSTConfigProperty("BaseToolName", m_baseToolName , "TTClusterPosition");
  declareProperty("BaseToolType", m_baseToolType = "STOfflinePosition");

  declareInterface<ISTClusterPosition>(this);
}

StatusCode STSmearedPosition::initialize()
{
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // the underlying tool
  m_baseTool = tool<ISTClusterPosition>(m_baseToolType, m_baseToolName);

  // get the gaussian generator
  IRndmGenSvc* tRandNumSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  sc = tRandNumSvc->generator( Rndm::Gauss(0.,1.0), m_gaussDist.pRef() );
  if (sc.isFailure()) return Error( "Failed to init generator ", sc);
  sc = release(tRandNumSvc);
  if (sc.isFailure()) return Error( "Failed to release RndmGenSvc ", sc);

  return StatusCode::SUCCESS; 
}

double STSmearedPosition::applySmear(const double value, const unsigned int nStrips) const {
  double corrValue = 0.0;
  nStrips < m_corrections.size() ?  corrValue = m_corrections[nStrips-1] : corrValue = m_corrections.back();
  corrValue = corrValue* m_gaussDist->shoot();
  return sqrt(corrValue * corrValue + value*value);
}

STSmearedPosition::~STSmearedPosition()
{
  //destructer
}

ISTClusterPosition::Info STSmearedPosition::estimate( const STCluster*
                                                      aCluster ) const
{
  ISTClusterPosition::Info theInfo = m_baseTool->estimate(aCluster);
  const double newFracPos = applySmear(theInfo.fractionalPosition, 
                                       theInfo.clusterSize); 
  theInfo.fractionalPosition = newFracPos;
  return theInfo;
}

ISTClusterPosition::Info STSmearedPosition::estimate( const STCluster* aCluster,
                                                      const StateVector& ) const
{  
  return this->estimate( aCluster );
}



ISTClusterPosition::Info 
STSmearedPosition::estimate(const SmartRefVector<LHCb::STDigit>& digits) const
{  

  
  ISTClusterPosition::Info theInfo = m_baseTool->estimate(digits);
  const double newFracPos = applySmear(theInfo.fractionalPosition, theInfo.clusterSize); 
  theInfo.fractionalPosition = newFracPos;
  return theInfo;
}

double STSmearedPosition::error(const unsigned int nStrips) const
{ 
 // estimate of error
 double corrValue = 0.0;
 nStrips < m_corrections.size() ?  corrValue = m_corrections[nStrips-1] : corrValue = m_corrections.back();
 
 // get the value from the underlying tool
 const double eValue = m_baseTool->error(nStrips);

 return sqrt (eValue * eValue + corrValue * corrValue);
}

double STSmearedPosition::error( const unsigned int nStrips, 
                                 const StateVector&) const
{ 
  return this->error( nStrips );  
}
