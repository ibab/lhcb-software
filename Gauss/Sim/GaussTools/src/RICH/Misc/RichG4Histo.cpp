// from Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
//local

#include "RichG4Histo.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

//
//   Author SE 21-8-2002 
//

RichG4Histo::RichG4Histo(ISvcLocator* svcLoc) {

  m_RichG4HistoPath="RICHG4HISTO/";

  // Book histograms
  bookRichG4Histograms() ;  
}
RichG4Histo::~RichG4Histo() { ; }

void RichG4Histo::bookRichG4Histograms() {

  IMessageSvc* msgSvc;
  StatusCode sc = svcLoc->service( "MessageSvc", msgSvc, true );

  MsgStream RichG4Histolog(msgSvc, "RichG4Histo" );

  RichG4Histolog << MSG::INFO << "Now Booking Rich G4 Histo" << endreq;
  std::string title;

  IHistogramSvc* CurHistoSvc;
  sc = svcLoc->service( "HistogramSvc", CurHistoSvc, true );

  title="Total number of Rich1 Hits per event ";


  RichG4Histolog << MSG::INFO<<"Current Histo Path  "<<m_RichG4HistoPath<<endreq;

  m_hNumTotHitRich1= CurHistoSvc->book(m_RichG4HistoPath+"1",title,100,0.0,200.0);
  if(! m_hNumTotHitRich1) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="GlobalPhotoelectron Origin Y vs X for Rich1 ";

  m_hGlobalPEOriginXY = CurHistoSvc->book(m_RichG4HistoPath+"2",title,300,-2000.0,2000.0,300,-2000.0, 2000.0);
  if(!m_hGlobalPEOriginXY ) {
   RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }

  title="Global hit coordinate Y vs X for Rich1 ";

  m_hGlobalHitXY= CurHistoSvc->book(m_RichG4HistoPath+"3",title,300,-2000.0,2000.0,300,-2000.0, 2000.0);

   if(!m_hGlobalHitXY ) {
  RichG4Histolog << MSG::ERROR<<"Failed to book histo   "<<title<<endreq;

  }
   //  return StatusCode::SUCCESS;

}
