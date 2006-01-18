#ifndef L0MUONKERNEL_BOARDUNIT_H
#define L0MUONKERNEL_BOARDUNIT_H     1

/* class BoardUnit L0MuonKernel/BoardUnit.h

   Class representing a board 
   of the level-o muon trigger processor 
   
   author  Luisanna Tocco
   date  24 September 2003
*/ 


#include <boost/dynamic_bitset.hpp>
#include <vector>
#include "L0MuonKernel/ProcUnit.h"
#include "L0MuonKernel/L0MUnit.h"


namespace L0Muon {

   class BoardUnit : public L0MUnit {
     
  public:

    /// Constructor 
    BoardUnit();

    /// Constructor 
    BoardUnit(LHCb::MuonTileID id);

    /// Constructor 
    BoardUnit(DOMNode* pNode);

    /// Destructor
    ~BoardUnit();

    /// Give a static type name to the unit
    std::string type() {
      return "BoardUnit";
    }

  private:
     

  };


};  // namespace L0Muon

#endif      // L0MUONKERNEL_BOARDUNIT_H  
