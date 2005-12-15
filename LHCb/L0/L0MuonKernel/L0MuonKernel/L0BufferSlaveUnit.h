#ifndef L0MUONKERNEL_L0BUFFERSLAVEUNIT_H
#define L0MUONKERNEL_L0BUFFERSLAVEUNIT_H 

/** @class L0BufferSlaveUnit L0BufferSlaveUnit.h  L0MuonKernel/L0BufferSlaveUnit.h
 
  class for constructing the L0Buffer 

  @author
  @date

*/
#include <stdio.h>
#include <boost/dynamic_bitset.hpp>
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/CandRegisterHandler.h"

namespace L0Muon {

  static const int L0BufferSlaveSize  =  45*16 ;
  
  class L0BufferSlaveUnit : public L0MUnit {

  public:

    /// Default Constructor
    L0BufferSlaveUnit();

    /// Constructor
    L0BufferSlaveUnit(MuonTileID id);

    /// Constructor
    L0BufferSlaveUnit(DOMNode* pNode);

    /// Destructor
    ~L0BufferSlaveUnit();

    void initialize();
    void execute();

   /// Give a static type name to the unit
    std::string type() {
      return "L0BufferSlaveUnit";
    }

  private:

    // Input candidate registers from the BCSU Unit (max=12)
    std::map<int,CandRegisterHandler> m_candRegHandlerBCSU;
    // Input candidate register from the Ctrl Unit
    CandRegisterHandler m_candRegHandlerCtrl;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_L0BUFFERSLAVEUNIT_H
