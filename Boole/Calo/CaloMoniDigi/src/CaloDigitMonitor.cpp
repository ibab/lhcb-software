// $Id:
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2003/03/10 14:35:15  ibelyaev
//  fix a problem
//
// Revision 1.2  2002/11/13 20:49:11  ibelyaev
//  small update of monitoring algorithms
//
// ============================================================================
//
#define CALOALGS_CALODIGITMONITOR_CPP 1 
//CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// AIDA 
#include "AIDA/IHistogram1D.h"
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/IHistogramSvc.h"
// Event/CaloEvent
#include "Event/MCCaloHit.h" 
#include "Event/MCCaloDigit.h" 
#include "Event/CaloDigit.h" 
// local
#include "CaloDigitMonitor.h"

// MANDATORY!!!

static const AlgFactory<CaloDigitMonitor>          Factory ;
const       IAlgFactory& CaloDigitMonitorFactory = Factory ;
//-----------------------------------------------------------------------------
// Implementation file for class : CaloDigitMonitor
//
// 25/05/2001 : Olivier Callot
//-----------------------------------------------------------------------------

// Standard creator
CaloDigitMonitor::CaloDigitMonitor( const std::string& name, 
				    ISvcLocator* pSvcLocator) 
  : Algorithm                   ( name , pSvcLocator            )
  , m_maxMultiplicity ( 2000. )
  , m_maxEnergy       ( 2000. )      // This is in GeV.
  , m_scaleHit        ( 1.    )
{
  declareProperty( "Detector"         , m_nameOfDetector );
  declareProperty( "MaxMultiplicity"  , m_maxMultiplicity);
  declareProperty( "MaxEnergy"        , m_maxEnergy      );
  declareProperty( "ScaleHit"         , m_scaleHit       );
};

//=============================================================================
// Standard destructor
//=============================================================================
CaloDigitMonitor::~CaloDigitMonitor() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CaloDigitMonitor::initialize() {
  MsgStream log(messageService(), name());
  log << MSG::DEBUG << " >>> Initialize" << endreq;
  
  m_nameOfHits     = "MC/"  + m_nameOfDetector + "/Hits"   ;
  m_nameOfMCDigits = "MC/"  + m_nameOfDetector + "/Digits" ;
  m_nameOfDigits   = "Raw/" + m_nameOfDetector + "/Digits" ;
  
  double hMax;
  std::string histoDir = "Calo/" + m_nameOfDetector ;
  std::string hName   ;
  hMax = m_maxMultiplicity;
  hName    = "Hits multiplicity " + m_nameOfDetector;
  m_histMultHit = histoSvc()->book( histoDir, 1101, hName, 100, 0., hMax );

  hName    = "MCDigits multiplicity " + m_nameOfDetector;
  m_histMultMCD = histoSvc()->book( histoDir, 1102, hName, 100, 0., 2*hMax );

  hName    = "Digits multiplicity " + m_nameOfDetector;
  m_histMultDig = histoSvc()->book( histoDir, 1103, hName, 100, 0., hMax );

  hMax  = m_maxEnergy;
  hName = "Total energy (GeV) in Hits " + m_nameOfDetector;
  m_histEnergyHit = histoSvc()->book( histoDir, 1111, hName, 100, 0., hMax );

  hName = "Total energy (GeV) in MCDigits " + m_nameOfDetector;
  m_histEnergyMCD = histoSvc()->book( histoDir, 1112, hName, 100, 0., hMax );

  hName = "Total energy (GeV) in Digits " + m_nameOfDetector;
  m_histEnergyDig = histoSvc()->book( histoDir, 1113, hName, 100, 0., hMax );

  log << MSG::INFO 
      << "Monitoring Hits " << m_nameOfHits 
      << "   MC Digit   " << m_nameOfMCDigits
      << "   Digit   " << m_nameOfDigits
      << ", Histograms booked" << endreq;

  return StatusCode::SUCCESS; 
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitMonitor::execute() {
  MsgStream log(messageService(), name());
  log << MSG::DEBUG << " >>> Execute" << endreq;

/**  
 * Counting the MCCaloHit objects.
 */
  double nbHit      = 0.;
  double sumHit     = 0.;
  SmartDataPtr< MCCaloHits > Hits ( eventDataService() , m_nameOfHits );
  if( 0 != Hits ) { 
    MCCaloHits::const_iterator sig;
    for ( sig = Hits->begin() ; Hits->end() != sig ; ++sig ) {
      nbHit   += 1.;
      sumHit  += (*sig)->activeE();
    }
    sumHit    *= m_scaleHit / GeV ;
    m_histMultHit->   fill( nbHit, 1. );
    m_histEnergyHit-> fill( sumHit, 1. );
  }
/**  
 * Counting the MCCaloDigit objects
 */
  double nbMCDigit    = 0.;
  double sumMCDigit   = 0.;
  SmartDataPtr<MCCaloDigits> MCDigits ( eventDataService(), m_nameOfMCDigits );
  if( 0 != MCDigits ) { 
    MCCaloDigits::const_iterator dep;
    for ( dep = MCDigits->begin() ; MCDigits->end() != dep ; ++dep ) {
      nbMCDigit    += 1.;
      sumMCDigit   += (*dep)->activeE();
    }
    sumMCDigit     *= m_scaleHit / GeV ;
  
    m_histMultMCD->   fill( nbMCDigit, 1. );
    m_histEnergyMCD-> fill( sumMCDigit, 1. );
  }
/**  
 * Counting the Digit objects
 */
  double nbDigit  = 0.;
  double sumDigit = 0.;
  SmartDataPtr<CaloDigits> digits ( eventDataService() , m_nameOfDigits );
  if( 0 != digits ) { 
    CaloDigits::const_iterator dig;
    for ( dig = digits->begin() ; digits->end() != dig ; ++dig ) {
      nbDigit  += 1.;
      sumDigit += (*dig)->e();
    }
    sumDigit /= GeV;
    m_histMultDig-> fill( nbDigit, 1. );
    m_histEnergyDig->fill( sumDigit, 1. );
  }
  
  log << MSG::DEBUG
      << "#Hits=" << nbHit << " E=" << sumHit << " GeV, " 
      << "#MCDigit=" << nbMCDigit << " E=" <<  sumMCDigit << " GeV, " 
      << "#Digits " << nbDigit << " E=" << sumDigit << " GeV" << endreq;
  
  return StatusCode::SUCCESS; 
};
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloDigitMonitor::finalize() {
  
  MsgStream log(messageService(), name());
  log << MSG::DEBUG << " >>> Finalize" << endreq;
  return StatusCode::SUCCESS;
}
