#ifndef L0MUONKERNEL_TELL1CTRLUNIT_H
#define L0MUONKERNEL_TELL1CTRLUNIT_H     1

/* class Tell1CtrlUnit L0MuonKernel/Tell1CtrlUnit.h

   Class representing a TELL1CTRL 
   of the level-o muon trigger processor 
   
   author  Julien Cogan
   date  8 November 2005
*/ 

#include "L0MuonKernel/L0MUnit.h"

namespace L0Muon {

   class Tell1CtrlUnit : public L0MUnit {
     
  public:

    /// Constructor 
    Tell1CtrlUnit();

    /// Constructor 
    Tell1CtrlUnit(LHCb::MuonTileID id);

    /// Constructor 
    Tell1CtrlUnit(DOMNode* pNode);

    /// Destructor
    ~Tell1CtrlUnit();

    /// Give a static type name to the unit
    std::string type() {
      return "Tell1CtrlUnit";
    }

  private:
     
  };


};  // namespace L0Muon

#endif      // L0MUONKERNEL_TELL1CTRLUNIT_H  
