// $Id:
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/01/18 18:15:21  odescham
// v2r0 : adapt to the new Event Model + cleaning
//
// Revision 1.1.1.1  2004/10/25 08:57:08  ibelyaev
// // The new package: code is inported from Calo/CaloMonitor
//
// Revision 1.3  2003/03/10 14:35:15  ibelyaev
//  fix a problem
//
// Revision 1.2  2002/11/13 20:49:11  ibelyaev
//  small update of monitoring algorithms
//
// ============================================================================
//CLHEP
#include "Kernel/SystemOfUnits.h"
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ObjectVector.h"
// Event/CaloEvent
#include "Event/MCCaloHit.h" 
#include "Event/MCCaloDigit.h" 
#include "Event/CaloDigit.h" 
// local
#include "CaloDigitMonitor.h"


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
  : GaudiHistoAlg ( name , pSvcLocator            )
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
  SmartDataPtr< LHCb::MCCaloHits > Hits ( eventDataService() , m_nameOfHits );
  if( 0 != Hits ) { 
    LHCb::MCCaloHits::const_iterator sig;
    for ( sig = Hits->begin() ; Hits->end() != sig ; ++sig ) {
      nbHit   += 1.;
      sumHit  += (*sig)->activeE();
    }
    sumHit    *= m_scaleHit / GeV ;

    plot1D(nbHit  , 1101 
           ,"Hits multiplicity " + m_nameOfDetector 
           , 0., m_maxMultiplicity , 100 , 1.) ;
    plot1D(sumHit , 1111 
           ,"Total energy (GeV) in Hits " + m_nameOfDetector
           , 0., m_maxEnergy       , 100 , 1.) ;
  }
/**  
 * Counting the MCCaloDigit objects
 */
  double nbMCDigit    = 0.;
  double sumMCDigit   = 0.;
  SmartDataPtr<LHCb::MCCaloDigits> MCDigits ( eventDataService(), m_nameOfMCDigits );
  if( 0 != MCDigits ) { 
    LHCb::MCCaloDigits::const_iterator dep;
    for ( dep = MCDigits->begin() ; MCDigits->end() != dep ; ++dep ) {
      nbMCDigit    += 1.;
      sumMCDigit   += (*dep)->activeE();
    }
    sumMCDigit     *= m_scaleHit / GeV ;

    plot1D(nbMCDigit , 1102 
           ,"MCDigits multiplicity " + m_nameOfDetector
           , 0.,  m_maxMultiplicity , 100 , 1.) ;
    plot1D(sumMCDigit , 1112 
           ,"Total energy (GeV) in MCDigits " + m_nameOfDetector 
           , 0.,  m_maxEnergy       , 100 , 1.) ;
  }
/**  
 * Counting the Digit objects
 */
  double nbDigit  = 0.;
  double sumDigit = 0.;
  SmartDataPtr<LHCb::CaloDigits> digits ( eventDataService() , m_nameOfDigits );
  if( 0 != digits ) { 
    LHCb::CaloDigits::const_iterator dig;
    for ( dig = digits->begin() ; digits->end() != dig ; ++dig ) {
      nbDigit  += 1.;
      sumDigit += (*dig)->e();
    }
    sumDigit /= GeV;
  
    plot1D(nbDigit  , 1103
           ,"Digits multiplicity " + m_nameOfDetector
           , 0.,  m_maxMultiplicity , 100 , 1.) ;
    plot1D(sumDigit , 1113
           ,"Total energy (GeV) in Digits " + m_nameOfDetector
           , 0.,  m_maxEnergy       , 100 , 1.) ;
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
