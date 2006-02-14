//
// This File contains the definition of the OTSmearer -class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 19-09-2000

// BOOST!
#include "boost/lexical_cast.hpp"

#include "Kernel/STDetSwitch.h"

// CLHEP
#include "Kernel/PhysicalConstants.h"

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#
// Histogramming
#include "AIDA/IHistogram2D.h"

#include "MCSTDepositChecker.h"

// xml geometry
#include "DetDesc/IGeometryInfo.h"
#include "STDet/DeSTDetector.h"

// Event
#include "Event/MCSTDeposit.h"
#include "Event/MCHit.h"

using namespace LHCb;

static const AlgFactory<MCSTDepositChecker> s_Factory;
const IAlgFactory& MCSTDepositCheckerFactory = s_Factory;

//--------------------------------------------------------------------
//
//  MCSTDepositChecker : Check digitization procedure for the outer tracker
//
//--------------------------------------------------------------------

MCSTDepositChecker::MCSTDepositChecker(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  GaudiHistoAlg(name, pSvcLocator),
  m_tracker(0)  
{
 // constructer
 declareProperty("detType", m_detType = "TT");
}

MCSTDepositChecker::~MCSTDepositChecker(){
  // destructer
}

StatusCode MCSTDepositChecker::initialize(){

  if( "" == histoTopDir() ) setHistoTopDir(m_detType+"/");

  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

      
  // detector element     
  m_tracker =  getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  m_depositLocation = MCSTDepositLocation::TTDeposits;
  STDetSwitch::flip(m_detType,m_depositLocation);

  // book vectors of histos
  if (fullDetail() == true) sc = initHistograms();
 
  return StatusCode::SUCCESS;
}

StatusCode MCSTDepositChecker::execute(){


  // retrieve Digitizations
  MCSTDeposits* depositsCont = get<MCSTDeposits>(m_depositLocation); 
 
  // number of digits
  plot((double)depositsCont->size(),1,"num dep",0.,10000.,200);

  // histos per digit
  for (MCSTDeposits::const_iterator iterDep = depositsCont->begin(); 
      iterDep != depositsCont->end(); ++iterDep){
    this->fillHistograms(*iterDep);
  } // loop iterDep

  return StatusCode::SUCCESS;
}

StatusCode MCSTDepositChecker::initHistograms(){

  // init 2-d histos -  

  // histograms per station
  AIDA::IHistogram2D* aHisto2D = 0;
  std::string tDirPath = this->histoPath()+"/";
  for (unsigned int iStation = m_tracker->firstStation();iStation<=m_tracker->lastStation();++iStation){
     // x vs y
     std::string ID = boost::lexical_cast<std::string>(201+(int)iStation);
     aHisto2D = histoSvc()->book(tDirPath+ID,"x vs y"+ID,50,
                          -1000./cm,1000./cm,50,-1000./cm,1000./cm);
     m_XvsYHistos.push_back(aHisto2D);

  } // loop station
 
  return StatusCode::SUCCESS;

}

StatusCode MCSTDepositChecker::fillHistograms(const MCSTDeposit* aDeposit) const{

  // dep charge
  plot(aDeposit->depositedCharge(),2,"dep charge", 0. , 500., 500);

  // histogram by station
  const int iStation = aDeposit->channelID().station();
  plot((double)iStation, 3, "n dep per stat", -0.5, 10.5, 11);
  
  // by layer
  plot((double)(100*iStation+aDeposit->channelID().layer()),
                 4, "n dep per layer",  -0.5, 600.5, 601);
      
   // detailed level histos
  if (fullDetail() == true){
    SmartRef<MCHit> aHit = aDeposit->mcHit();
    if (0 != aHit) {
    
      // take average
      Gaudi::XYZPoint impactPoint = aHit->midPoint();

      // fill x vs y scatter plots    
      m_XvsYHistos[iStation-1]->fill(impactPoint.x()/cm,
                                     impactPoint.y()/cm);
    }
  } // fullDetail

  return StatusCode::SUCCESS;

}













