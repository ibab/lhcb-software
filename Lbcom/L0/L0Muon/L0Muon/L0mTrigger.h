// $Id: L0mTrigger.h,v 1.4 2002-11-04 08:38:19 atsareg Exp $

#ifndef L0MUON_L0MTRIGGER_H
#define L0MUON_L0MTRIGGER_H 1

#include "GaudiKernel/Algorithm.h"
// Private include files
#include "L0Muon/L0mCrate.h"
#include "Event/Kernel.h"
#include "MuonKernel/MuonSystemLayout.h"
#include "Event/MuonDigit.h"
#include "Event/MuonCoord.h"

/** @class L0mTrigger L0mTrigger.h L0mTrigger.h 

   L0Muon trigger processor detailed algorithm
   
   @author Andrei Tsaregorodtsev
   @date 6 April 2001 
*/

class L0mPad;
class IMuonTileXYZTool;   

class L0mTrigger : public Algorithm  {

public:
  /// Constructor: A constructor of this form must be provided.
  L0mTrigger(const std::string& name, ISvcLocator* pSvcLocator); 

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
  friend class L0mCrate;
    
  std::vector<L0mTower*>* getTowers() {return &m_towers; } 
  
private: 
 
  std::vector<L0mCrate*> m_crates;
  
  /// Create a tower starting from a pad in M3
  L0mTower* createTower(L0mPad* pad, const std::vector<L0mPad>& pads); 
  /// Make L0mPads from MuonDigits 
  StatusCode makePadsFromDigits();
  /// Make L0mPads from MuonCoords 
  StatusCode makePadsFromCoords();
  std::vector<int> m_foiXSize;         // values of FoI's in X
  std::vector<int> m_foiYSize;         // values of FoI's in Y
  std::vector<double> m_ptParameters;  // Pt calculation parameters
  std::vector<int> m_extraM1;          // parameters for extrapolation to M1
  std::string m_outputCandidates; // candidates location in the transient store
  std::string m_inputPads;
  std::string m_mode;  
  double m_precision;    // Bin width for Pt value encoding
  int m_bins;            // Number of bins for Pt value encoding
  bool m_limitedY;                // flag to use limitedY mode
  bool m_ignoreM1;                // flag to use ignore M1 mode
  bool m_inputFromMuonCoords;     // flag to take input from MuonCoord's
  bool m_seedClustering;          // flag to perform seed clustering
  
  std::vector<L0mTower*> m_towers;
  std::vector<L0mPad> m_pads;
  MuonSystemLayout m_layout;   // pad layout for the whole MuonSystem
  MuonSystemLayout m_lulayout; // logical unit layout for the whole MuonSystem
  
  L0mProcUnit m_procUnit;
  IMuonTileXYZTool *m_iTileXYZTool;// this is a tool to get coordinates from ID

  int m_OKcandidates;   // counter of total number og OK candidates

};

#endif    // L0MUON_L0MTRIGGER_H
