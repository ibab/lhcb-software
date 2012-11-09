// Include files 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"
//local
#include "GaussCherenkov/CkvG4SvcLocator.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "globals.hh"



// local
#include "CherenkovG4HistoDefineSet8.h"


//-----------------------------------------------------------------------------
// Implementation file for class : CherenkovG4HistoDefineSet8
//
// 2012-10-08 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CherenkovG4HistoDefineSet8::CherenkovG4HistoDefineSet8(  ) {

  m_CherenkovG4HistoHPathSet8= "RICHG4HISTOSET8/";
  
  BookCkvStepAnalysisSet8Histo();
  

}
//=============================================================================
// Destructor
//=============================================================================
CherenkovG4HistoDefineSet8::~CherenkovG4HistoDefineSet8() {} 

//=============================================================================
void CherenkovG4HistoDefineSet8::BookCkvStepAnalysisSet8Histo()
{
  MsgStream RichG4Histolog(CkvG4SvcLocator::RichG4MsgSvc(),"RichG4HistoSet8" );
  RichG4Histolog << MSG::INFO << "Now Booking Rich G4 Histo Set8" << endreq;
  
  IHistogramSvc* CurHistoSvc = CkvG4SvcLocator::RichG4HistoSvc();



    std::string title= "Rich1 XY coord on Mirror1 StepAction";

  m_CkvHisto_XY_Rich1Mirror1= CurHistoSvc->book(m_CherenkovG4HistoHPathSet8+"501",title,200,-1000.0,1000.0,200,-1000.0,1000.0);

  title= "Rich1 XY coord on Mirror2 StepAction"; 
  m_CkvHisto_XY_Rich1Mirror2= CurHistoSvc->book(m_CherenkovG4HistoHPathSet8+"503",title,200,-1000.0,1000.0,200,-2000.0,2000.0);
  title= "Rich1 XY coord on Mirror2 StepAction LocalTop"; 

  m_CkvHisto_XY_Rich1Mirror2LocalTop= CurHistoSvc->book(m_CherenkovG4HistoHPathSet8+"504",
                            title,200,-700.0,700.0,200,-1000.0,300.0);
  title= "Rich1 XY coord on Mirror2 StepAction LocalBot"; 
  m_CkvHisto_XY_Rich1Mirror2LocalBot= CurHistoSvc->book(m_CherenkovG4HistoHPathSet8+"505",title,
                 200,-700.0,700.0,200,-1000.0,300.0);

  title= "Rich1 XY coord on QW StepAction"; 

  m_CkvHisto_XY_Rich1QW = CurHistoSvc->book(m_CherenkovG4HistoHPathSet8+"516",title,200,-3000.0,3000.0,200,-3000.0,3000.0);

  title= "Rich1 XY coord on QW StepAction Local Top"; 
  m_CkvHisto_XY_Rich1QWLocalTop = CurHistoSvc->book(m_CherenkovG4HistoHPathSet8+"517",title,200,-500.0,500.0,200,-500.0,500.0);
  title= "Rich1 XY coord on QW StepAction Local Bot"; 
  m_CkvHisto_XY_Rich1QWLocalBot = CurHistoSvc->book(m_CherenkovG4HistoHPathSet8+"518",title,200,-500.0,500.0,200,-500.0,500.0);


  title= "Rich1 XY coord on PhotSupFrame  StepAction"; 

  m_CkvHisto_XY_Rich1PhSup = CurHistoSvc->book(m_CherenkovG4HistoHPathSet8+"506",title,200,-3000.0,3000.0,200,-3000.0,3000.0);


  title= "Rich1 XY coord on PhotSupFrame  StepAction Local  Top"; 
  m_CkvHisto_XY_Rich1PhSupLocalTop = CurHistoSvc->book(m_CherenkovG4HistoHPathSet8+"507",
                                                                  title,200,-700.0,700.0,200,-700.0,700.0);
  title= "Rich1 XY coord on PhotSupFrame  StepAction Local  Bot"; 
  m_CkvHisto_XY_Rich1PhSupLocalBot = CurHistoSvc->book(m_CherenkovG4HistoHPathSet8+"508",
                                                                title,200,-700.0,700.0,200,-700.0,-700.0);


  title= "Rich1 XY coord on Pmt Module Plane   StepAction"; 
  m_CkvHisto_XY_Rich1ModulePlaneTop =  CurHistoSvc->book(m_CherenkovG4HistoHPathSet8+"510",title,
                                                         200,-700.0,700.0,200,1200.0,1900.0);

  title= "Rich1 XY coord on Pmt Module Plane   StepAction"; 
  m_CkvHisto_XY_Rich1ModulePlaneBot =  CurHistoSvc->book(m_CherenkovG4HistoHPathSet8+"511",title,
                                                         200,-700.0,700.0,200,-1900.0,-1200.0);

  
}
