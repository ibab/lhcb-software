// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/L0mTrigger.h,v 1.1 2001-06-07 16:46:17 atsareg Exp $

#ifndef L0MUON_L0MTRIGGER_H
#define L0MUON_L0MTRIGGER_H 1


// Include files
#include "GaudiKernel/Algorithm.h"

/** @class L0mTrigger L0mTrigger.h L0mTrigger.h 

   L0Muon trigger steering algorithm
   
   @author Andrei Tsaregorodtsev
   @date 6 April 2001 
*/

class L0mTrigger : public Algorithm {
public:
  /// Standard algorithm constructor
  L0mTrigger(const std::string& name, ISvcLocator* pSvcLocator); 

  /// Standard algorithm initialize method
  StatusCode initialize();
  /// Standard algorithm execute method
  StatusCode execute();
  /// Standard algorithm finalize method
  StatusCode finalize();
    
private:
  Algorithm* m_padBuilder;
  Algorithm* m_triggerProc;
  
  std::string m_triggerProcMode;
  
};

#endif    // L0MUON_L0MTRIGGER_H
