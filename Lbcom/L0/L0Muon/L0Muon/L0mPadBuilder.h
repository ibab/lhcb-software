// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/L0mPadBuilder.h,v 1.3 2001-07-09 19:27:15 atsareg Exp $

#ifndef L0MUON_L0MPADBUILDER_H
#define L0MUON_L0MPADBUILDER_H 1


// Include files
#include <vector>
#include <string>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"

// Forward declarations
class MuonLogicalPadLayout;

/** @class L0mPadBuilder L0mPadBuilder.h L0mPadBuilder.h 

   Algorithm for building a list of L0mPad's 
   
   @author Andrei Tsaregorodtsev
   @date 6 April 2001 
*/

class L0mPadBuilder : public Algorithm {
public:
  // Constructor: A constructor of this form must be provided.
  L0mPadBuilder(const std::string& name, ISvcLocator* pSvcLocator); 

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  MuonLogicalPadLayout* m_layout;
  bool m_layout_xml;
  std::vector<double> m_zStation;
  std::vector<int> m_regions[5];
  std::string m_outputPads;
  
};

#endif    // L0MUON_L0MPADBUILDER_H
