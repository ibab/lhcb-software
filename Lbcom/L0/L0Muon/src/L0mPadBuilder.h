// $Header:

#ifndef L0MUON_L0MPADBUILDER_H
#define L0MUON_L0MPADBUILDER_H 1


// Include files
#include <vector>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Property.h"

// Forward declarations
class MuonPadLayout;

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

  MuonPadLayout* m_layout;
  BooleanProperty m_layout_xml;
  DoubleArrayProperty m_zStation;
  DoubleArrayProperty m_basicCellX;
  DoubleArrayProperty m_basicCellY;
  IntegerArrayProperty m_regions[5];
  StringProperty m_outputPads;
  
};

#endif    // L0MUON_L0MPADBUILDER_H
