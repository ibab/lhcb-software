// $Id: STSignalToNoiseTool.cpp,v 1.6 2007-01-09 15:02:24 jvantilb Exp $

// Gaudi files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// DigiEvent
#include "Event/STCluster.h"

// Geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

// LHCbKernel
#include "Kernel/STDataFunctor.h"

// local
#include "STSignalToNoiseTool.h"

//------------------------------------------------------------
// 
//------------------------------------------------------------

DECLARE_TOOL_FACTORY( STSignalToNoiseTool );

STSignalToNoiseTool::STSignalToNoiseTool( const std::string& type, 
                                          const std::string& name, 
                                          const IInterface* parent ) : 
  GaudiTool( type, name, parent ),
  m_tracker(0) 
{ 
  declareProperty("noiseParams",     m_paramsInElectron         ); 
  declareProperty("conversionToADC", m_conversionToADC = 0.0009 );
  declareProperty("detType",         m_detType         = "TT"   );

  m_paramsInElectron.push_back(776);
  m_paramsInElectron.push_back(47.9/Gaudi::Units::picofarad);

  // to get correct interface
  declareInterface<ISTSignalToNoiseTool>(this);
}

STSignalToNoiseTool::~STSignalToNoiseTool()
{
  //destructer
}

StatusCode STSignalToNoiseTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
   
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  // convert params to ADC
  for (unsigned int iParam = 0; iParam < m_paramsInElectron.size(); ++iParam){
    m_paramsInADC.push_back(m_conversionToADC*m_paramsInElectron[iParam]);
  }

  return sc;
}

double STSignalToNoiseTool::signalToNoise( const LHCb::STDigit* aDigit )
{
  // calculate S/N of STDigit
  return (aDigit->depositedCharge()/noiseInADC(aDigit->channelID()));
}

double STSignalToNoiseTool::signalToNoise( const LHCb::STCluster* aCluster )
{
  // calculate S/N of cluster
  return (aCluster->totalCharge()/noiseInADC(aCluster->channelID()));
}

double STSignalToNoiseTool::signalToNoise( const SmartRefVector<LHCb::STDigit>&
                                           digitCont )
{
  // S/N from SmartRefVector of ITDigits
  double totCharge = 
    std::accumulate(digitCont.begin(), digitCont.end(), 0.,
                    STDataFunctor::Accumulate_Charge<const LHCb::STDigit*>());

  return (totCharge/noiseInADC(digitCont.front()->channelID()));
}

double STSignalToNoiseTool::noiseInADC( const DeSTSector* aSector )
{
  // look up noise of Channel in ADC
  return  m_paramsInADC[0] + (aSector->capacitance()* m_paramsInADC[1]);
}

double STSignalToNoiseTool::noiseInElectrons( const DeSTSector* aSector )
{
  // look up noise of channel in electrons
  return m_paramsInElectron[0] + (aSector->capacitance()*m_paramsInElectron[1]);
}

double STSignalToNoiseTool::noiseInADC(const LHCb::STChannelID& aChan)
{
  // look up noise of Channel in ADC
  DeSTSector* tSector = m_tracker->findSector(aChan);
  return (tSector != 0 ?  noiseInADC(tSector) : 99999. ) ;
}

double STSignalToNoiseTool::noiseInElectrons(const LHCb::STChannelID& aChan)
{
  // look up noise of channel in electrons
  DeSTSector* tSector = m_tracker->findSector(aChan);
  return (tSector != 0 ?  noiseInElectrons(tSector) : 99999. ) ;
}

double STSignalToNoiseTool::convertToADC(const double aCharge)
{
  // conversion to ADC 
  return (aCharge*m_conversionToADC);
}
