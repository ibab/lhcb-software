#ifndef L0MUONKERNEL_BOARDUNIT_H
#define L0MUONKERNEL_BOARDUNIT_H     1

/* class BoardUnit L0MuonKernel/BoardUnit.h

   Class representing a board 
   of the L0Muon Trigger for hardware simulations
   
   author  Luisanna Tocco
   date  24 September 2003
*/ 


#include <boost/dynamic_bitset.hpp>
#include <vector>
#include "L0mConf/L0MPuNodeBase.h"
#include "L0MuonKernel/L0mProcUnit.h"
#include "L0MuonKernel/Unit.h"
#include "L0MuonKernel/BestCandidateSelectionUnit.h"
#include "MuonTools/IMuonTileXYZTool.h"
#include "GaudiKernel/MsgStream.h"

namespace L0Muon {

   class BoardUnit : public Unit {
     
  public:

     BoardUnit();
    ~BoardUnit();

     void initialize();
     void execute();
     void finalize();

  private:
     
     int m_imax ;
     BestCandidateSelectionUnit m_Bcsu;
     

  };


};  // namespace L0Muon

#endif      // L0MUONKERNEL_BOARDUNIT_H  
