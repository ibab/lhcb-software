// $Id: L0MuonOnlineMonitor.h,v 1.2 2008-04-08 11:31:03 jucogan Exp $
#ifndef COMPONENT_L0MUONONLINEMONITOR_H 
#define COMPONENT_L0MUONONLINEMONITOR_H 1

// Include files

#include "L0MuonMonitorBase.h"
#include "LogicalPads2DMaps.h"
#include "PhysicalChannelsHist.h"

/** @class L0MuonOnlineMonitor L0MuonOnlineMonitor.h component/L0MuonOnlineMonitor.h
 *  
 *
 *  @author 
 *  @date   2008-04-02
 */
class L0MuonOnlineMonitor : public L0MuonMonitorBase {
public: 
  /// Standard constructor
  L0MuonOnlineMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonOnlineMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:

private:

  LogicalPads2DMaps*    m_padsMaps;
  PhysicalChannelsHist* m_channelHist;

  AIDA::IHistogram1D* m_decoding_status[4][3];

};
#endif // COMPONENT_L0MUONONLINEMONITOR_H
