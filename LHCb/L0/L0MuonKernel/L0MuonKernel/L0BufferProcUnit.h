#ifndef L0MUONKERNEL_L0BUFFERPROCUNIT_H
#define L0MUONKERNEL_L0BUFFERPROCUNIT_H 

/** @class L0BufferProcUnit L0BufferProcUnit.h  L0MuonKernel/L0BufferProcUnit.h
 
  class for constructing the L0Buffer 

  @author
  @date

*/
#include <stdio.h>
#include <boost/dynamic_bitset.hpp>
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/CandRegisterHandler.h"

namespace L0Muon {

  static const int L0BufferProcSize  =  36*16 ;
  
  class L0BufferProcUnit : public L0MUnit {

  public:

    /// Default Constructor
    L0BufferProcUnit();

    /// Constructor
    L0BufferProcUnit(LHCb::MuonTileID id);

    /// Constructor
    L0BufferProcUnit(DOMNode* pNode);

    /// Destructor
    ~L0BufferProcUnit();

    void initialize();
    void execute();
    
   /// Give a static type name to the unit
    std::string type() {
      return "L0BufferProcUnit";
    }

  private:

    CandRegisterHandler m_candRegHandlerIn;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_L0BUFFERPROCUNIT_H
