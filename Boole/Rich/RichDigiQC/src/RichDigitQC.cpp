// $Id: RichDigitQC.cpp,v 1.4 2004-03-16 13:52:46 jonesc Exp $

// local
#include "RichDigitQC.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichDigitQC
//
// 2003-09-08 : Chris Jones   (Christopher.Rob.Jones@cern.ch)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichDigitQC>          s_factory ;
const        IAlgFactory& RichDigitQCFactory = s_factory ;

// Standard constructor, initializes variables
RichDigitQC::RichDigitQC( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : RichAlgBase ( name, pSvcLocator ) {

  // Declare job options
  declareProperty( "InputDigits", m_digitTDS = MCRichDigitLocation::Default );
  declareProperty( "HistoPath", m_histPth = "RICH/DIGI/DIGITQC" );

}

// Destructor
RichDigitQC::~RichDigitQC() {};

// Initialisation
StatusCode RichDigitQC::initialize() {

  debug() << "Initialize" << endreq;

  // Initialize base class
  StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Book histograms
  if ( !bookHistograms() ) return StatusCode::FAILURE;

  // Initialise mult counts
  m_hitMult[Rich::Rich1] = 0;
  m_hitMult[Rich::Rich2] = 0;
  m_eventCount = 0;

  debug() << " Histogram location   = " << m_histPth << endreq;

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode RichDigitQC::execute() {

  debug() << "Execute" << endreq;

  // Locate MCRichDigits
  MCRichDigits * richDigits = get<MCRichDigits>( m_digitTDS );

  // Loop over all digits
  int nR1 = 0; int nR2 =0;
  for ( MCRichDigits::const_iterator iDigit = richDigits->begin();
        iDigit != richDigits->end();
        ++iDigit ) {

    if ( (*iDigit)->key().rich() == Rich::Rich1 ) {
      ++nR1;
    } else {
      ++nR2;
    }

  }

  m_rich1->fill( nR1 );
  m_rich2->fill( nR2 );
  ++m_eventCount;
  m_hitMult[ Rich::Rich1 ] += nR1;
  m_hitMult[ Rich::Rich2 ] += nR2;

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichDigitQC::finalize() {

  debug() << "Finalize" << endreq;

  const double avR1    = ( m_eventCount > 0 ? m_hitMult[Rich::Rich1]/m_eventCount : 0 );
  const double avR2    = ( m_eventCount > 0 ? m_hitMult[Rich::Rich2]/m_eventCount : 0 );
  const double avR1Err = ( m_eventCount > 0 ? sqrt(m_hitMult[Rich::Rich1])/m_eventCount : 0 );
  const double avR2Err = ( m_eventCount > 0 ? sqrt(m_hitMult[Rich::Rich2])/m_eventCount : 0 );  
  info() << "RICH1 digit multiplicity = " << avR1 <<" +- " << avR1Err << endreq
         << "RICH2 digit multiplicity = " << avR2 <<" +- " << avR2Err << endreq;

  // finalize base class
  return RichAlgBase::finalize();
}


StatusCode RichDigitQC::bookHistograms() {

  std::string title;
  const int nBins = 100;

  title = "RICH1 Occupancy";
  m_rich1 = histoSvc()->book( m_histPth, 1, title, nBins, 0, 5000 );

  title = "RICH2 Occupancy";
  m_rich2 = histoSvc()->book( m_histPth, 2, title, nBins, 0, 2000 );

  return StatusCode::SUCCESS;
}
