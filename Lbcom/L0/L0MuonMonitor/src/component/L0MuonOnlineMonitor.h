#define _ERROR_V2_ 1
// $Id: L0MuonOnlineMonitor.h,v 1.5 2008-07-25 14:42:59 jucogan Exp $
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

  L0MuonPadsHistos*     m_padsHistos;
  L0MuonChannelsHistos* m_channelsHistos;
  L0MuonInfoHistos *    m_info;
#if _ERROR_V2_
  L0MuonErrorHistos *   m_error;
#else  
  // ---- TMP ---- WILL NOT BE USED WITH BANK V2
  AIDA::IHistogram1D* m_decoding_status[4][3];
  // ---- END TMP ---- WILL NOT BE USED WITH BANK V2
#endif
  L0MuonCandHistos*     m_candHistosFinal;
  L0MuonCandHistos*     m_candHistosPU;

};
#endif // COMPONENT_L0MUONONLINEMONITOR_H
