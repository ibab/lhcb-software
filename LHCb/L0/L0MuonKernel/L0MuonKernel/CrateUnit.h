#ifndef L0MUONKERNEL_CRATEUNIT_H
#define L0MUONKERNEL_CRATEUNIT_H     1

/** @class CrateUnit CrateUnit.h L0MuonKernel/CrateUnit.h

Class representing a quarter 
of the level-0 trigger: It constructs 
boards, best candidate selection unit and processing 
units. It initialize, execute and finalize all 
the subunits. 
   
author  Luisanna Tocco
date  24 September 2003
*/ 


#include "L0MuonKernel/L0MUnit.h"

namespace L0Muon {

  class CrateUnit : public L0MUnit {
    
  public:
    
    /// Constructor
    CrateUnit();

    /// Constructor
    CrateUnit(DOMNode* pNode);

    /// Destructor
    ~CrateUnit();
 
    /// Give a static type name to the unit
    std::string type() {
      return "CrateUnit";
    }
    
  private:

  };

};  // namespace L0Muon

#endif      // L0MUONKERNEL_CRATEUNIT_H  
