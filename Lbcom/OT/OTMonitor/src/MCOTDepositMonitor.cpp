// $Id: MCOTDepositMonitor.cpp,v 1.11 2007-02-26 15:46:08 cattanem Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// OTDet
#include "OTDet/DeOTDetector.h" 

// MCEvent
#include "Event/MCHit.h"
#include "Event/MCOTDeposit.h"

// MathCore
#include "GaudiKernel/Point3DTypes.h"

/// BOOST
#include "boost/lexical_cast.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"

// local
#include "MCOTDepositMonitor.h"

/** @file MCOTDepositMonitor.cpp 
 *
 *  Implementation of the MCOTDepositMonitor class
 *  
 *  @author M. Needham
 *  @date   19-09-2000
 *  @author adapted by J.Nardulli
 *  @date   20-07-2004
 */

using namespace boost;
using namespace boost::lambda;
using boost::lexical_cast;

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

  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
  
  /// set path
  if("" == histoTopDir()) setHistoTopDir("OT/");
    
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
  LHCb::MCOTDeposits* depCont = get<LHCb::MCOTDeposits>(LHCb::MCOTDepositLocation::Default);

  // number of deposits
  m_nDepositsHisto->fill(double(depCont->size()));

  // histos per deposit
  for_each(depCont->begin(), depCont->end(), 
           bind(&MCOTDepositMonitor::fillHistograms, this, _1));
  
  if ( fullDetail() ) m_nCrossTalkHisto->fill(double(m_nCrossTalkHits));
 
  return StatusCode::SUCCESS;
}

StatusCode MCOTDepositMonitor::initHistograms() {
 
  // number of deposits in container
  m_nDepositsHisto = book(1, "Number of deposits",0., 20000., 200);
  // number of deposits per layer
  m_nHitsPerLayerHisto = book(3, "Number of deposits per layer", 9.5, 34.5, 25);
  // drift distance
  m_driftDistHisto = book(6, "Drift distance", 0., 5., 50);
  
  // number of crosstalk hits in container
  if ( fullDetail() ) {
    // number of deposits per station
    m_nHitsPerStationHisto = book(2, "Number of deposits per station", 0.5, 3.5, 3);
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
		      -50.0*Gaudi::Units::ns, 200.0*Gaudi::Units::ns, 250);
      m_driftTimeHistos.push_back(aHisto1D);
      
      /// y vs x
      id=200+iStation;
      aHisto2D = book2D(id, "y(cm) vs x(cm) station "+stationToString, 
			-3500./Gaudi::Units::cm,3500./Gaudi::Units::cm, 700, 
                        -3500./Gaudi::Units::cm,3500./Gaudi::Units::cm, 700);
      m_yvsxHistos.push_back(aHisto2D);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode MCOTDepositMonitor::fillHistograms( LHCb::MCOTDeposit* aDeposit ) {
  
  // per layer
  int iStation = (aDeposit->channel()).station();
  int iLayer = (aDeposit->channel()).layer();
  int iUniqueLayerNum = 10 * iStation + iLayer;
  
  m_nHitsPerLayerHisto->fill(double(iUniqueLayerNum));
 
  // reference to mctruth
  const LHCb::MCHit* aHit = aDeposit->mcHit();
  
  aHit?m_driftDistHisto->fill(aDeposit->driftDistance()):++m_nCrossTalkHits;
  
  // if (0 != aHit) {
//     // drift distance
//     m_driftDistHisto->fill(aDeposit->driftDistance());
//     // aHit valid 
//   } else {
//     // crosstalk hit
//     ++m_nCrossTalkHits;
//   }
  
  if ( fullDetail() ) {
     // histogram per station
    m_nHitsPerStationHisto->fill(double(iStation));
    m_driftTimeHistos[iStation-m_firstStation]->fill(aDeposit->time());
    if (0 != aHit) {
      // retrieve entrance + exit points and take average
      Gaudi::XYZPoint mcHitPoint = aHit->midPoint();
      // fill y vs x scatter plots    
      m_yvsxHistos[iStation-m_firstStation]->fill(mcHitPoint.x()/Gaudi::Units::cm, mcHitPoint.y()/Gaudi::Units::cm);
    }
  }
 
  return StatusCode::SUCCESS;
}
