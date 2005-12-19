// $Id: STSignalToNoiseTool.cpp,v 1.1.1.1 2005-12-19 15:58:58 mneedham Exp $
//
// This File contains the implementation of the STSignalToNoiseTool class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 19-09-2000

// Gaudi files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Kernel/SystemOfUnits.h"

// ITEvent
#include "Event/STCluster.h"

// Geom
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"


// LHCbKernel
#include "Kernel/STDataFunctor.h"

// local
#include "STSignalToNoiseTool.h"


//------------------------------------------------------------
// This tool is used to add crosstalk in IT
//------------------------------------------------------------

static ToolFactory<STSignalToNoiseTool> s_factory;
const IToolFactory& STSignalToNoiseToolFactory = s_factory;

STSignalToNoiseTool::STSignalToNoiseTool(const std::string& type, 
                const std::string& name, const IInterface* parent): 
GaudiTool( type, name, parent ),
m_tracker(0) 
{ 
  // constructor

  declareProperty("noiseParams",m_paramsInElectron); 
  declareProperty("conversionToADC",m_conversionToADC = 0.0009);
  declareProperty("detType", m_detType = "TT");

  m_paramsInElectron.push_back(700);
  m_paramsInElectron.push_back(48.0/picofarad);

  // to get correct interface
  declareInterface<ISTSignalToNoiseTool>(this);
}

STSignalToNoiseTool::~STSignalToNoiseTool(){

 //destructer
}

/// initialize
StatusCode STSignalToNoiseTool::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
   
  m_tracker =  getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  // convert params to ADC
  for (unsigned int iParam = 0 ; iParam < m_paramsInElectron.size(); ++iParam){
    m_paramsInADC.push_back(m_paramsInElectron[iParam]);
  }

  return StatusCode::SUCCESS;
}

double STSignalToNoiseTool::signalToNoise(const LHCb::STDigit* aDigit) {
  // calc S/N of ITDigit
  return (aDigit->depositedCharge()/noiseInADC(aDigit->channelID()));
}

double STSignalToNoiseTool::signalToNoise( const LHCb::STCluster* aCluster ) {
  // calculate S/N of cluster
  return (signalToNoise(aCluster->digits()));
}

double STSignalToNoiseTool::signalToNoise(const SmartRefVector<LHCb::STDigit>& digitCont) {
  // S/N from SmartRefVector of ITDigits
  double totCharge = std::accumulate(digitCont.begin(),
                                digitCont.end(),0.,
                       STDataFunctor::Accumulate_Charge<const LHCb::STDigit*>());

  return (totCharge/noiseInADC(digitCont.front()->channelID()));
}

double STSignalToNoiseTool::noiseInADC(const DeSTSector* aSector) {

  // look up noise of Channel in electrons
  return  m_paramsInADC[0] + (aSector->capacitance()* m_paramsInADC[1]);
}

double STSignalToNoiseTool::noiseInElectrons(const DeSTSector* aSector) {

  // look up noise of channel in ADC
  return m_paramsInElectron[0] + (aSector->capacitance()* m_paramsInElectron[1]);
}

double STSignalToNoiseTool::noiseInADC(const LHCb::STChannelID& aChan) {

  // look up noise of Channel in electrons
  DeSTSector* tSector = m_tracker->findSector(aChan);
  return (tSector != 0 ?  noiseInADC(tSector) : 99999. ) ;
}

double STSignalToNoiseTool::noiseInElectrons(const LHCb::STChannelID& aChan) {

  // look up noise of channel in ADC
  DeSTSector* tSector = m_tracker->findSector(aChan);
  return (tSector != 0 ?  noiseInElectrons(tSector) : 99999. ) ;
}

double STSignalToNoiseTool::convertToADC(const double aCharge) {

  // conversion to ADC 
  return (aCharge*m_conversionToADC);
}


