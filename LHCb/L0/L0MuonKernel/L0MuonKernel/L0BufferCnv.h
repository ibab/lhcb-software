#ifndef L0MUONKERNEL_L0BUFFERCNV_H
#define L0MUONKERNEL_L0BUFFERCNV_H 


#include "Kernel/MuonTileID.h"
#include "ProcessorKernel/TileRegister.h"
#include "L0MuonKernel/CandRegisterHandler.h"
#include <string>
#include <map>
#include <vector>
#include <fstream>

namespace L0Muon {
  
  /** @class L0BufferCnv L0BufferCnv.h  L0MuonKernel/L0BufferCnv.h
 
  Abstract base class for writing out the l0Buffers of the L0Muon FPGAs.

  The L0Buffer converters takes some registers in input and write out in a file the L0Buffer 
  in a format suitable for the High stat test of the L0Muon.

  There is one specialization of L0BufferCnv for each type of FPGA of the L0Muon processor :
  - PUL0BufferCnv 
  - BCSUL0BufferCnv
  - CUL0BufferCnv
  - SUL0BufferCnv
  
  In addition, the OLCnv outputs the content of the optical link in a format suitable  
  for the PU injection buffer.
 
  @author Julien Cogan
  @date 2007-08-27
  
  */
  class L0BufferCnv {

  public:

    /** Constructor

    @param id : MuonTileID of the FPGA attached to the L0Buffer

    */
    L0BufferCnv(LHCb::MuonTileID id);

    /// Destructor
    virtual ~L0BufferCnv();
    
    /// Get the L0Buffer type
    virtual std::string type() =0;

    /** Open the output file

        The filename is : <type>Q<q>R<r><xy>.txt where :
        - q(=1,4), r(=1,4) x and y refers to the MuonTileID of the L0Buffer converter. 
        - type is the L0Buffer converter type
  
        @param path : directory where the file should be written
    */
    void open(std::string path);
    
    /// Close the output file.
    void close();

    /** Write the event in the output file.

        @param ievt : current event number
    */
    virtual void write(int ievt) =0;

    /// Returns MuonTileID associated to the L0Buffer converter.
    LHCb::MuonTileID mid(){return m_mid;}
    
    
  protected:
    LHCb::MuonTileID m_mid; ///< MuonTileID of the FPGA associated to the L0Buffer converter

    std::ofstream m_file; ///< Output file where the L0Buffer is written 
    
    bool m_valid;         ///< Flag indicating whether the L0Buffer is valid. 
                          ///< Set to true in the constructor of the speciliazed class 
                          ///< if the necessary registers have been found.

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_L0BUFFERCNV_H
