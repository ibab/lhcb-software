// $Id: RichG4HistoDefineTimer.cpp,v 1.1 2003-04-29 17:08:05 seaso Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"


// local
#include "RichG4HistoDefineTimer.h"
#include "RichG4SvcLocator.h"
// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "CLHEP/Units/PhysicalConstants.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4HistoDefineTimer
//
// 2003-04-23 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4HistoDefineTimer::RichG4HistoDefineTimer(  ) {

  m_RichG4HistoPathTimer="RICHG4HISTOTIMER/";
  bookRichG4TimerHistograms();
  

}
RichG4HistoDefineTimer::~RichG4HistoDefineTimer( ) 
{; }
void RichG4HistoDefineTimer::bookRichG4TimerHistograms() 
{
  MsgStream RichG4Histolog(RichG4SvcLocator::RichG4MsgSvc(), 
                           "RichG4HistoTimer" );
  RichG4Histolog << MSG::INFO 
                 << "Now Booking Rich G4 Histo Timer" << endreq;
  std::string title;
  IHistogramSvc* CurHistoSvc = RichG4SvcLocator::RichG4HistoSvc();

   title="Total Real CPU Time per event for RICH ";

  m_timeWholeRichEvent= CurHistoSvc->book( m_RichG4HistoPathTimer+"1",
                             title,100,0.0,10.0);

   title="Total Real CPU Time per event for RICH Big Events";

   m_timeWholeRichMBEvent= CurHistoSvc->book( m_RichG4HistoPathTimer+"10",
                             title,100,0.0,500.0);

}

//=============================================================================
