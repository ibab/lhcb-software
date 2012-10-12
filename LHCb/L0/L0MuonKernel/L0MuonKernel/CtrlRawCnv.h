#ifndef L0MUONKERNEL_CTRLRAWCNV_H
#define L0MUONKERNEL_CTRLRAWCNV_H

#include "L0MuonKernel/CandRegisterHandler.h"
#include "L0MuonKernel/CtrlRawErrors.h"
#include "L0MuonKernel/MuonCandidate.h"
#include <string>
#include <map>
#include <vector>

namespace L0Muon {


  /** @class CtrlRawCnv CtrlRawCnv.h  L0MuonKernel/CtrlRawCnv.h

      Class to interface the data in L0MuonRaw bank (written by the controller boards) with the L0Muon Register.

      As there is one L0MuonCtrlCand per side (i.e. for 2 quarters), this class handle a side (index stored in #m_side).
      Therefore, it deals with 2 quarters :
      - side A (m_side=0) : Q1 (iq=0) and Q2 (iq=1)
      - side C (m_side=1) : Q3 (iq=0) and Q4 (iq=1)

      It holds pointers to the @link Register registers @endlink containing the data written in the L0MuonRaw bank
      from the TELL1 linked to the controller board :
      - CAND_CTRLQ<q>, where q(=0,...,3) is the quarter : candidates selected by the controller board in quarter q
      - BOARDCAND_Q<q>_<ib>, where q(=0,...,3) is the quarter and ib(=0,...,11) is the board index in the quarter :
      candidates in quarter q sent by the board ib to the controller board.

      @author Julien Cogan
      @date 2007-09-03

  */
  class CtrlRawCnv  {

  private:

    /// Size in words of the full frame sent by 1 controller board on 1 channel (optical link)
    static const unsigned int board_full_frame_size = 36;

    /// Total size in words of the full frame sent by 1 controller board (sum of 2 channels)
    static const unsigned int board_full_data_size  = board_full_frame_size*2;

    /// Size in words of the part of frame containing data sent by 1 controller board on 1 channel (optical link)
    static const unsigned int board_frame_size      = 23;

    /// Total size in words of the frame containing data sent by 1 controller board (summed of 2 channels)
    static const unsigned int board_data_size       = board_frame_size*2 ;

  public:

    /// Default Constructor
    CtrlRawCnv();

    /** Constructor with side identifier (side A=0, side C=1)

    @param side : identifier of the side attached to this converter (0 for A side, 1 for C side)
    */
    CtrlRawCnv(int side);

    /// Destructor
    ~CtrlRawCnv();

    /** Returns the MuonTileID of the board (BCSU) defined by its quarter and board index

        @param iq : index of the quarter in the side (0 to 1)
        @param ib : index of the board (0 to 11)
    */
    LHCb::MuonTileID mid_BCSU(int iq, int ib);

    /// Clear the registers
    void release();

    /// Returns the muon candidates selected by both quarters of the side attached to this converter
    std::vector<PMuonCandidate> muonCandidates();

    /// Returns the muon candidates sent by the BCSU of both quarters of the side attached to this converter
    std::vector<PMuonCandidate> muonCandidatesBCSU();

    /** Returns the status set by the controller board of the specified quarter

        @param iq : index of the quarter in the side (0 to 1)
    */
    int status(int i){return m_candRegHandler[i].getStatus();}

    /** Returns the status set by the specified processing board

        @param iq : index of the quarter in the side (0 to 1)
        @param ib : index of the board (0 to 11)
    */
    int status_BCSU(int i,int ib){return m_candRegHandlerBCSU[i][ib].getStatus();}

    /** Decode the L0MuonRaw bank related to the controller board.

        From the data in the bank, it :
        - fills the registers with the candidates selected by controller board
        - fills the registers with the candidates sent by the processing board
        - fills the CtrlRawErrors errors with the error encountered in the decoding

        Return values :
        -  0 : incomplete bank
        - -1 : bad format (in 1st part of the bank)
        - -2 : bad format (in 2nd part of the bank)
        -  1 : only 1st first part was decoded
        -  2 : the two parts were decoded

        @param raw : vector of unsigned int with the L0MuonCtrlCand bank data
        @param version : version of the L0MuonCtrlCand bank
        @param mode : decoding mode (<=1: decode only the 1st part of the bank, >1: decode both parts)
    */
    void decodeBank(const std::vector<unsigned int> &raw, int version, int &RefL0EventNumber, int &RefL0_B_Id);
    void rawBank(std::vector<unsigned int> &raw, int version);

    void dump(const std::vector<unsigned int> &raw);

    bool inError(int iq){ return m_errors[iq].inError();}
    bool decodingError(int iq){ return m_errors[iq].decodingError();}
    void dumpErrorHeader(int ib, std::string tab="") {std::cout<<m_errors[ib].header(tab)<<std::endl;}
    void dumpErrorField(int ib, std::string tab="") {std::cout<<tab<<m_errors[ib]<<std::endl;}
    void dumpErrorCounters(std::string &os);

    bool isActiv(){return m_activ;}

    int numberOfDecodedBanks() {return m_n_decoded_banks;}

  private:

    void decodeBank_v1(const std::vector<unsigned int> &raw, int &RefL0EventNumber, int &RefL0_B_Id);
    void decodeBank_v2(const std::vector<unsigned int> &raw, int &RefL0EventNumber, int &RefL0_B_Id);

    bool m_activ;
    int m_n_decoded_banks;
    int m_side;

    enum fpgas {CU,SU};

    // Input candidate registers
    CandRegisterHandler m_candRegHandler[2];
    CandRegisterHandler m_candRegHandlerBCSU[2][12];

    CtrlRawErrors m_errors[2];


  };
} // namespace L0Muon

#endif    // L0MUONKERNEL_CTRLRAWCNV_H
