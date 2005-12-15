#ifndef L0MUONKERNEL_L0BUFFERBCSUNIT_H
#define L0MUONKERNEL_L0BUFFERBCSUNIT_H 

/** @class L0BufferBCSUnit L0BufferBCSUnit.h  L0MuonKernel/L0BufferBCSUnit.h
 
  class for constructing the L0Buffer 

  @author
  @date

*/
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/CandRegisterHandler.h"

namespace L0Muon {
  
  static const int L0BufferBCSUSize  =  22*16 ;

  class L0BufferBCSUnit : public L0MUnit {

  public:

    /// Default Constructor
    L0BufferBCSUnit();

    /// Constructor
    L0BufferBCSUnit(MuonTileID id);

    /// Constructor
    L0BufferBCSUnit(DOMNode* pNode);

    /// Destructor
    ~L0BufferBCSUnit();

    void initialize();
    void execute();

   /// Give a static type name to the unit
    std::string type() {
      return "L0BufferBCSUnit";
    }

  private:

    // Input candidate registers from the Proc/Core Unit (max=4)
    std::map<int,CandRegisterHandler> m_candRegHandlerProc;
    // Input candidate register from the BCSU Unit 
    CandRegisterHandler m_candRegHandlerBCSU;


  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_L0BUFFERBCSUNIT_H
