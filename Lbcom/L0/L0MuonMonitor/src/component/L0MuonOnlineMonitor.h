#ifndef COMPONENT_L0MUONONLINEMONITOR_H 
#define COMPONENT_L0MUONONLINEMONITOR_H 1

// Include files

#include "L0MuonMonitorBase.h"
#include "L0MuonPadsHistos.h"
#include "L0MuonChannelsHistos.h"
#include "L0MuonInfoHistos.h"
#include "L0MuonErrorHistos.h"
#include "L0MuonCandHistos.h"

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

  bool m_fullMonitoring;
  
  L0MuonPadsHistos*     m_padsHistos;
  L0MuonChannelsHistos* m_channelsHistos;
  L0MuonInfoHistos *    m_info;
  L0MuonErrorHistos *   m_error;
  L0MuonCandHistos*     m_candHistosFinal;
  L0MuonCandHistos*     m_candHistosPU;

};
#endif // COMPONENT_L0MUONONLINEMONITOR_H
