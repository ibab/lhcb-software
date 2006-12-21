// $Id: MCSTDigitChecker.cpp,v 1.4 2006-12-21 17:54:48 jvantilb Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "MCSTDigitChecker.h"

#include "Event/MCSTDigit.h"

#include "Kernel/STDetSwitch.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( MCSTDigitChecker );

//--------------------------------------------------------------------
//
//  MCSTDigitChecker : Check digitization procedure for the outer tracker
//
//--------------------------------------------------------------------

MCSTDigitChecker::MCSTDigitChecker(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
 GaudiHistoAlg(name, pSvcLocator)
 
{
  // constructer

  declareProperty("DetType", m_detType = "TT");
}

MCSTDigitChecker::~MCSTDigitChecker(){
  // destructer
}

StatusCode MCSTDigitChecker::initialize(){
 
  if( "" == histoTopDir() ) setHistoTopDir(m_detType+"/");
  
  m_digitLocation = MCSTDigitLocation::TTDigits;
  STDetSwitch::flip(m_detType,m_digitLocation);

  return GaudiHistoAlg::initialize();
}

StatusCode MCSTDigitChecker::execute(){

  // execute once per event

  // retrieve Digitizations
  MCSTDigits* digitsCont = get<MCSTDigits>(m_digitLocation);

  // number of digits
  plot((double)digitsCont->size()," num MCDigits" ,  0., 10000., 100);

  // histos per digit
  MCSTDigits::const_iterator iterDep = digitsCont->begin();
  for( ; iterDep != digitsCont->end(); ++iterDep){
    this->fillHistograms(*iterDep);
  } // loop iterDep

  return StatusCode::SUCCESS;
}

StatusCode MCSTDigitChecker::fillHistograms(const MCSTDigit* aDigit) const{

  // fill histos per digit

  // number of deposits that contribute
  plot((double)aDigit->mcDeposit().size()," num dep per digit", -0.5, 10.5, 11);

  // histogram by station
  const int iStation = aDigit->channelID().station();
  plot((double)iStation, " n dep per stat" ,-0.5 ,10.5, 11 );
  
  // by layer
  const int iLayer = aDigit->channelID().layer();
  plot((double)(100*iStation+iLayer)," n dep per layer"  , -0.5, 600.5, 601);

  // end
  return StatusCode::SUCCESS;
}












