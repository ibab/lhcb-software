#ifndef L0MUONKERNEL_L0BUFFERCNV_H
#define L0MUONKERNEL_L0BUFFERCNV_H 

/** @class L0BufferCnv L0BufferCnv.h  L0MuonKernel/L0BufferCnv.h
 
  Abstract base class for writing out the l0Buffers of the L0Muon FPGAs

  @author
  @date

*/

#include "Kernel/MuonTileID.h"
#include "ProcessorKernel/TileRegister.h"
#include "L0MuonKernel/CandRegisterHandler.h"
#include <string>
#include <map>
#include <vector>
#include <fstream>

namespace L0Muon {
  
  class L0BufferCnv {

  public:

    /// Constructor
    L0BufferCnv(LHCb::MuonTileID id);

    /// Destructor
    virtual ~L0BufferCnv();
    
    /// Get the L0Buffer type
    virtual std::string type() =0;

    void open(std::string path);
    void close();

    virtual void write(int ievt) =0;

    LHCb::MuonTileID mid(){return m_mid;}
    
    
  protected:
    LHCb::MuonTileID m_mid;

    // Output file
//     FILE *m_file;
    std::ofstream m_file;
    
    bool m_valid;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_L0BUFFERCNV_H
