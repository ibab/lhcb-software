// Event
#include "Event/STDigit.h"

// local
#include "STDigitMonitor.h"

#include "Kernel/STDigitFun.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STDigitMonitor )

//--------------------------------------------------------------------
//
//  STDigitMonitor : Monitor the STDigits
//
//--------------------------------------------------------------------

STDigitMonitor::STDigitMonitor( const std::string& name, 
                                ISvcLocator* pSvcLocator ) :
  ST::HistoAlgBase(name, pSvcLocator)
{
  // constructer
  declareSTConfigProperty("InputData",m_dataLocation , STDigitLocation::TTDigits );
}

STDigitMonitor::~STDigitMonitor()
{
  // destructer
}

StatusCode STDigitMonitor::initialize()
{
  // Set the top directory to IT or TT.
  if( "" == histoTopDir() ) setHistoTopDir(detType()+"/");

  // Initialize GaudiHistoAlg
  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
     
  return StatusCode::SUCCESS;
}

StatusCode STDigitMonitor::execute()
{
  // retrieve STDigits
  const STDigits* digitsCont = get<STDigits>(m_dataLocation);
  
  // number of digits
  plot((double)digitsCont->size(), 1, "Number of digits", 0.,10000., 500);

  // histos per digit
  STDigits::const_iterator iterDigit = digitsCont->begin();
  for( ; iterDigit != digitsCont->end(); ++iterDigit){
    this->fillHistograms(*iterDigit);
  }

  double shorth = SiChargeFun::shorth(digitsCont->begin(), digitsCont->end());
  plot(shorth,"shorth", 0., 100., 200);

  return StatusCode::SUCCESS;
}

void STDigitMonitor::fillHistograms(const STDigit* aDigit)
{
  // histogram by station
  const int iStation = aDigit->station();
  plot((double)iStation, 2, "Number of digits per station" , -0.5, 4.5, 5);
  
  // by layer
  const int iLayer = aDigit->layer();
  plot((double)(10*iStation+iLayer), 3, "Number of digits per layer",
       -0.5, 40.5, 41);

  if ( fullDetail() ){
      plot(aDigit->depositedCharge(),detectorType(aDigit->channelID())+"/1", 
            "Deposited charge ", 0., 128., 128);
  }

}
