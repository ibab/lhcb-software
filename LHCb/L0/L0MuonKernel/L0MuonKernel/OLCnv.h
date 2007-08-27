#ifndef L0MUONKERNEL_OLCNV_H
#define L0MUONKERNEL_OLCNV_H 

/** @class OLCnv OLCnv.h  L0MuonKernel/OLCnv.h
 
  Abstract base class for writing out the l0Buffers of the L0Muon FPGAs

  @author
  @date

*/

#include "L0MuonKernel/L0BufferCnv.h"
#include <string>

namespace L0Muon {
  
  class OLCnv : public L0BufferCnv {

  public:

    /// Constructor
    OLCnv(LHCb::MuonTileID id);

    /// Destructor
    ~OLCnv();
    
    /// Get the OL type
     /// Give a static type name to the unit
     std::string type() {
//        return "L0Buffer_PU";
       return "OLDumpUnit";
     }

     void write(int ievt);
    
  private:

    TileRegister* m_ol;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_OLCNV_H
