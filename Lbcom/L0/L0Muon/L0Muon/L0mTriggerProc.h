// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/L0mTriggerProc.h,v 1.1 2001-06-07 16:46:17 atsareg Exp $

#ifndef L0MUON_L0MTRIGGERPROC_H
#define L0MUON_L0MTRIGGERPROC_H 1


// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"

// Private include files
#include "L0Muon/L0mTower.h"

/** @class L0mTrigger L0mTrigger.h L0mTrigger.h 

   L0Muon trigger processor algorithm
   
   @author Andrei Tsaregorodtsev
   @date 6 April 2001 
*/


class L0mPad;

class L0mTriggerProc : public Algorithm {
public:
  /// Constructor: A constructor of this form must be provided.
  L0mTriggerProc(const std::string& name, ISvcLocator* pSvcLocator); 

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
protected: 

  /// Create a tower starting from a pad in M3
  L0mTower* createTower(L0mPad* pad, ObjectVector<L0mPad>* pads); 

  std::vector<int> m_foiXSize;  
  std::vector<int> m_foiYSize;  
  std::vector<double> m_ptParameters;  
  std::string m_outputCandidates;
  std::string m_inputPads;
  std::string m_mode;
  
  ObjectVector<L0mTower>* m_towers;
  std::vector<MuonLayout> m_layout;

};

#endif    // L0MUON_L0MTRIGGERPROC_H
