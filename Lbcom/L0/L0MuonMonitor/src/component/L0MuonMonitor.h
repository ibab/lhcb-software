// $Id: L0MuonMonitor.h,v 1.1 2008-04-08 11:31:03 jucogan Exp $
#ifndef COMPONENT_L0MUONMONITOR_H 
#define COMPONENT_L0MUONMONITOR_H 1

// Include files

#include "MuonDAQ/IMuonRawBuffer.h"

#include "L0MuonMonitorBase.h"
#include "LogicalPads2DMaps.h"
#include "PhysicalChannelsHist.h"

/** @class L0MuonMonitor L0MuonMonitor.h component/L0MuonMonitor.h
 *  
 *
 *  @author 
 *  @date   2008-04-08
 */
class L0MuonMonitor : public L0MuonMonitorBase {
public: 
  /// Standard constructor
  L0MuonMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  PhysicalChannelsHist* m_channelHist_l0muon;
  PhysicalChannelsHist* m_channelHist_muon;

  IMuonRawBuffer* m_muonBuffer;

};
#endif // COMPONENT_L0MUONMONITOR_H
