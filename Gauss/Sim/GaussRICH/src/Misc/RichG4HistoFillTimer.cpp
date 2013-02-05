// $Id: RichG4HistoFillTimer.cpp,v 1.2 2003-07-16 13:24:07 seaso Exp $
// Include files 


#include "globals.hh"

// local
#include "GaussRICH/RichG4HistoFillTimer.h"
/// GaudiKernel
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "GaussRICH/RichG4SvcLocator.h"

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
   SmartDataPtr<IHistogram1D>htimeWholeRichMBSmallEvent
                (m_currentHistoSvc,"RICHG4HISTOTIMER/20");
   if(htimeWholeRichEvent)htimeWholeRichEvent->
                 fill( TimeforThisRichEvent);
   // file only when the time is more than 1 second.

   if( TimeforThisRichEvent > 1.0 ) {
     
   if(htimeWholeRichMBEvent)htimeWholeRichMBEvent->
                 fill(TimeforThisRichEvent);
   if(htimeWholeRichMBSmallEvent)htimeWholeRichMBSmallEvent->
                 fill(TimeforThisRichEvent);
   
   }
   
}

//=============================================================================
