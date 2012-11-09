// $Id: RichG4HistoFillTimer.h,v 1.1 2003-04-29 17:08:05 seaso Exp $
#ifndef GAUSSRICH_RICHG4HISTOFILLTIMER_H 
#define GAUSSRICH_RICHG4HISTOFILLTIMER_H 1

// Include files
#include "G4Timer.hh"

/** @class RichG4HistoFillTimer 
 * RichG4HistoFillTimer.h 
 * RichAnalysis/RichG4HistoFillTimer.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2003-04-23
 */
class IHistogramSvc;

class RichG4HistoFillTimer {
public:
  /// Standard constructor
  RichG4HistoFillTimer( ); 

  virtual ~RichG4HistoFillTimer( ); ///< Destructor

  G4Timer* timerRichEvent() 
  {
    return m_timerRichEvent;
  }

  
  void RichG4BeginEventTimer();
  

  void RichG4EndEventTimer();

  
  
protected:

private:

  G4Timer* m_timerRichEvent;

  IHistogramSvc* m_currentHistoSvc;
  
};
#endif // GAUSSRICH_RICHG4HISTOFILLTIMER_H
