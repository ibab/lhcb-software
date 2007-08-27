#ifndef L0MUONKERNEL_BCSUL0BUFFERCNV_H
#define L0MUONKERNEL_BCSUL0BUFFERCNV_H 

/** @class BCSUL0BufferCnv BCSUL0BufferCnv.h  L0MuonKernel/BCSUL0BufferCnv.h
 
  Abstract base class for writing out the l0Buffers of the L0Muon FPGAs

  @author
  @date

*/

#include "L0MuonKernel/L0BufferCnv.h"
#include <string>

namespace L0Muon {
  
  class BCSUL0BufferCnv : public L0BufferCnv {

  public:

    /// Constructor
    BCSUL0BufferCnv(LHCb::MuonTileID id);

    /// Destructor
    ~BCSUL0BufferCnv();
    
    /// Get the BCSUL0Buffer type
     /// Give a static type name to the unit
     std::string type() {
//        return "L0Buffer_BCSU";
       return "L0BufferBCSUnit";
     }

     void write(int ievt);
    
  private:

    // Input candidate registers from the Proc/Core Unit (max=4)
    CandRegisterHandler m_candRegHandlerPU[4];
    // Input candidate register from the BCSU Unit
    CandRegisterHandler m_candRegHandler;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_BCSUL0BUFFERCNV_H
