// $Header:

#ifndef L0MUON_L0MTOWERBUILDER_H
#define L0MUON_L0MTOWERBUILDER_H 1


// Include files
#include <vector>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/DataObject.h"
#include "L0mTower.h"
#include "MuonLayout.h"


/** @class L0mTowerBuilder L0mTowerBuilder.h L0mTowerBuilder.h 

   Algorithm for building a list of L0mTower's 
   
   @author Andrei Tsaregorodtsev
   @date 6 April 2001 
*/

class L0mTowerBuilder : public Algorithm {
public:
  // Constructor: A constructor of this form must be provided.
  L0mTowerBuilder(const std::string& name, ISvcLocator* pSvcLocator); 

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  L0mTower* createTower(L0mPad* pad, ObjectVector<L0mPad>* pads);     
  
private: 

  MuonLayout  m_layout1;
  MuonLayout  m_layout2;
  MuonLayout  m_layout3;
  MuonLayout  m_layout4;
  MuonLayout  m_layout5;
  
};

#endif    // L0MUON_L0MTOWERBUILDER_H
