// $Id: $
// Include files 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "globals.hh"




// local
#include "TorchTBG4DefineHistSet6.h"
#include "RichG4SvcLocator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TorchTBG4DefineHistSet6
//
// 2012-06-01 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TorchTBG4DefineHistSet6::TorchTBG4DefineHistSet6(  ) {

 m_TorchTBHistoPathSet6="TORCHTBG4HISTOSET6/";

 BookTorchTBCherenkovG4HistogramSet6();
 
}
//=============================================================================
// Destructor
//=============================================================================
TorchTBG4DefineHistSet6::~TorchTBG4DefineHistSet6() {} 

//=============================================================================
void TorchTBG4DefineHistSet6::BookTorchTBCherenkovG4HistogramSet6() {
   MsgStream TorchG4Histolog(RichG4SvcLocator::RichG4MsgSvc(), 
                                         "TorchTBG4HistoSet6" );
  TorchG4Histolog << MSG::INFO << "Now Booking TorchTB histo    Set6" << endreq;
 
  std::string title;
  IHistogramSvc* CurHistoSvc = RichG4SvcLocator::RichG4HistoSvc();  
  title = "Overall Number of Hits in TorchTB ";
  
  m_histoNumTotHitInTestBeam = CurHistoSvc->book(m_TorchTBHistoPathSet6+ "1001",
                                            title,100,0.0, 200.0);
  

  title = "Number of Hits in TorchTB Mcp0  ";

  m_histoNumTotHitMcp0= CurHistoSvc->book(m_TorchTBHistoPathSet6+ "1002",
                                     title,100,0.0,100.0);
  
  title = "Number of Hits in TorchTB Mcp1  ";

  m_histoNumTotHitMcp1= CurHistoSvc->book(m_TorchTBHistoPathSet6+ "1003",
                                     title,100,0.0,100.0);

  title =" Z location of Mcp0 Hits ";
  
  m_histoHitZLocationMcp0= CurHistoSvc->book(m_TorchTBHistoPathSet6+ "1010", 
                                        title,100,10800.0, 10900.0);
  

  
}
