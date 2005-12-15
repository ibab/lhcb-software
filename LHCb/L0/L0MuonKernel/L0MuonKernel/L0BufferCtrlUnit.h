#ifndef L0MUONKERNEL_L0BUFFERCTRLUNIT_H
#define L0MUONKERNEL_L0BUFFERCTRLUNIT_H 

/** @class L0BufferCtrlUnit L0BufferCtrlUnit.h  L0MuonKernel/L0BufferCtrlUnit.h
 
  class for constructing the L0Buffer 

  @author
  @date

*/
#include <stdio.h>
#include <boost/dynamic_bitset.hpp>
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/CandRegisterHandler.h"

namespace L0Muon {

  static const int L0BufferCtrlSize  =  44*16 ;
  
  class L0BufferCtrlUnit : public L0MUnit {

  public:

    /// Default Constructor
    L0BufferCtrlUnit();

    /// Constructor
    L0BufferCtrlUnit(MuonTileID id);

    /// Constructor
    L0BufferCtrlUnit(DOMNode* pNode);

    /// Destructor
    ~L0BufferCtrlUnit();

    void initialize();
    void execute();

   /// Give a static type name to the unit
    std::string type() {
      return "L0BufferCtrlUnit";
    }

  private:

    // Input candidate registers from the BCSU Unit (max=12)
    std::map<int,CandRegisterHandler> m_candRegHandlerBCSU;
    // Input candidate register from the Ctrl Unit
    CandRegisterHandler m_candRegHandlerCtrl;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_L0BUFFERCTRLUNIT_H
