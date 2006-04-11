// $Id: MCOTDepositMonitor.cpp,v 1.8 2006-04-11 19:23:16 janos Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// OTDet
#include "OTDet/DeOTDetector.h" 

// MCEvent
#include "Event/MCHit.h"
#include "Event/MCOTDeposit.h"

// MathCore
#include "Kernel/Point3DTypes.h"
#include "Kernel/SystemOfUnits.h"

/// BOOST
#include "boost/lexical_cast.hpp"

// local
#include "MCOTDepositMonitor.h"


/** @file MCOTDepositMonitor.cpp 
 *
 *  Implementation of the MCOTDepositMonitor class
 *  
 *  @author M. Needham
 *  @date   19-09-2000
 *  @Adapted J.Nardulli
 *  @date   20-07-2004
 */

/// Declaration of algorithm factory

DECLARE_ALGORITHM_FACTORY( MCOTDepositMonitor );

MCOTDepositMonitor::MCOTDepositMonitor( const std::string& name, 
					ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg(name, pSvcLocator),
  m_nCrossTalkHits(0) {
  // constructor
}

MCOTDepositMonitor::~MCOTDepositMonitor() {
  // destructor
}

StatusCode MCOTDepositMonitor::initialize() {
  
  if("" == histoTopDir()) setHistoTopDir("OT/");
  
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) {
    return Error("Failed to initialize", sc);
  }
  
  // Get OT Geometry from XML
  DeOTDetector* tracker = getDet<DeOTDetector>( DeOTDetectorLocation::Default );
  
  m_nStations = tracker->nStation();
  m_firstStation = tracker->firstStation();  

  // intialize histos
  this->initHistograms();

  return StatusCode::SUCCESS;
}

StatusCode MCOTDepositMonitor::execute() {
  // execute
  m_nCrossTalkHits = 0;

  // retrieve MCOTDeposits
  LHCb::MCOTDeposits* depCont = 
    get<LHCb::MCOTDeposits>( LHCb::MCOTDepositLocation::Default );

  // number of deposits
  m_nDepositsHisto->fill((double)depCont->size(),1.);

  // histos per deposit
  LHCb::MCOTDeposits::iterator iterDep;
  for ( iterDep = depCont->begin(); 
	iterDep != depCont->end(); iterDep++ ) {
    this->fillHistograms(*iterDep);
  } // loop iterDep

 if ( fullDetail() ) m_nCrossTalkHisto->fill((double)m_nCrossTalkHits,1.);
 
  return StatusCode::SUCCESS;
}

StatusCode MCOTDepositMonitor::initHistograms() {
 
  // number of deposits in container
  m_nDepositsHisto= book(1, "nDeposits",0., 20000., 200);
  // number of deposits per layer
  m_nHitsPerLayerHisto = book(3, "nDeposits per layer", 9.5, 34.5, 25);
  // drift distance
  m_driftDistHisto = book(6, "Drift distance", 0., 5., 50);
  
  // number of crosstalk hits in container
  if ( fullDetail() ) {
    // number of deposits per station
    m_nHitsPerStationHisto = book(2, "nDeposit per stat", 0.5, 3.5, 3);
    m_nCrossTalkHisto= book(11, "Number XTalk and noise deposits", 0., 1000., 100);

    // histograms per station
    int id;
    AIDA::IHistogram1D* aHisto1D;
    AIDA::IHistogram2D* aHisto2D;
    // drift time spectra
    for (int iStation = m_firstStation; iStation <= m_nStations; ++iStation) {
      std::string stationToString = boost::lexical_cast<std::string>(iStation);
      id=100+iStation;
      aHisto1D = book(id, "Deposit time per station "+stationToString,
		      -50.0*ns, 200.0*ns, 250);
      m_driftTimeHistos.push_back(aHisto1D);
      
      /// y vs x
      id=200+iStation;
      aHisto2D = book2D(id, "y(cm) vs x(cm) station "+stationToString, 
			-4000./cm,4000./cm, 800, -4000./cm,4000./cm, 800);
      m_yvsxHistos.push_back(aHisto2D);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode MCOTDepositMonitor::fillHistograms( LHCb::MCOTDeposit* aDeposit ) {

  // histogram per station
  const int iStation = aDeposit->channel().station();
  if ( fullDetail() ) {
    m_nHitsPerStationHisto->fill((double)iStation,1.);
  }
  
  // per layer
  const int iLayer = aDeposit->channel().layer();
  int iUniqueLayerNum = 10 * iStation + iLayer;
  m_nHitsPerLayerHisto->fill( (double)iUniqueLayerNum, 1.);

  // reference to mctruth
  const LHCb::MCHit* aHit = aDeposit->mcHit();

  if (0 != aHit) {
      // drift distance
    m_driftDistHisto->fill(aDeposit->driftDistance(),1.0);
    // aHit valid 
  } else {
    // crosstalk hit
    m_nCrossTalkHits++;
  }
  
  if ( fullDetail() ) {
    m_driftTimeHistos[iStation-m_firstStation]->fill(aDeposit->time(), 1.);
    if (0 != aHit) {
      // retrieve entrance + exit points and take average
      Gaudi::XYZPoint mcHitPoint = aHit->midPoint();
      // fill y vs x scatter plots    
      m_yvsxHistos[iStation-m_firstStation]->fill(mcHitPoint.x()/cm,
						  mcHitPoint.y()/cm);
    }
  }
 
  return StatusCode::SUCCESS;
}
