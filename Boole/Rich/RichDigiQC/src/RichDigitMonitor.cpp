// $Id: RichDigitMonitor.cpp,v 1.1.1.1 2003-06-30 16:56:39 jonesc Exp $

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// Event model
#include "Event/RichDigit.h"

// RichKernel
#include "RichKernel/RichSmartID.h"
#include "RichKernel/RichDetectorType.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// local
#include "RichDigitMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichDigitMonitor
//
// 2002-11-20 : Andy Presland   (Andrew.Presland@cern.ch)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichDigitMonitor>          s_factory ;
const        IAlgFactory& RichDigitMonitorFactory = s_factory ;


// Standard constructor, initializes variables
RichDigitMonitor::RichDigitMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : Algorithm ( name, pSvcLocator ) {

  // Declare job options
  declareProperty( "InputDigits", m_digitTDS = RichDigitLocation::Default );
  declareProperty( "HistoPath", m_histoPath = "RICH/DIGI/" );

}

// Destructor
RichDigitMonitor::~RichDigitMonitor() {};

// Initialisation
StatusCode RichDigitMonitor::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Book histograms
  if ( !bookHistograms() ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode RichDigitMonitor::execute() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Execute" << endreq;

  // Locate RichDigits
  SmartDataPtr<RichDigits> richDigits( eventSvc(), m_digitTDS );
  if ( !richDigits ) {
    msg << MSG::WARNING << "Cannot locate RichDigits at " << m_digitTDS << endreq;
    return StatusCode::FAILURE;
  } else {
    msg << MSG::DEBUG << "Successfully located " << richDigits->size()
        << " RichDigits at " << m_digitTDS << endreq;
  }

  // Loop over all digits
  int nR1 = 0; int nR2 =0;
  for ( RichDigits::const_iterator iDigit = richDigits->begin();
        iDigit != richDigits->end();
        ++iDigit ) {

    RichSmartID id = (*iDigit)->key();
    if ( id.rich() == Rich::Rich1 ) {
      ++nR1;
    } else {
      ++nR2;
    }

  }

  m_rich1->fill( nR1, 1. );
  m_rich2->fill( nR2, 1. );

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichDigitMonitor::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode RichDigitMonitor::bookHistograms() {

  MsgStream msg(msgSvc(), name());

  std::string title;
  StatusCode sc = StatusCode::SUCCESS;

  title = "RICH1 OCCUPANCY";
  m_rich1 = histoSvc()->book( m_histoPath+"1", title, 500, 0.0, 2000 );
  if ( !m_rich1 ) {
    msg << MSG::ERROR << "Failed to book histogram '"
        << title << "'" << endreq;
    sc = StatusCode::FAILURE;
  }

  title = "RICH2 OCCUPANCY";
  m_rich2 = histoSvc()->book( m_histoPath+"2", title, 500, 0.0, 5000 );
  if ( !m_rich2 ) {
    msg << MSG::ERROR << "Failed to book histogram '"
        << title << "'" << endreq;
    sc = StatusCode::FAILURE;
  }

  if ( sc ) msg << MSG::DEBUG << "Histograms booked successfully at "
                << m_histoPath << endreq;
  return sc;
}

