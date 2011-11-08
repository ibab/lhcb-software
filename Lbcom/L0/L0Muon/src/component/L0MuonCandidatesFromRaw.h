#ifndef L0MUONTRIGGER_L0MUONCANDIDATESFROMRAW_H
#define L0MUONTRIGGER_L0MUONCANDIDATESFROMRAW_H     1

#include "L0Base/L0FromRawBase.h"
#include "L0MuonOutputs.h"

/** @class L0MuonCandidatesFromRaw L0MuonCandidatesFromRaw.h component/L0MuonCandidatesFromRaw.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2008-04-14
 */
class L0MuonCandidatesFromRaw : public L0FromRawBase {
    
public:

  L0MuonCandidatesFromRaw(const std::string& name, ISvcLocator* pSvcLocator);
  // ~L0MuonCandidatesFromRaw();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();
  StatusCode dumpErrors();
  
private:

  // Algorithm's properties
  int m_procVersion;                  // Emulator version
  std::string  m_configfile;          // Config file name
  int m_mode;                         // Output mode (defin the bank to be decoded)
  bool m_disableTAE;                  // Flag to disable the decoding of TAE event (obsolete, for backward compatibility)
  bool m_enableTAE;                   ///< Enable TAE mode
 
  // For output to RawEvent, TES or L0ProcessorDatas
  L0MuonOutputs* m_outputTool;

  // Counters for final report
  int m_totEvent ;  // Tot number of events processed
  int m_totBx ;     // Tot number of time slices processed
  int m_errorEvent ; // Number of events where there is a difference between the reprocessing and the raw data
  
  // TAE
  std::map<int,std::string> m_tae_items;  ///< Definitions of tae slots
    
};

#endif      // L0MUONTRIGGER_L0MUONCANDIDATESFROMRAW_H 
