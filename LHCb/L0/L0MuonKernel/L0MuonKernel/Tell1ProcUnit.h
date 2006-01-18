#ifndef L0MUONKERNEL_TELL1PROCUNIT_H
#define L0MUONKERNEL_TELL1PROCUNIT_H     1

/* class Tell1ProcUnit L0MuonKernel/Tell1ProcUnit.h

   Class representing a TELL1 
   of the level-o muon trigger processor 
   
   author  Julien Cogan
   date  8 November 2005
*/ 

#include "L0MuonKernel/L0MUnit.h"


namespace L0Muon {

   class Tell1ProcUnit : public L0MUnit {
     
  public:

    /// Constructor 
    Tell1ProcUnit();

    /// Constructor 
    Tell1ProcUnit(LHCb::MuonTileID id);

    /// Constructor 
    Tell1ProcUnit(DOMNode* pNode);

    /// Destructor
    ~Tell1ProcUnit();

    /// Give a static type name to the unit
    std::string type() {
      return "Tell1ProcUnit";
    }

  private:
     
     

  };


};  // namespace L0Muon

#endif      // L0MUONKERNEL_TELL1PROCUNIT_H  
