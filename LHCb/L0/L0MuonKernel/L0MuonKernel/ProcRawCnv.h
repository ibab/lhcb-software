#ifndef L0MUONKERNEL_PROCRAWCNV_H
#define L0MUONKERNEL_PROCRAWCNV_H

#include "Kernel/MuonTileID.h"
#include "L0MuonKernel/CandRegisterHandler.h"
#include "L0MuonKernel/ProcRawErrors.h"
#include "L0MuonKernel/MuonCandidate.h"
#include "ProcessorKernel/TileRegister.h"
#include <boost/dynamic_bitset.hpp>
#include <string>
#include <map>
#include <vector>

namespace L0Muon {


  /** @class ProcRawCnv ProcRawCnv.h  L0MuonKernel/ProcRawCnv.h

      Class to interface the data in L0MuonRaw bank (written by the processing boards) with the L0Muon Register.

      @author Julien Cogan
      @date 2007-09-03

  */
  class ProcRawCnv  {

  private:

    static const unsigned int board_full_frame_size = 36;
    static const unsigned int board_full_data_size  = board_full_frame_size*2;
    static const unsigned int board_frame_size      = 34;
    static const unsigned int board_data_size       = board_frame_size*2;

  public:

    /// Default Constructor
    ProcRawCnv();

    /// Default Constructor
    ProcRawCnv(int side);

    /// Destructor
    ~ProcRawCnv();

    LHCb::MuonTileID mid_BCSU(int ib);
    LHCb::MuonTileID mid_PU(int ib, int ipu);

    void release();

    std::vector<PMuonCandidate> muonCandidatesPU();
    std::vector<PMuonCandidate> muonCandidatesBCSU();
    std::vector<LHCb::MuonTileID> ols(LHCb::MuonTileID puid);
    std::vector<LHCb::MuonTileID> neighs(LHCb::MuonTileID puid);
    std::vector<LHCb::MuonTileID> pus();

    int status_BCSCU(int ib){return m_candRegHandlerBCSU[ib].getStatus();}
    int status_PU(int ib,int ipu){return m_candRegHandlerPU[ib][ipu].getStatus();}

    void decodeBank(const std::vector<unsigned int> &raw, int version, int &RefL0EventNumber, int &RefL0_B_Id);

    std::vector<unsigned int> rawBankW(int version){
      std::vector<unsigned int> raw;
      rawBank(raw, version);
      return raw;
    }
    void rawBank(std::vector<unsigned int> &raw, int version);

    void dump(const std::vector<unsigned int> &raw);

    bool inError(int ib){ return m_errors[ib].inError();}
    bool decodingError(int ib){ return m_errors[ib].decodingError();}
    void dumpErrorHeader(int iq, std::string tab="") {std::cout<<m_errors[iq].header(tab)<<std::endl;}
    void dumpErrorField(int iq, std::string tab="") {std::cout<<tab<<m_errors[iq]<<std::endl;}
    void dumpErrorCounters(std::string &os) ;


    bool isActiv(){return m_activ;}

    int numberOfDecodedBanks() {return m_n_decoded_banks;}

//     int checkMuonTiles();

  private:

    bool m_activ;
    int m_n_decoded_banks;

    void decodeBank_v1(const std::vector<unsigned int> &raw, int &RefL0EventNumber, int &RefL0_B_Id);
    void decodeBank_v2(const std::vector<unsigned int> &raw, int &RefL0EventNumber, int &RefL0_B_Id);

    int m_quarter;

    void reorderOLChannels(const std::vector<unsigned int> &raw, std::vector<unsigned int> &reordered);

    // Candidate registers
    CandRegisterHandler m_candRegHandlerBCSU[12];
    CandRegisterHandler m_candRegHandlerPU[12][4];

    // Pointers to tile registers organised into arrays according to board and pu indices
    TileRegister* m_ols[12][4];
    TileRegister* m_neighs[12][4];

    // Pointers to tile registers, organised into a map with the puID as a key
    std::map<LHCb::MuonTileID, TileRegister*> m_olsMap;
    std::map<LHCb::MuonTileID, TileRegister*> m_neighsMap;


    ProcRawErrors m_errors[12];

  };
} // namespace L0Muon


#endif    // L0MUONKERNEL_PROCRAWCNV_H
