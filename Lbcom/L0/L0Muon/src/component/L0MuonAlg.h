#ifndef L0MUONTRIGGER_L0MUONALG_H
#define L0MUONTRIGGER_L0MUONALG_H     1

#include "L0Base/L0AlgBase.h"

#include "MuonKernel/MuonSystemLayout.h"
#include "ProcessorKernel/Property.h"
//#include "ProcessorKernel/Unit.h"
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/L0MPtLUT.h"

// Interface
#include "L0Interfaces/IL0DUConfigProvider.h"
#include "MuonDAQ/IMuonRawBuffer.h"
#include "L0Interfaces/IL0MuonModifyInputTool.h"

#include "L0MuonOutputs.h"

#include "L0MuonKernel/MuonCandidate.h"

#include "DetDesc/Condition.h"

/** @class L0MuonAlg L0MuonAlg.h component/L0MuonAlg.h

 Algorithm to run the L0Muon emulator.

 Algorithm main properties :

 Properties inherited from L0AlgBase
  - WriteBanks      : write the raw banks (default is true)
  - WriteOnTES      : write on TES the L0MuonCandidates (default is false)
  - L0Context       : string added to the L0MuonCandidates location on TES

 Additional properties :
  - InputSource     : 0 => use MuonDigits in input / 1 => use L0MuonData in input (default is 0)
  - DAQMode         : TELL1 output mode : 0 => Light / 1 => Standard / 2 => Debug (default is 1)  
  - Compression     : flag to activate the compression of the output banks (default is True)
  - ConfigFile      : path of the xml configuration file
  - IgnoreM1        : run without M1
  - ForceM3         : force input M3 optical links content to 1
  - DebugMode       : flag to activate the debug mode of the emulator
  - IgnoreCondDB    : flag not to use the CondDB (default is False)

 Properties overloaded with CondDB parameters 
  - Version  : version of the processor to emulate 
  - FoiXSize : vectors of integer specifying the size of the fields of interest in X. 
  - FoiXSize : vectors of integer specifying the size of the fields of interest in Y. 
      
   @author Julien Cogan
   @date   2008-01-10

 */

class L0MuonAlg : public L0AlgBase {
    
public:

  L0MuonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  // ~L0MuonAlg();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();
    
private:

  void setLayouts(); ///< Set the layouts to be used in fillOLsfromCoords
  std::map<std::string,L0Muon::Property>  l0MuonProperties(); ///< Build the properties of L0MuonKernel Units

  StatusCode getDigitsFromMuon();   ///< Get the hits from the muon (Digits or ZS raw data)
  StatusCode getDigitsFromMuonNZS();///< Get the hits from the muon data (Non Zero supp.)
  StatusCode getDigitsFromL0Muon(); ///< Get the hits from the L0Muon data
  StatusCode fillOLsfromDigits();   ///< Fill the Optical Links before processing
  
  // Emulator input
  std::vector<LHCb::MuonTileID> m_digits; ///< Hits used by the emulator

  // CondDB
  bool m_ignoreCondDB;                ///< Flag to ignore the CondDB 
  std::string m_conditionNameVersion; ///< Name of the condition of the Controller board
  std::string m_conditionNameFOI;     ///< Name of the condition of the Processing board
  std::string m_parameterNameFOIx;    ///< Name of the parameter with FOIx in CondDB  
  std::string m_parameterNameFOIy;    ///< Name of the parameter with FOIy in CondDB  
  std::string m_parameterNameVersion; ///< Name of the parameter with emulator version in CondDB  

  // TCK
  IL0DUConfigProvider* m_confTool;
  std::string m_configName;           ///< L0DUConfigProviderName tool name
  std::string m_configType;           ///< L0DUConfigProviderType tool name
  std::string m_tck;                  ///< TCK (used to get the FOIs) 
  int m_itck;                         ///< TCK (used to get the FOIs) 
  bool m_useTCKFromData;              ///< Flag to take the TCK from data (odin)
  
  // Emulator running modes
  int m_version;                      ///< Emulator version 
  int m_mode;                         ///< Banks output mode (0=light, 1=standard, 2=full)
  bool m_compression;                 ///< Apply compression when writing banks

  // Emulator properties
  std::vector<int> m_foiXSize;        ///< values of FoI's in X 
  std::vector<int> m_foiYSize;        ///< values of FoI's in Y
  std::string  m_configfile;          ///< Config file name
  bool m_ignoreM1;                    ///< Flag to use M1 or not (not tested)
  bool m_forceM3;                     ///< Flag to force M3 optical link content to 1
  bool m_debug;                       ///< Flag to turn on debug mode for L0MuonKernel
  
  // Algorithm's properties
  bool m_enableTAE;                   ///< Enable TAE mode 
  int  m_inputSource;                 ///< Specify where to take the input data for the processing
                                      ///<  - 0: from Muon output
                                      ///<  - 1: from the input of the processor (extracted form L0Muon itself) 

  bool m_modify_digits;               ///< Modify the input data for special studies
  
  // For trigger emulation
  //L0Muon::Unit*  m_muontriggerunit; ///< Top Unit of the L0Muon emulator
  L0Muon::L0MUnit*  m_muontriggerunit; ///< Top Unit of the L0Muon emulator

  L0MuonOutputs* m_outputTool; ///< For output to RAwEvent, TES ot L0ProcessorDatas

  MuonSystemLayout m_layout;   ///< pad layout for the whole MuonSystem
  MuonSystemLayout m_lulayout; ///< logical unit layout for the whole MuonSystem 
  MuonSystemLayout m_ollayout; ///< optical link layout for the whole MuonSystem
  MuonSystemLayout m_stripH;   ///< vertical strip layout for the whole MuonSystem
  MuonSystemLayout m_stripV;   ///< horizontal strip layout for the whole MuonSystem

  int m_totEvent; ///< Event counter
  int m_totBx ;   ///< Tot number of time slices processed

  IMuonRawBuffer* m_muonBuffer;  ///< Interface to muon raw buffer 

  Condition * m_l0CondCtrl ;  ///< Pointer to the L0 Controller board conditions 
  Condition * m_l0CondProc ;  ///< Pointer to the L0 Processing board conditions

  std::string m_lut_path;     ///< Directory containing the LUT file
  std::string m_lut_basename; ///< BaseName of the LUT file 
  std::string m_lut_version;  ///< Version of LUT to be used (appended to the basename)
  L0MPtLUT * m_lut;             ///< LUT object

  IL0MuonModifyInputTool * m_modifyInputTool;
  std::string m_modifyInputToolType;
  std::string m_modifyInputToolName;
  
  /// Call back function to check the Version condition database content  
  StatusCode updateL0CondVersion() ;

  /// Call back function to check the FOI condition database content  
  StatusCode updateL0CondFOI() ;

  /// Function to check the FOI in the TCK configuration
  StatusCode updateL0TCKFOI() ;

  static inline int hard2softFOIConversion(int sta) 
  {
    switch (sta) {
    case 0: return 2; // M1
    case 1: return 1; // M2
    case 3: return 4; // M4
    case 4: return 4; // M5
    default : return 1;
    }
  }

  // TAE
  std::map<int,std::string> m_tae_items;  ///< Definitions of tae slots


};

#endif      // L0MUONTRIGGER_L0MUONALG_H
