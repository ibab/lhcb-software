// $Header:

#ifndef L0MUON_L0MTRIGGERPROC_H
#define L0MUON_L0MTRIGGERPROC_H 1


// Include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"

// Private include files
#include "L0mFoI.h"

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
  double pt (L0mPad* mp1, L0mPad* mp2 );
  double theta (L0mPad* mp1, L0mPad* mp2 );
  double phi (L0mPad* mp1, L0mPad* mp2 );
  
private: 

  L0mFoI* m_foi;
  IntegerArrayProperty m_foiXSize;  
  IntegerArrayProperty m_foiYSize;  
  DoubleProperty m_d1;
  DoubleProperty m_d2;
  DoubleProperty m_d3;
  DoubleProperty m_alpha;
  StringProperty m_outputCandidates;

};

#endif    // L0MUON_L0MTRIGGERPROC_H
