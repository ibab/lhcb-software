//
// This File contains the definition of the OTSmearer -class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 19-09-2000

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Histogramming

#include "AIDA/IHistogram1D.h"

#include "STDigitChecker.h"
#include "Kernel/ISTSignalToNoiseTool.h"

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"


#include "Event/STDigit.h"

#include "Kernel/STDetSwitch.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STDigitChecker );

//--------------------------------------------------------------------
//
//  STDigitChecker : Check digitization procedure for the outer tracker
//
//--------------------------------------------------------------------

STDigitChecker::STDigitChecker(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  GaudiHistoAlg(name, pSvcLocator),
  m_tracker(0)
{
  // constructer
  declareProperty("sigNoiseTool",m_sigNoiseToolName = "STSignalToNoiseTool");
  this->declareProperty("detType", m_detType = "TT");
}

STDigitChecker::~STDigitChecker(){
  // destructer
}

StatusCode STDigitChecker::initialize(){
 
  if( "" == histoTopDir() ) setHistoTopDir(m_detType+"/");

  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
    
  // detector element     
  m_tracker =  getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));
 
  // sig to noise tool
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>(m_sigNoiseToolName);

  m_dataLocation = STDigitLocation::TTDigits;
  STDetSwitch::flip(m_detType,m_dataLocation); 
 
  return StatusCode::SUCCESS;
}

StatusCode STDigitChecker::execute(){

  // execute once per event

  // retrieve Digitizations
  STDigits* digitsCont = get<STDigits>(m_dataLocation);
  
  // number of digits
  plot((double)digitsCont->size(),1, "num digit",0., 10000., 500);

  // histos per digit
  STDigits::const_iterator iterDigit = digitsCont->begin();
  for( ; iterDigit != digitsCont->end(); ++iterDigit){
    this->fillHistograms(*iterDigit);
  } // loop iterDigit

  return StatusCode::SUCCESS;
}

StatusCode STDigitChecker::fillHistograms(const STDigit* aDigit) {

  // histogram by station on demand
  const int iStation = aDigit->channelID().station();
  plot((double)iStation, 2, "n digits per stat" , -0.5, 10.5, 11);
  
  // by layer
  const int iLayer = aDigit->channelID().layer();
  plot((double)(100*iStation+iLayer), 3 , "n digit per layer",-0.5,600.5, 601);

  if (fullDetail() == true){
    DeSTSector* aSector = m_tracker->findSector(aDigit->channelID());
    if (aSector != 0){
       plot(aDigit->depositedCharge(),aSector->type(), 0., 200., 200);
    }  
  }

  return StatusCode::SUCCESS;
}













