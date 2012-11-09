// $Id: $
// Include files 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"


// local
#include "RichG4HistoDefineSet5.h"
#include "GaussRICH/RichG4SvcLocator.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "globals.hh"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4HistoDefineSet5
//
// 2012-03-18 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4HistoDefineSet5::RichG4HistoDefineSet5(  ) {
  m_RichG4HistoHPathSet5="RICHG4HISTOSET5/";
  bookRichG4HistogramsSet5();
  
}
//=============================================================================
// Destructor
//=============================================================================
RichG4HistoDefineSet5::~RichG4HistoDefineSet5() {} 

//=============================================================================
void RichG4HistoDefineSet5::bookRichG4HistogramsSet5() {
  
  MsgStream RichG4Histolog(RichG4SvcLocator::RichG4MsgSvc(), 
                                         "RichG4HistoSet5" );
  RichG4Histolog << MSG::INFO << "Now Booking RichG4 histo    Set5" << endreq;
  std::string title;
  IHistogramSvc* CurHistoSvc = RichG4SvcLocator::RichG4HistoSvc();  
  title = "Overall Occupancy in RICH1 with HPDs";
 
  m_NumTotHitHRich1All = CurHistoSvc->book(m_RichG4HistoHPathSet5+"463",
                              title,1000,0.0,8000.0);
  title = "Overall Occupancy in RICH1 using HPDs  ";
 
  m_NumTotHitHRich1AllA = CurHistoSvc->book(m_RichG4HistoHPathSet5+"453",
                              title,1000,0.0,16000.0);

  title = "Overall Occupancy in RICH2 with HPDs";
 
  m_NumTotHitHRich2All = CurHistoSvc->book(m_RichG4HistoHPathSet5+"483",
                              title,1000,0.0,8000.0);
  title = "Overall Occupancy in RICH2 No Scintllation with HPDs";
 
  m_NumTotHitHRich2NoScintAll = CurHistoSvc->book(m_RichG4HistoHPathSet5+"484",
                              title,1000,0.0,8000.0);

  title = "Overall Occupancy in RICH2 using HPDs";
 
  m_NumTotHitHRich2AllA = CurHistoSvc->book(m_RichG4HistoHPathSet5+"473",
                              title,1000,0.0,16000.0);
  title = "Overall Occupancy in RICH2 No Scintllation, using HPDs";
 
  m_NumTotHitHRich2NoScintAllA = CurHistoSvc->book(m_RichG4HistoHPathSet5+"474",
                              title,1000,0.0,16000.0);



  title = "Overall Occupancy in RICH1 with HPDs  Binary Readout";
 
  m_NumTotHitHRich1AllBinary = CurHistoSvc->book(m_RichG4HistoHPathSet5+"454",
                              title,1000,0.0,8000.0);
  title = "Overall Occupancy in RICH2 with HPDs  Binary Readout";
 
  m_NumTotHitHRich2AllBinary = CurHistoSvc->book(m_RichG4HistoHPathSet5+"475",
                              title,1000,0.0,8000.0);
  title = "Overall Occupancy in RICH2 No Scintllation with HPDs  Binary Readout";
 
  m_NumTotHitHRich2NoScintAllBinary = CurHistoSvc->book(m_RichG4HistoHPathSet5+"476",
                              title,1000,0.0,8000.0);

  title="XY Location of Rich1 Gas HPD hits on HPD detector Plane";

  m_hXYHitLocationHRich1Gas=CurHistoSvc->book(m_RichG4HistoHPathSet5+"461",
                                             title,1000,-900.0,900.0,1000,-900.0,900.0);

  title="XY Location of Rich2 Gas HPD hits on HPD detector Plane";

  m_hXYHitLocationHRich2Gas=CurHistoSvc->book(m_RichG4HistoHPathSet5+"481",
                                             title,1000,-900.0,900.0,1000,-900.0,900.0);
  title="XY Location of Rich2 Gas HPD hits on HPD Detector Plane NoScintillation";

  m_hXYHitLocationHNoScintRich2Gas=CurHistoSvc->book(m_RichG4HistoHPathSet5+"482",
                                             title,1000,-900.0,900.0,1000,-900.0,900.0);


  title="XY Location of Rich1 Gas HPD hits on HPD detector Plane Binary Readout";

  m_hXYHitLocationHRich1GasBinary=CurHistoSvc->book(m_RichG4HistoHPathSet5+"451",
                                             title,1000,-900.0,900.0,1000,-900.0,900.0);

  title="XY Location of Rich2 Gas HPD hits on HPD detector Plane Binary Readout";

  m_hXYHitLocationHRich2GasBinary=CurHistoSvc->book(m_RichG4HistoHPathSet5+"471",
                                             title,1000,-900.0,900.0,1000,-900.0,900.0);

  title="XY Location of Rich2 Gas HPD hits on HPD Detector Plane NoScintillation, Binary Readout";

  m_hXYHitLocationHNoScintRich2GasBinary=CurHistoSvc->book(m_RichG4HistoHPathSet5+"472",
                                             title,1000,-900.0,900.0,1000,-900.0,900.0);



  title="Occupancy in Rich1 HPDs";
  
  m_hPmtOccpHRich1 =   CurHistoSvc->book(m_RichG4HistoHPathSet5+"465",
                                        title, 400,0.0,200.0);
  title="Occupancy in Rich2 HPDs";
  m_hPmtOccpHRich2 =   CurHistoSvc->book(m_RichG4HistoHPathSet5+"485",
                                        title, 600,0.0,300.0);
  title="Occupancy in Rich2 HPDs No Scintillation";
  m_hPmtOccpHNoScintRich2 =   CurHistoSvc->book(m_RichG4HistoHPathSet5+"486",
                                        title, 600,0.0,300.0);


  title="Occupancy in Rich1 HPDs with Binary Readout";
  
  m_hPmtOccpHRich1Binary =   CurHistoSvc->book(m_RichG4HistoHPathSet5+"455",
                                        title, 400,0.0,200.0);
  title="Occupancy in Rich2 HPDs  Binary Readout";
  m_hPmtOccpHRich2Binary =   CurHistoSvc->book(m_RichG4HistoHPathSet5+"477",
                                        title, 600,0.0,300.0);
  title="Occupancy in Rich2 HPDs No Scintillation, with  Binary Readout";
  m_hPmtOccpHNoScintRich2Binary =   CurHistoSvc->book(m_RichG4HistoHPathSet5+"478",
                                        title, 600,0.0,300.0);
  
}
