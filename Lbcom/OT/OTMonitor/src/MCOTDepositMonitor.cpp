// $Id: MCOTDepositMonitor.cpp,v 1.3 2004-11-10 13:03:42 jnardull Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IHistogramSvc.h"

// CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"

// OTDet
#include "OTDet/DeOTDetector.h" 

//Event
#include "Event/MCHit.h"

// local
#include "MCOTDepositMonitor.h"

static const AlgFactory<MCOTDepositMonitor> s_Factory;
const IAlgFactory& MCOTDepositMonitorFactory = s_Factory;

/** @file MCOTDepositMonitor.cpp 
 *
 *  Implementation of the MCOTDepositMonitor class
 *  
 *  @author M. Needham
 *  @date   19-09-2000
 *  @Adapted J.Nardulli
 *  @date   20-07-2004
 */

MCOTDepositMonitor::MCOTDepositMonitor(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  OTMonitorAlgorithm(name, pSvcLocator),
  m_nCrossTalkHits(0)
{
  // constructor
}

MCOTDepositMonitor::~MCOTDepositMonitor()
{
  // destructor
}

StatusCode MCOTDepositMonitor::initialize()
{
  // Loading OT Geometry from XML
  SmartDataPtr<DeOTDetector> tracker( detSvc(), "/dd/Structure/LHCb/OT" );
  if ( !tracker ) {
    return Error("Unable to retrieve OT detector element from xml");
  }
  m_numStations = tracker->numStations();
  m_firstOTStation = tracker->firstOTStation();  

  // intialize histos
  this->initHistograms();

  return StatusCode::SUCCESS;
}


StatusCode MCOTDepositMonitor::execute()
{
  // execute
  m_nCrossTalkHits = 0;

  // retrieve MCOTDeposits
  SmartDataPtr<MCOTDeposits> depCont(eventSvc(),MCOTDepositLocation::Default);
  if (!depCont){
    warning () << "Failed to find OT deposits container" << endreq;
    return StatusCode::FAILURE;
  }

  // number of deposits
  m_nDepositsHisto->fill((double)depCont->size(),1.0);

  // histos per deposit
  MCOTDeposits::iterator iterDep;
  for(iterDep = depCont->begin(); 
      iterDep != depCont->end(); iterDep++){
    this->fillHistograms(*iterDep);
  } // loop iterDep

 if ( fullDetail() ) m_nCrossTalkHisto->fill((double)m_nCrossTalkHits,1.);
 
  return StatusCode::SUCCESS;
}

StatusCode MCOTDepositMonitor::finalize()
{
  // Finalize
  return StatusCode::SUCCESS;
}

StatusCode MCOTDepositMonitor::initHistograms()
{
  // Intialize histograms
  std::string tDirPath = this->histoDirPath();

  // number of deposits in container
  m_nDepositsHisto= histoSvc()->book(tDirPath+"1","num deposits",200,0.,20000.);

  // number of deposits per station
  if ( fullDetail() ) {
    m_nHitsPerStationHisto = histoSvc()->book(tDirPath+"2","n deposit per stat",
                                              3, 0.5, 3.5);
  }
  
  // number of deposits per layer
  m_nHitsPerLayerHisto = histoSvc()->book( tDirPath+"3","n deposit per layer",
                                           25, 9.5, 34.5);
  
  // drift distance
  m_driftDistHisto = histoSvc()->book( tDirPath+"6","drift distance", 
                                       50, 0., 5.);
  
  // number of crosstalk hits in container
  if ( fullDetail() ) {
    m_nCrossTalkHisto= histoSvc()->book( tDirPath+"11",
                                         "num XTalk and noise deposits",
                                         100, 0., 1000.);

    // histograms per station
    int ID;
    IHistogram1D* aHisto1D;
    IHistogram2D* aHisto2D;
    int iStation;
    // drift time spectra
    for (iStation = m_firstOTStation; iStation <= m_numStations; ++iStation) {
      ID=100+iStation;
      std::string aString= this->intToString(ID);
      aHisto1D = histoSvc()->book( tDirPath+this->intToString(ID),
                                  "deposit time station "+
                                   this->intToString(iStation),
                                   250, -50.0*ns, 200.0*ns );
      m_driftTimeHistos.push_back(aHisto1D);
    }
    
    // x vs y
    for (iStation = m_firstOTStation; iStation <= m_numStations; ++iStation) {
      ID=200+iStation;
      aHisto2D = histoSvc()->book(tDirPath+this->intToString(ID),
                                  "x(cm) vs y(cm) station "+
                                  this->intToString(iStation), 100,
                                  -4000./cm,4000./cm, 100,-4000./cm,4000./cm);
      m_xvsyHistos.push_back(aHisto2D);
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode MCOTDepositMonitor::fillHistograms(MCOTDeposit* aDeposit)
{
  // histogram by station
  const int iStation = aDeposit->channel().station();
  if ( fullDetail() ) {
    m_nHitsPerStationHisto->fill((double)iStation,1.);
  }
  
  // by layer
  const int iLayer = aDeposit->channel().layer();
  int iUniqueLayerNum = 10 * iStation + iLayer;
  m_nHitsPerLayerHisto->fill( (double)iUniqueLayerNum, 1.);

  if ( fullDetail() ) {
    m_driftTimeHistos[iStation-m_firstOTStation]->fill(aDeposit->time(), 1.);
  }
  
  // reference to mctruth
  MCHit* aHit = aDeposit->mcHit();

  if (0 != aHit) {
    // pointer valid hit deposit is real.....

    if ( fullDetail() ) {
      // retrieve entrance + exit points and take average
      HepPoint3D mcHitPoint = 0.5*(aHit->entry()+aHit->exit());

      // fill x vs y scatter plots    
      m_xvsyHistos[iStation-m_firstOTStation]->fill(mcHitPoint.x()/cm,
                                                    mcHitPoint.y()/cm);
    }
    
    // drift distance
    m_driftDistHisto->fill(aDeposit->driftDistance(),1.0);

  } // aHit valid 
  else {
    // crosstalk hit
    m_nCrossTalkHits++;
  }

  // end
  return StatusCode::SUCCESS;

}
