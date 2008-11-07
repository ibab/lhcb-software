#ifndef L0MUONKERNEL_SELECTIONUNIT_H
#define L0MUONKERNEL_SELECTIONUNIT_H     1

#include <map>
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/CandRegisterHandler.h"


namespace L0Muon {

  /** @class SelectionUnit SelectionUnit.h L0MuonKernel/SelectionUnit.h

  Base class to perform candidate selection in the L0Muon processor .

  It inherits from L0MUnit.

  During the postexecute phase, it :
  - selects the 2 best candidates (highest PT) from the list of candidates in input registers,
  - fills the output register with the selected candidates.

  The input and output registers are manipulated using CandRegisterHandler.

  This class is specialized in BCSUnit and CtrlUnit.

  author  Luisanna Tocco, Julien Cogan
  date  24 September 2003
  */ 
  class SelectionUnit : public L0MUnit {
     
  public:

    /// Default Constructor 
    SelectionUnit();

    /// Constructor with a MuonTileID
    SelectionUnit(LHCb::MuonTileID id);

    /// Constructor from xml node
    SelectionUnit(DOMNode* pNode);

    /// Destructor
    ~SelectionUnit();
     
    /// Select the 2 best candidates and fill the output register
    void select();

    /// Release output register
    void postexecute();

  protected:
     
    std::map<int,CandRegisterHandler> m_candRegHandlerIn; ///< input candidates
    CandRegisterHandler m_candRegHandlerOut;              ///< output candidates 

  };


};  // namespace L0Muon

#endif      // L0MUONKERNEL_SELECTIONUNIT_H  
