// $Id: RichG4HistoFillTimer.cpp,v 1.1 2003-04-29 17:08:05 seaso Exp $
// Include files 


#include "globals.hh"

// local
#include "RichG4HistoFillTimer.h"
/// GaudiKernel
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "RichG4SvcLocator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4HistoFillTimer
//
// 2003-04-23 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4HistoFillTimer::RichG4HistoFillTimer(  ):m_timerRichEvent(0),
                                               m_currentHistoSvc
  (RichG4SvcLocator::RichG4HistoSvc())

{

  m_timerRichEvent= new G4Timer();
  //  m_currentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();
 

}
RichG4HistoFillTimer::~RichG4HistoFillTimer(  )
{;}
void RichG4HistoFillTimer::RichG4BeginEventTimer() 
{
  m_timerRichEvent->Start();
  
  
}

void RichG4HistoFillTimer::RichG4EndEventTimer()
{
  m_timerRichEvent->Stop();
  
  G4double TimeforThisRichEvent=m_timerRichEvent-> GetRealElapsed();
 
  // Now to find and fill the histogram
   SmartDataPtr<IHistogram1D>htimeWholeRichEvent
                 (m_currentHistoSvc,"RICHG4HISTOTIMER/1");
   SmartDataPtr<IHistogram1D>htimeWholeRichMBEvent
                (m_currentHistoSvc,"RICHG4HISTOTIMER/10");
   if(htimeWholeRichEvent)htimeWholeRichEvent->
                 fill( TimeforThisRichEvent);
   if(htimeWholeRichMBEvent)htimeWholeRichMBEvent->
                 fill(TimeforThisRichEvent);
   
   
}

//=============================================================================
