// $Id: L0DURawBankMonitor.h,v 1.13 2010-05-26 10:46:21 odescham Exp $
#ifndef L0DURAWBANKMONITOR_H 
#define L0DURAWBANKMONITOR_H 1

// Include files
// Interfaces
#include "L0Interfaces/IL0DUFromRawTool.h"
#include "L0Interfaces/IL0CondDBProvider.h"
#include "L0Interfaces/IL0DUEmulatorTool.h"
#include "GaudiKernel/IEventTimeDecoder.h"
#include "Event/RawBankReadoutStatus.h"
#include "CaloDAQ/ICaloDataProvider.h"
#include <TH1D.h>
#include <TH2D.h>
#include <TAxis.h>
// Base class
#include "CaloUtils/Calo2Dview.h"

/** @class L0DURawBankMonitor L0DURawBankMonitor.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-03-03
 */

namespace L0DUBase{
  namespace L0DUError{
    enum Type{ IdleLink      = 0,
               TLK           = 1,
               DeMux         = 2,
               BxPGAShift    = 3,
               BxOdinShift   = 4,
               InputData     = 5,
               EmulatorCheck = 6,
               UnknownTCK    = 7,
               WrongSpdMult  = 8,
               MissL0Muon    = 9,
               MissL0Calo    = 10,
               MissL0PU      = 11
    };
  }
}


class L0DURawBankMonitor : public Calo2Dview {
public: 
  /// Standard constructor
  L0DURawBankMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0DURawBankMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


  

protected:

private:
  void bookHistos();
  bool emulatorCheck(LHCb::L0DUConfig* config, int unit=1, std::string txt="");

  std::string m_fromRawTool;
  std::string m_emulatorTool;
  IL0DUFromRawTool*  m_fromRaw;
  IL0DUEmulatorTool* m_emuTool;
  IL0CondDBProvider* m_condDB;  
  IEventTimeDecoder* m_odin;
  ICaloDataProvider* m_spd;
  bool m_conv;
  bool m_data;
  bool m_proc;
  bool m_status;
  bool m_addr;
  bool m_full;
  int m_bin;
  std::vector<std::vector<std::string> > m_locs;
  bool m_decode;
  bool m_warn;
  std::string m_lab;
  bool m_generic;
  std::string m_caloTool;
  GaudiAlg::HistoID toHistoID(std::string unit);
  bool m_first;
  int m_mask;
  bool m_bcidDet;
  std::vector<std::string> m_rawLocations;
};
#endif // L0DURAWBANKMONITOR_H
