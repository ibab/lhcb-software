#ifndef L0MUONKERNEL_PUL0BUFFERCNV_H
#define L0MUONKERNEL_PUL0BUFFERCNV_H 

/** @class PUL0BufferCnv PUL0BufferCnv.h  L0MuonKernel/PUL0BufferCnv.h
 
  Abstract base class for writing out the l0Buffers of the L0Muon FPGAs

  @author
  @date

*/

#include "L0MuonKernel/L0BufferCnv.h"
#include <string>

namespace L0Muon {
  
  class PUL0BufferCnv : public L0BufferCnv {

  public:

    /// Constructor
    PUL0BufferCnv(LHCb::MuonTileID id);

    /// Destructor
    ~PUL0BufferCnv();
    
    /// Get the PUL0Buffer type
     /// Give a static type name to the unit
     std::string type() {
//        return "L0Buffer_PU";
       return "L0BufferProcUnit";
     }

     void write(int ievt);
    
  private:

    CandRegisterHandler m_candRegHandler;
    TileRegister* m_ol;
    TileRegister* m_neigh;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_PUL0BUFFERCNV_H
