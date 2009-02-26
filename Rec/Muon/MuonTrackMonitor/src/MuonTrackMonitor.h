// $Id: MuonTrackMonitor.h,v 1.1.1.1 2009-02-26 09:29:59 ggiacomo Exp $
#ifndef MUONTRACKMONITOR_H 
#define MUONTRACKMONITOR_H 1

#include "GaudiAlg/GaudiHistoAlg.h"


/** @class MuonTrackMonitor MuonTrackMonitor.h
 *  
 *
 *  @author Giacomo GRAZIANI
 *  @date   2009-02-25
 */
class MuonTrackMonitor : public GaudiHistoAlg {
public: 
  MuonTrackMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonTrackMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};
#endif // MUONTRACKMONITOR_H
