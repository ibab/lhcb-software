#ifndef L0MUONKERNEL_SUL0BUFFERCNV_H
#define L0MUONKERNEL_SUL0BUFFERCNV_H 

/** @class SUL0BufferCnv SUL0BufferCnv.h  L0MuonKernel/SUL0BufferCnv.h
 
  Abstract base class for writing out the l0Buffers of the L0Muon FPGAs

  @author
  @date

*/

#include "L0MuonKernel/L0BufferCnv.h"
#include <string>

namespace L0Muon {
  
  class SUL0BufferCnv : public L0BufferCnv {

  public:

    /// Constructor
    SUL0BufferCnv(LHCb::MuonTileID id);

    /// Destructor
    ~SUL0BufferCnv();
    
    /// Get the SUL0Buffer type
     /// Give a static type name to the unit
     std::string type() {
//        return "L0Buffer_SU";
       return "L0BufferSlaveUnit";
     }

     void write(int ievt);
    
  private:

    // Candidate registers for the proc. boards
    CandRegisterHandler m_candRegHandlerBCSU[12];
    // Candidate register for the ctrl board
    CandRegisterHandler m_candRegHandler;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_SUL0BUFFERCNV_H
