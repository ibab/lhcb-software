// $Id: TrackMonitor.h,v 1.1 2007-05-08 06:49:00 mneedham Exp $
#ifndef TRACKMONITOR_H
#define TRACKMONITOR_H 1
 
// Include files
 
// from Gaudi
#include "TrackMonitorBase.h"


/** @class TrackMonitor TrackMonitor.h "TrackCheckers/TrackMonitor"
 * 
 * Class for track monitoring
 *  @author M. Needham.
 *  @date   6-5-2007
 */                 
                                                           
class TrackMonitor : public TrackMonitorBase {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  TrackMonitor( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~TrackMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm execute */
  virtual StatusCode execute();

 private:

  void fillHistograms(const LHCb::Track* aTrack);
  double m_refZ;
  double m_xMax;
  double m_yMax;

};

#endif // TRACKMONITOR_H
