// ============================================================================
// Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"
// Event/CaloEvent
#include "Event/MCCaloHit.h" 
#include "Event/MCCaloDigit.h" 
#include "Event/CaloDigit.h" 
// local
#include "CaloDigitChecker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloDigitChecker
//
// 25/05/2001 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( CaloDigitChecker )

// Standard creator
CaloDigitChecker::CaloDigitChecker( const std::string& name, 
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
  setProperty( "HistoTopDir", "Calo/" );
}

//=============================================================================
// Standard destructor
//=============================================================================
CaloDigitChecker::~CaloDigitChecker() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CaloDigitChecker::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if(msgLevel(MSG::DEBUG)) debug() << " >>> Initialize" << endmsg;
  
  m_nameOfHits     = "MC/"  + m_nameOfDetector + "/Hits"   ;
  m_nameOfMCDigits = "MC/"  + m_nameOfDetector + "/Digits" ;
  m_nameOfDigits   = "Raw/" + m_nameOfDetector + "/Digits" ;
  
  info()      << "Monitoring Hits " << m_nameOfHits 
              << "   MC Digit   " << m_nameOfMCDigits
              << "   Digit   " << m_nameOfDigits
              << ", Histograms booked" << endmsg;

  return StatusCode::SUCCESS; 
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitChecker::execute() {
  if(msgLevel(MSG::DEBUG)) debug() << " >>> Execute" << endmsg;

/**  
 * Counting the MCCaloHit objects.
 */
  double nbHit      = 0.;
  double sumHit     = 0.;
  SmartDataPtr< LHCb::MCCaloHits > Hits ( eventSvc() , m_nameOfHits );
    
  if( 0 != Hits ) { 
    LHCb::MCCaloHits::const_iterator sig;
    for ( sig = Hits->begin() ; Hits->end() != sig ; ++sig ) {
      nbHit   += 1.;
      sumHit  += (*sig)->activeE();
    }
    sumHit    *= m_scaleHit / Gaudi::Units::GeV ;

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
  SmartDataPtr<LHCb::MCCaloDigits> MCDigits ( eventSvc(), m_nameOfMCDigits );

  if( 0 != MCDigits ) { 
    LHCb::MCCaloDigits::const_iterator dep;
    for ( dep = MCDigits->begin() ; MCDigits->end() != dep ; ++dep ) {
      nbMCDigit    += 1.;
      sumMCDigit   += (*dep)->activeE();
    }
    sumMCDigit     *= m_scaleHit / Gaudi::Units::GeV ;

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
  SmartDataPtr<LHCb::CaloDigits> digits ( eventSvc() , m_nameOfDigits );

  if( 0 != digits ) { 
    LHCb::CaloDigits::const_iterator dig;
    for ( dig = digits->begin() ; digits->end() != dig ; ++dig ) {
      nbDigit  += 1.;
      sumDigit += (*dig)->e();
    }
    sumDigit /= Gaudi::Units::GeV;
  
    plot1D(nbDigit  , 1103
           ,"Digits multiplicity " + m_nameOfDetector
           , 0.,  m_maxMultiplicity , 100 , 1.) ;
    plot1D(sumDigit , 1113
           ,"Total energy (GeV) in Digits " + m_nameOfDetector
           , 0.,  m_maxEnergy       , 100 , 1.) ;
  }
  
  if(msgLevel(MSG::DEBUG)) debug() << "#Hits=" << nbHit << " E=" << sumHit << " GeV, " 
                                   << "#MCDigit=" << nbMCDigit << " E=" <<  sumMCDigit << " GeV, " 
                                   << "#Digits " << nbDigit << " E=" << sumDigit << " GeV" << endmsg;
  
  return StatusCode::SUCCESS; 
}
