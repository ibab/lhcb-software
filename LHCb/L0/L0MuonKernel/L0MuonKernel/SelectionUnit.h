#ifndef L0MUONKERNEL_SELECTIONUNIT_H
#define L0MUONKERNEL_SELECTIONUNIT_H     1

/* class SelectionUnit L0MuonKernel/SelectionUnit.h

Class representing a selection 
of the level-o muon trigger processor 
   
author  Luisanna Tocco
date  24 September 2003
*/ 


#include <map>
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/CandRegisterHandler.h"


namespace L0Muon {

  class SelectionUnit : public L0MUnit {
     
  public:

    /// Constructor 
    SelectionUnit();

    /// Constructor 
    SelectionUnit(LHCb::MuonTileID id);

    /// Constructor 
    SelectionUnit(DOMNode* pNode);

    /// Destructor
    ~SelectionUnit();
     
    // Select the 2 best candidates and fill the output register
    void select();

    void postexecute();

  protected:
     
    std::map<int,CandRegisterHandler> m_candRegHandlerIn;
    CandRegisterHandler m_candRegHandlerOut;

  };


};  // namespace L0Muon

#endif      // L0MUONKERNEL_SELECTIONUNIT_H  
