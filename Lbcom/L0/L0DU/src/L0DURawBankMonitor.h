// $Id: L0DURawBankMonitor.h,v 1.2 2008-05-29 14:01:15 odescham Exp $
#ifndef L0DURAWBANKMONITOR_H 
#define L0DURAWBANKMONITOR_H 1

// Include files
// Interfaces
#include "L0Interfaces/IL0DUFromRawTool.h"
#include "L0Interfaces/IL0CondDBProvider.h"
#include "L0Interfaces/IL0DUEmulatorTool.h"
#include "Kernel/IEventTimeDecoder.h"
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
    enum Type{ Tell1         = 0,
               DeMux         = 1,
               TLK           = 2,
               IdleLink      = 3,
               BxPGAShift    = 4,
               BxOdinShift   = 5,
               InputData     = 6,
               EmulatorCheck = 7

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
  IL0DUFromRawTool*    m_fromRaw;
  IL0DUEmulatorTool*     m_emuTool;
  IL0CondDBProvider*   m_condDB;  
  IEventTimeDecoder* m_odin;

  bool m_conv;
  bool m_data;
  bool m_proc;
  bool m_status;
  bool m_addr;
  bool m_full;
  int m_bin;
  std::vector<std::vector<std::string> > m_locs;
};
#endif // L0DURAWBANKMONITOR_H
