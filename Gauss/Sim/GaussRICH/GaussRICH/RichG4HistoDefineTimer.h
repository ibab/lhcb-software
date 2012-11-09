// $Id: RichG4HistoDefineTimer.h,v 1.2 2003-07-16 13:24:07 seaso Exp $
#ifndef GAUSSRICH_RICHG4HISTODEFINETIMER_H 
#define GAUSSRICH_RICHG4HISTODEFINETIMER_H 1

// Include files
#include <string>
#include <cmath>
#include <vector>
#include "GaudiKernel/ISvcLocator.h"

// Forward declarations
class IHistogram1D;
class IHistogram2D;


/** @class RichG4HistoDefineTimer 
 * RichG4HistoDefineTimer.h 
 * RichAnalysis/RichG4HistoDefineTimer.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2003-04-23
 */
class RichG4HistoDefineTimer {
public:
  /// Standard constructor
  RichG4HistoDefineTimer( ); 

  virtual ~RichG4HistoDefineTimer( ); ///< Destructor

  void bookRichG4TimerHistograms();
  
protected:

private:
  IHistogram1D*         m_timeWholeRichEvent;
  IHistogram1D*         m_timeWholeRichMBEvent;
  IHistogram1D*         m_timeWholeRichSmallMBEvent;
  std::string m_RichG4HistoPathTimer; ///< Histo path
  ISvcLocator* m_svcLocTimer;
  

};
#endif // GAUSSRICH_RICHG4HISTODEFINETIMER_H
