#ifndef L0MUONTRIGGER_L0MUONCANDIDATESFROMRAW_H
#define L0MUONTRIGGER_L0MUONCANDIDATESFROMRAW_H     1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "L0MuonOutputs.h"

class L0MuonCandidatesFromRaw : public GaudiAlgorithm {
    
public:

  L0MuonCandidatesFromRaw(const std::string& name, ISvcLocator* pSvcLocator);
  // ~L0MuonCandidatesFromRaw();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();
    
private:

  // Algorithm's properties
  std::string m_extension;            // Extension to the location of the L0MuonCandidates
  int m_procVersion;                  // Emulator version
  std::vector<double> m_ptParameters; // Pt calculation parameters
  std::string  m_configfile;          // Config file name

  // For output to RawEvent, TES or L0ProcessorDatas
  L0MuonOutputs* m_outputTool;

  // Counters for final report
  int m_totEvent ;  // Tot number of events processed
  int m_errorEvent ; // Number of events where there is a difference between the reprocessing and the raw data
  
};

#endif      // L0MUONTRIGGER_L0MUONCANDIDATESFROMRAW_H 
