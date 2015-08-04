// Event
#include "Event/MCSTDigit.h"

// local
#include "MCSTDigitMonitor.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( MCSTDigitMonitor )

//--------------------------------------------------------------------
//
//  MCSTDigitMonitor : Make plots for MCSTDigits
//
//--------------------------------------------------------------------

MCSTDigitMonitor::MCSTDigitMonitor( const std::string& name, 
                                    ISvcLocator* pSvcLocator ) :
 ST::HistoAlgBase(name, pSvcLocator) 
{
  // constructer
  declareSTConfigProperty("InputData", m_digitLocation , MCSTDigitLocation::TTDigits );
}

MCSTDigitMonitor::~MCSTDigitMonitor()
{
  // destructer
}

StatusCode MCSTDigitMonitor::initialize()
{
  // Set the top directory to IT or TT.
  if( "" == histoTopDir() ) setHistoTopDir(detType()+"/");

  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
  

  return sc;
}

StatusCode MCSTDigitMonitor::execute()
{
  // retrieve Digitizations
  const MCSTDigits* digitsCont = get<MCSTDigits>(m_digitLocation);

  // number of digits
  plot((double)digitsCont->size(),1,"Number of MCDigits" ,  0., 10000., 100);

  // histos per digit
  MCSTDigits::const_iterator iterDep = digitsCont->begin();
  for( ; iterDep != digitsCont->end(); ++iterDep){
    this->fillHistograms(*iterDep);
  } // loop iterDep

  return StatusCode::SUCCESS;
}

void MCSTDigitMonitor::fillHistograms(const MCSTDigit* aDigit) const
{
  // number of deposits that contribute
  plot((double)aDigit->mcDeposit().size(),2,"Number of deposits per digit", 
       -0.5, 10.5, 11);

  // histogram by station
  const int iStation = aDigit->channelID().station();
  plot((double)iStation,3 ,"Number of digits per station" ,-0.5 , 4.5, 11 );
  
  // by layer
  const int iLayer = aDigit->channelID().layer();
  plot((double)(10*iStation+iLayer), "Number of digits per layer", 
       -0.5, 40.5, 41);

}
