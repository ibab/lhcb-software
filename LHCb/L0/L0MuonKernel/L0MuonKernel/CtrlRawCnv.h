#ifndef L0MUONKERNEL_CTRLRAWCNV_H
#define L0MUONKERNEL_CTRLRAWCNV_H 

/** @class CtrlRawCnv CtrlRawCnv.h  L0MuonKernel/CtrlRawCnv.h
 
  Class for constructing the raw data generated by the TELL1 linked to the processing boards in transparent mode. 

  @author
  @date

*/
#include "L0MuonKernel/CandRegisterHandler.h"
#include "L0MuonKernel/MuonCandidate.h"
#include <string>
#include <map>
#include <vector>

namespace L0Muon {
  
  class CtrlRawCnv  {
    
  private:

    static const unsigned int board_full_frame_size = 36;
    static const unsigned int board_full_data_size  = board_full_frame_size*2;
    static const unsigned int board_frame_size      = 23;
    static const unsigned int board_data_size       = board_frame_size*2 ;

  public:

    /// Default Constructor
    CtrlRawCnv();

    /// Default Constructor
    CtrlRawCnv(int side);

    /// Destructor
    ~CtrlRawCnv();
    
    LHCb::MuonTileID mid_BCSU(int iq, int ib);
    
    void release();

    std::vector<PMuonCandidate> muonCandidates();
    std::vector<PMuonCandidate> muonCandidatesBCSU();

    int status(int i){return m_candRegHandler[i].getStatus();}
    int status_BCSU(int i,int ib){return m_candRegHandlerBCSU[i][ib].getStatus();}
    
    void decodeBank(std::vector<unsigned int> raw, int version);
    std::vector<unsigned int> rawBank(int version);

    void dump(int version, std::string tab);
    void dump(int version);
    void formattedDump(int version, std::string tab);
    void formattedDump(int version);

    int L0_B_Id(int iboard, int ichannel)      {return m_L0_B_Id[iboard][ichannel];}
    int L0EventNumber(int iboard, int ichannel){return m_L0EventNumber[iboard][ichannel];}
    int BCID_SU(int iboard) {return m_BCID_SU[iboard];}
    int BCID_CU(int iboard) {return m_BCID_CU[iboard];}
    int BCID(int iboard, int ichannel) {return m_BCID[iboard][ichannel];}
    int BCID_BCSU(int iboard, int ichannel, int ibcsu) {return m_BCID_BCSU[iboard][ichannel][ibcsu];}

    int pb_link_error(int iboard, int ichannel, int ibcsu) {return m_pb_link_error[iboard][ichannel][ibcsu];}
    bool boardIndexError(int iboard, int ichannel) {return m_boardIndex[iboard][ichannel]!=(m_side*2+iboard);}

    bool inError(bool verbose=false);
    
    bool isActiv(){return m_activ;}
    
    int decodingError(int iq){ return m_decodingError[iq];}

    void setEventCounters(int L0EventNum, int L0_B_Id);

  private:
    
    bool m_activ;
    
    int m_side;

    enum fpgas {CU,SU};

    // Input candidate registers
    CandRegisterHandler m_candRegHandler[2];
    CandRegisterHandler m_candRegHandlerBCSU[2][12];

    int m_L0_B_Id[2][2];
    int m_L0EventNumber[2][2];
    int m_BCID[2][2];
    int m_BCID_BCSU[2][2][12];
    int m_BCID_SU[2];
    int m_BCID_CU[2];
    int m_pb_link_error[2][2][12];
    int m_error[2];
    int m_boardIndex[2][2];

    int m_decodingError[2];
    
  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_CTRLRAWCNV_H
