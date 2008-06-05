#ifndef L0MUONTRIGGER_L0MUONALG_H
#define L0MUONTRIGGER_L0MUONALG_H     1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "MuonKernel/MuonSystemLayout.h"

#include "ProcessorKernel/Property.h"
#include "ProcessorKernel/Unit.h"

#include "MuonDAQ/IMuonRawBuffer.h"

#include "L0MuonOutputs.h"

#include "L0MuonKernel/MuonCandidate.h"

/** @class L0MuonAlg L0MuonAlg.h component/L0MuonAlg.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2008-01-10
 */

class L0MuonAlg : public GaudiAlgorithm {
    
public:

  L0MuonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  // ~L0MuonAlg();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

    
private:

  void setLayouts(); // Set the layouts to be used in fillOLsfromCoords
  std::map<std::string,L0Muon::Property>  l0MuonProperties(); // Build the properties of L0MuonKernel Units

  StatusCode fillOLsfromDigits(); // Fill the Optical Links before processing

  // Emulator running modes
  int m_procVersion;                 // Version bank to write
  int m_bankVersion;                 // Version bank to write
  int m_outputMode;                  // Output mode (0=light, 1=standard, 2=full)
  // Emulator properties
  std::vector<int> m_foiXSize;        // values of FoI's in X
  std::vector<int> m_foiYSize;        // values of FoI's in Y
  std::vector<double> m_ptParameters; // Pt calculation parameters
  std::string  m_configfile;          // Config file name
  bool m_ignoreM1;                    // Flag to use M1 or not (not tested)
  bool m_ignoreM2;                    // Flag to use M2 or not (not tested)
  bool m_forceM3;                     // Flag to force M3 optical link content to 1
  bool m_debug;                       // Flag to turn on debug mode for L0MuonKernel
  
  // Algorithm's properties
  std::string m_extension;            // Extension to the location of the L0MuonCandidates
  bool m_writeL0ProcData;             // Flag to activate the writing of the L0ProcessorData for the L0DU
  bool m_writeOnTES;                  // Flag to activate the writing of the L0MuonCandidates 
  bool m_storeBank;                   // Flag to enable/disable event storage in raw banks
  int  m_inputSource;                 // Specify where to take the input data for the processing
                                      //  - 0: from Muon output
                                      //  - 1: from the input of the processor (extracted form L0Muon itself) 

  // For trigger emulation
  L0Muon::Unit*  m_muontriggerunit; // Top Unit of the L0Muon emulator

  // For output to RAwEvent, TES ot L0ProcessorDatas
  L0MuonOutputs* m_outputTool;

  MuonSystemLayout m_layout;   // pad layout for the whole MuonSystem
  MuonSystemLayout m_lulayout; // logical unit layout for the whole MuonSystem 
  MuonSystemLayout m_ollayout; // optical link layout for the whole MuonSystem
  MuonSystemLayout m_stripH;   // vertical strip layout for the whole MuonSystem
  MuonSystemLayout m_stripV;   // horizontal strip layout for the whole MuonSystem

  int m_totEvent;
  
 // Interface to muon raw buffer 
  IMuonRawBuffer* m_muonBuffer; 

  static inline std::string timeSlot(int bx)  {
    std::string ts;
    switch (bx) {
    case -7 : return "Prev7/";
    case -6 : return "Prev6/";
    case -5 : return "Prev5/";
    case -4 : return "Prev4/";
    case -3 : return "Prev3/";
    case -2 : return "Prev2/";
    case -1 : return "Prev1/";
    case  0 : return "";
    case  1 : return "Next1/";
    case  2 : return "Next2/";
    case  3 : return "Next3/";
    case  4 : return "Next4/";
    case  5 : return "Next5/";
    case  6 : return "Next6/";
    case  7 : return "Next7/";
    default : return "Unknown";
    };

  };

  std::vector<int> m_time_slots;

};

#endif      // L0MUONTRIGGER_L0MUONALG_H
