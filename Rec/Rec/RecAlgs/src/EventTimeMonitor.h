#ifndef EVENTTIMEMONITOR_H 
#define EVENTTIMEMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"


/** @class EventTimeMonitor EventTimeMonitor.h
 *  
 *  Creates histograms of event time
 *
 *  @author Patrick Koppenburg
 *  @date   2012-04-19
 */
class EventTimeMonitor : public GaudiHistoAlg {
public: 
  /// Standard constructor
  EventTimeMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~EventTimeMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  /// the histogram definition (as property) 
  Gaudi::Histo1DDef m_histoS ;         // the histogram definition (as property) 
  Gaudi::Histo1DDef m_histoH ;         // the histogram definition (as property) 
  Gaudi::Histo1DDef m_histoD ;         // the histogram definition (as property) 
  Gaudi::Histo1DDef m_histoY ;         // the histogram definition (as property) 
  AIDA::IHistogram1D* m_plotS ;                // the histogram of seconds
  AIDA::IHistogram1D* m_plotH ;                // the histogram of hours
  AIDA::IHistogram1D* m_plotD ;                // the histogram of day of year
  AIDA::IHistogram1D* m_plotY ;                // the histogram of year
  std::vector<unsigned int> m_moffsets ; // month offsets. Isn't theer a method doing that?
};
#endif // EVENTTIMEMONITOR_H
