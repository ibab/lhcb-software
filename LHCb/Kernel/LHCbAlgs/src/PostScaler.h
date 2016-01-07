// $Id: PostScaler.h,v 1.1 2007-03-02 08:49:27 cattanem Exp $
#ifndef POSTSCALER_H
#define POSTSCALER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "Kernel/IEventCounter.h"

/** @class PostScaler PostScaler.h
 *  Allows to prescale events at a given rate suing a random number (as Prescaler.cpp)
 *  or to force the fraction of accepted events to be as given by options (or both).
 *
 *  @author P. Koppenburg
 *  @date   2004-06-15
 */
class PostScaler : public GaudiAlgorithm {
public:
  /// Standard constructor
  PostScaler( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PostScaler( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  long        m_nEvents;             ///< Counter of events accepted
  long        m_nEventsAll;          ///< Counter of events entering
  DoubleProperty m_percentPass;      ///< Minimal reduction rate to achieve (statistics mode)
  double      m_forcedReduction;     ///< Percentage of events that should be passed (random number mode)
  double      m_margin;              ///< Safety margin (accept if acc events < m_event/m_forcedReduction + m_margin)
  IEventCounter* m_eventCounter ;
};
#endif // CMT_POSTSCALER_H
