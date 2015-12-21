// Kernel
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "LHCbMath/LHCbMath.h"


// Mathematical tools
#include <math.h> 
 
// Event
#include "Event/STDigit.h"
#include "Event/STCluster.h"

#include "Kernel/STFun.h"
#include "STSmearedPosition.h"

using namespace LHCb;

DECLARE_TOOL_FACTORY( STSmearedPosition )
 
STSmearedPosition::STSmearedPosition( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent)
{
  declareProperty("CorrectionsVec",m_corrections = { 0.0 , 0.0 , 0.0 , 0.0 });
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
  auto tRandNumSvc = service<IRndmGenSvc>("RndmGenSvc", true);
  m_gaussDist = tRandNumSvc->generator( Rndm::Gauss(0.,1.0) );
  if (!m_gaussDist) return Error( "Failed to init generator ", sc);

  return StatusCode::SUCCESS; 
}

double STSmearedPosition::applySmear(double value, unsigned int nStrips) const {
  double corrValue = 0.0;
  corrValue = ( nStrips < m_corrections.size() ?  m_corrections[nStrips-1] : m_corrections.back() );
  corrValue *=  m_gaussDist->shoot();
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
