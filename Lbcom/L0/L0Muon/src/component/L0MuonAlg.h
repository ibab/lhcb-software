#ifndef L0MUONTRIGGER_L0MUONALG_H
#define L0MUONTRIGGER_L0MUONALG_H     1



#include "GaudiAlg/GaudiAlgorithm.h"
#include "MuonKernel/MuonSystemLayout.h"
#include "L0MuonKernel/MuonTriggerUnit.h"

#include "MuonDAQ/IMuonRawBuffer.h"

//class IMuonTileXYZTool; 
class L0MuonAlg : public GaudiAlgorithm {
    
public:

  L0MuonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  // ~L0MuonAlg();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

    
private:

  void setLayouts();              // Set the layouts to be used in fillOLsfromCoords
  int setL0MuonProperties();      // Set the properties of L0MuonKernel Units

  StatusCode fillOLsfromDigits(); // Fill the Optical Links before processing
  int fillRawEvent();             // Fill the RawEvent container
  int fillL0ProcessorData();      // Fill the L0ProcessorData container

  std::string dumpCandidates();   // Print out the candidate sent to L0DU
  std::string dumpRawRegisters(); // Print out the RawBuffer registers

  // Algorithme's properties
  std::vector<int> m_foiXSize;        // values of FoI's in X
  std::vector<int> m_foiYSize;        // values of FoI's in Y
  std::vector<double> m_ptParameters; // Pt calculation parameters
  std::string  m_configfile;          // Config file name
  bool m_ignoreM1;                    // Flag to use M1 or not (latter case not tested)
  bool m_debug;                       // Flag to turn on debug mode for L0MuonKernel
  bool m_storeFlag;                   // Flag to enable/disable event storage in raw banks
  bool m_zip;                         // Flag to turn on the compression of the RawBuffer registers
  std::string m_l0BufPath;            // Directory where to write the L0Buffers

  // For trigger emulation
  L0Muon::MuonTriggerUnit*  m_muontriggerunit; // Top Unit of the L0Muon emulator

  MuonSystemLayout m_layout;   // pad layout for the whole MuonSystem
  MuonSystemLayout m_lulayout; // logical unit layout for the whole MuonSystem 
  MuonSystemLayout m_ollayout; // optical link layout for the whole MuonSystem
  MuonSystemLayout m_stripH;   // vertical strip layout for the whole MuonSystem
  MuonSystemLayout m_stripV;   // horizontal strip layout for the whole MuonSystem

  //  IMuonTileXYZTool *m_iTileXYZTool;// this is a tool to get coordinates from ID

  // Counters for final report
  int m_totNumCand; // Tot number of candidates found
  int m_totTrigEvt; // Tot number of events with at least a candidate
  int m_totRawSize; // Total size of the raw buffer
  int m_totEvent ;  // Tot number of events processed
  
  // Interface to muon raw buffer 
  IMuonRawBuffer* m_muonBuffer; 
};

#endif      // L0MUONKERNEL_CRATE_H  
