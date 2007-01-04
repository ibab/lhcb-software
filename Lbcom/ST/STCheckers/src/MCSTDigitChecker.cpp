// $Id: MCSTDigitChecker.cpp,v 1.6 2007-01-04 10:37:36 jvantilb Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCbKernel
#include "Kernel/STDetSwitch.h"

// Event
#include "Event/MCSTDigit.h"

// local
#include "MCSTDigitChecker.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( MCSTDigitChecker );

//--------------------------------------------------------------------
//
//  MCSTDigitChecker : Make plots for MCSTDigits
//
//--------------------------------------------------------------------

MCSTDigitChecker::MCSTDigitChecker( const std::string& name, 
                                    ISvcLocator* pSvcLocator ) :
 GaudiHistoAlg(name, pSvcLocator) 
{
  // constructer
  declareProperty("DetType", m_detType = "TT");
}

MCSTDigitChecker::~MCSTDigitChecker()
{
  // destructer
}

StatusCode MCSTDigitChecker::initialize()
{
  // Set the top directory to IT or TT.
  if( "" == histoTopDir() ) setHistoTopDir(m_detType+"/");
  
  m_digitLocation = MCSTDigitLocation::TTDigits;
  STDetSwitch::flip(m_detType,m_digitLocation);

  return GaudiHistoAlg::initialize();
}

StatusCode MCSTDigitChecker::execute()
{
  // retrieve Digitizations
  MCSTDigits* digitsCont = get<MCSTDigits>(m_digitLocation);

  // number of digits
  plot((double)digitsCont->size(),"Number of MCDigits" ,  0., 10000., 100);

  // histos per digit
  MCSTDigits::const_iterator iterDep = digitsCont->begin();
  for( ; iterDep != digitsCont->end(); ++iterDep){
    this->fillHistograms(*iterDep);
  } // loop iterDep

  return StatusCode::SUCCESS;
}

StatusCode MCSTDigitChecker::fillHistograms(const MCSTDigit* aDigit) const
{
  // number of deposits that contribute
  plot((double)aDigit->mcDeposit().size(),"Number of deposits per digit", 
       -0.5, 10.5, 11);

  // histogram by station
  const int iStation = aDigit->channelID().station();
  plot((double)iStation, "Number of digits per station" ,-0.5 , 4.5, 11 );
  
  // by layer
  const int iLayer = aDigit->channelID().layer();
  plot((double)(10*iStation+iLayer), "Number of digits per layer", 
       -0.5, 40.5, 41);

  // end
  return StatusCode::SUCCESS;
}
