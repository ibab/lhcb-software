#ifndef L0MUONKERNEL_CUL0BUFFERCNV_H
#define L0MUONKERNEL_CUL0BUFFERCNV_H 

/** @class CUL0BufferCnv CUL0BufferCnv.h  L0MuonKernel/CUL0BufferCnv.h
 
  Abstract base class for writing out the l0Buffers of the L0Muon FPGAs

  @author
  @date

*/

#include "L0MuonKernel/L0BufferCnv.h"
#include <string>

namespace L0Muon {
  
  class CUL0BufferCnv : public L0BufferCnv {

  public:

    /// Constructor
    CUL0BufferCnv(LHCb::MuonTileID id);

    /// Destructor
    ~CUL0BufferCnv();
    
    /// Get the CUL0Buffer type
     /// Give a static type name to the unit
     std::string type() {
//        return "L0Buffer_CU";
       return "L0BufferCtrlUnit";
     }

     void write(int ievt);
    
  private:

    // Candidate registers for the proc. boards
    CandRegisterHandler m_candRegHandlerBCSU[12];
    // Candidate register for the ctrl board
    CandRegisterHandler m_candRegHandler;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_CUL0BUFFERCNV_H
