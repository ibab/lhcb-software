// $Id: MuonEfficiencyMonitor.h,v 1.1.1.1 2009-02-26 09:29:59 ggiacomo Exp $
#ifndef MUONTRACKMONITOR_H 
#define MUONTRACKMONITOR_H 1

#include "GaudiAlg/GaudiHistoAlg.h"


/** @class MuonEfficiencyMonitor MuonTrackMonitor.h
 *  
 *
 *  @author 
 *  @date   2009-02-25
 */
class MuonEfficiencyMonitor : public GaudiHistoAlg {
public: 
  MuonEfficiencyMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonEfficiencyMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};
#endif // MUONTRACKMONITOR_H
