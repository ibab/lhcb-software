#ifndef L0MUONKERNEL_CTRLUNIT_H
#define L0MUONKERNEL_CTRLUNIT_H     1

/* class CtrlUnit L0MuonKernel/CtrlUnit.h

   Class representing a ctrl 
   of the level-o muon trigger processor 
   
   author  Luisanna Tocco
   date  24 September 2003
*/ 


#include "L0MuonKernel/SelectionUnit.h"


namespace L0Muon {

   class CtrlUnit : public SelectionUnit {
     
  public:

    /// Constructor 
    CtrlUnit();

    /// Constructor 
    CtrlUnit(LHCb::MuonTileID id);

    /// Constructor 
    CtrlUnit(DOMNode* pNode);

    /// Destructor
    ~CtrlUnit();

    void initialize();
    void execute();

    /// Give a static type name to the unit
    std::string type() {
      return "CtrlUnit";
    }

   private:

  };


};  // namespace L0Muon

#endif      // L0MUONKERNEL_CTRLUNIT_H  
