// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/L0mTriggerProc.h,v 1.3 2001-07-10 10:20:16 atsareg Exp $

#ifndef L0MUON_L0MTRIGGERPROC_H
#define L0MUON_L0MTRIGGERPROC_H 1


// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"

// Private include files
#include "L0Muon/L0mTower.h"
#include "L0Muon/L0mProcUnit.h"

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
  
  ObjectVector<L0mTower>* m_towers;
  std::vector<MuonLayout> m_layout;
  
  L0mProcUnit m_procUnit;

};

#endif    // L0MUON_L0MTRIGGERPROC_H
