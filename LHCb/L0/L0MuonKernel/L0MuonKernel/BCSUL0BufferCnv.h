#ifndef L0MUONKERNEL_BCSUL0BUFFERCNV_H
#define L0MUONKERNEL_BCSUL0BUFFERCNV_H


#include "L0MuonKernel/L0BufferCnv.h"
#include <string>

namespace L0Muon {

  /** @class BCSUL0BufferCnv BCSUL0BufferCnv.h  L0MuonKernel/BCSUL0BufferCnv.h

  Class for writing out the l0Buffer of the L0Muon BCSU FPGA.

  It inherits from L0BufferCnv. There is one such converter per BCSU.

  As input , it uses :
  - Register with PU candidates named : PUCAND_BCSUQ<q>R<r><xy>_<ipu> (ipu is the PU index in the board);
    not that this register is an alias to CAND_PUQ<q>R<r><x',y'>, where x',y' refers to the PU's MuonTileID coordinates
  - Register with BCSU candidates named : CAND_BCSUQ<q>R<r><xy>
  .
  , where q(=1,...,4), r=(1,...,4), x,y refers to the BCSU attached to this converter

  @author Julien Cogan
  @date 2008-08-22

  */
  class BCSUL0BufferCnv : public L0BufferCnv {

  public:

    /** Constructor with MuonTileID

    @param id : MuonTileID of the BCSU attached to the converter

    */
    BCSUL0BufferCnv(LHCb::MuonTileID id);

    /// Destructor
    ~BCSUL0BufferCnv();

    /// Give a static type name to the unit : L0BufferBCSUnit
    std::string type() {
      return "L0BufferBCSUnit";
    }

    /** Write the event in the output file

        @param ievt : current event number
    */
    void write(int ievt);

  private:

    CandRegisterHandler m_candRegHandlerPU[4]; ///< Handle the registers with the PU's candidates
    CandRegisterHandler m_candRegHandler; ///< Handle the register with the BCSU candidates

  };
} // namespace L0Muon

#endif    // L0MUONKERNEL_BCSUL0BUFFERCNV_H
