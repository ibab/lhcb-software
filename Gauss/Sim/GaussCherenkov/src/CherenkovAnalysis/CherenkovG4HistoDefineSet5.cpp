// $Id: $
// Include files 
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"



// local
#include "CherenkovG4HistoDefineSet5.h"
#include "GaussRICH/RichG4SvcLocator.h"
#include "CherenkovG4Counters.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "globals.hh"

//-----------------------------------------------------------------------------
// Implementation file for class : CherenkovG4HistoDefineSet5
//
// 2011-04-15 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CherenkovG4HistoDefineSet5::CherenkovG4HistoDefineSet5(  ) {
   m_RichG4HistoPathSet5="RICHG4HISTOSET5/";
   bookCherenkovG4HistogramsSet5();
   
}
//=============================================================================
// Destructor
//=============================================================================
CherenkovG4HistoDefineSet5::~CherenkovG4HistoDefineSet5() {} 

//=============================================================================
void CherenkovG4HistoDefineSet5::bookCherenkovG4HistogramsSet5() 
{
  
  MsgStream RichG4Histolog(RichG4SvcLocator::RichG4MsgSvc(), 
                                         "RichG4HistoSet5" );
  RichG4Histolog << MSG::INFO << "Now Booking Cherenkov histo    Set5" << endreq;
  std::string title;
  IHistogramSvc* CurHistoSvc = RichG4SvcLocator::RichG4HistoSvc();  
  title = "Overall Occupancy in RICH1 ";
 
  m_NumTotHitRich1All = CurHistoSvc->book(m_RichG4HistoPathSet5+"363",
                              title,1000,0.0,8000.0);
  title = "Overall Occupancy in RICH2 ";
 
  m_NumTotHitRich2All = CurHistoSvc->book(m_RichG4HistoPathSet5+"383",
                              title,1000,0.0,8000.0);
  title = "Overall Occupancy in RICH2 No Scintllation";
 
  m_NumTotHitRich2NoScintAll = CurHistoSvc->book(m_RichG4HistoPathSet5+"384",
                              title,1000,0.0,8000.0);

  title="XY Location of Rich1 Gas PMT hits on PMT Plane";

  //  m_hXYHitLocationRich1Gas=CurHistoSvc->book(m_RichG4HistoPathSet5+"361",
  //                                           title,1000,-1200.0,1200.0,1000,-1200.0,1200.0);
 m_hXYHitLocationRich1Gas=CurHistoSvc->book(m_RichG4HistoPathSet5+"361",
                                             title,1000,-900.0,900.0,1000,-900.0,900.0);
  title="XY Location of Rich2 Gas PMT hits on PMT Plane";

  m_hXYHitLocationRich2Gas=CurHistoSvc->book(m_RichG4HistoPathSet5+"381",
                                             title,1000,-900.0,900.0,1000,-900.0,900.0);
  title="XY Location of Rich2 Gas PMT hits on PMT Plane NoScintillation";

  m_hXYHitLocationNoScintRich2Gas=CurHistoSvc->book(m_RichG4HistoPathSet5+"382",
                                             title,1000,-900.0,900.0,1000,-900.0,900.0);
  title="Occupancy in Rich1 Pmts";
  
  m_hPmtOccpRich1 =   CurHistoSvc->book(m_RichG4HistoPathSet5+"365",
                                        title, 4000,0.0,2000.0);
  title="Occupancy in Rich2 Pmts";
  m_hPmtOccpRich2 =   CurHistoSvc->book(m_RichG4HistoPathSet5+"385",
                                        title, 6400,0.0,3200.0);
  title="Occupancy in Rich2 Pmts No Scintillation";
  m_hPmtOccpNoScintRich2 =   CurHistoSvc->book(m_RichG4HistoPathSet5+"386",
                                        title, 6400,0.0,3200.0);
  

  title="XY Location of Rich1 Gas PMT hits on Mirror1 ";
    
  m_XYHitLocationRich1GasMirror1 = CurHistoSvc->book(m_RichG4HistoPathSet5+"366",title,
                                               1000, -1000.0, 1000.0, 1000,-1000.0, 1000.0);
 
  title="XY Location of Rich1 Gas PMT hits on Mirror2 ";
    
  m_XYHitLocationRich1GasMirror2 = CurHistoSvc->book(m_RichG4HistoPathSet5+"368",title,
                                                1000, -1000.0, 1000.0,1000,-1000.0, 1000.0);
 
 
}

