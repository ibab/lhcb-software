// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/L0mTriggerProcD.h,v 1.1 2001-06-07 16:46:18 atsareg Exp $

#ifndef L0MUON_L0MTRIGGERPROCD_H
#define L0MUON_L0MTRIGGERPROCD_H 1

// Private include files
#include "L0mTriggerProc.h"
#include "L0mCrate.h"

/** @class L0mTriggerProcD L0mTriggerProcD.h L0mTriggerProcD.h 

   L0Muon trigger processor detailed algorithm
   
   @author Andrei Tsaregorodtsev
   @date 6 April 2001 
*/


class L0mPad;

class L0mTriggerProcD : public L0mTriggerProc {
public:
  /// Constructor: A constructor of this form must be provided.
  L0mTriggerProcD(const std::string& name, ISvcLocator* pSvcLocator); 

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private: 
  
  double m_precision;
  int m_bits;
  std::vector<L0mCrate> m_crates;

};

#endif    // L0MUON_L0MTRIGGERPROCD_H
