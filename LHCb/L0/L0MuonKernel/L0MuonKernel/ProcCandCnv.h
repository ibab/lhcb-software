#ifndef L0MUONKERNEL_PROCCANDCNV_H
#define L0MUONKERNEL_PROCCANDCNV_H 

#include "L0MuonKernel/CandRegisterHandler.h"
#include <string>
#include <map>
#include <vector>
#include "L0MuonKernel/ProcCandErrors.h"

namespace L0Muon {
  
  /** @class ProcCandCnv ProcCandCnv.h  L0MuonKernel/ProcCandCnv.h
      
      Class to interface the data in L0MuonProcCand bank with the L0Muon Register.

      @author Julien Cogan
      @date 2007-09-03
  */
  class ProcCandCnv  {

  public:

    /// Default Constructor
    ProcCandCnv();

    /// Default Constructor
    ProcCandCnv(int quarter);

    /// Destructor
    ~ProcCandCnv();

    LHCb::MuonTileID mid_BCSU(int ib);
    LHCb::MuonTileID mid_PU(int ib, int ipu);

    void release();
    void setDecodingMode();
    
    std::vector<PMuonCandidate> muonCandidatesPU();
    std::vector<PMuonCandidate> muonCandidatesBCSU();
    std::vector<PMuonCandidate> muonCandidatesPU(int ib);
    std::vector<PMuonCandidate> muonCandidatesBCSU(int ib);
    std::vector<PMuonCandidate> muonCandidatesPU(int ib, int ipu);

    int decodeBank(const std::vector<unsigned int> & raw, int version);
    int decodeBank_v1(const std::vector<unsigned int> &raw);
    int decodeBank_v2(const std::vector<unsigned int> &raw);

    int rawBank(std::vector<unsigned int> &raw, int ievt, int version, bool compression);
    int rawBank_v1(std::vector<unsigned int> &raw, int ievt);
    int rawBank_v2(std::vector<unsigned int> &raw, int ievt, bool compression);
    
    void dump(std::string tab=""){
      for (int ib=0; ib<12; ++ib) 
      {
        m_candRegHandlerBCSU[ib].dump(-1,tab);
        for (int ip=0; ip<4; ++ip) {
          m_candRegHandlerPU[ib][ip].dump(-1,tab+" ");
        }      
      }
    }

    int ref_l0_B_Id() {return m_ref_l0_B_Id;}
    int ref_l0EventNumber() {return m_ref_l0EventNumber;}
    void submitL0_B_Id(int l0_B_Id){if (m_ref_l0_B_Id==-1) m_ref_l0_B_Id=l0_B_Id;}
    void submitL0EventNumber(int l0EventNumber){if (m_ref_l0EventNumber==-1) m_ref_l0EventNumber=l0EventNumber;}
    void clearRef();

    const bool inError(int ib) const { return m_errors[ib].inError();}
    const bool inError(int ib, int ipu) const { return m_errors[ib].inError(ipu);}
    const int  decodingError(int ib) const { return m_errors[ib].decodingError();}
    const int  hardwareError(int ib) const { return m_errors[ib].hardwareError();}
    const int  statusError(int ib) const { return m_errors[ib].statusError();}
    const int  statusError(int ib,int ipu) const { return m_errors[ib].statusError(ipu);}
    const int  bcidError(int ib) const { return m_errors[ib].bcidError();}
    const int  bcidError(int ib,int ipu) const { return m_errors[ib].bcidError(ipu);}
    
    const ProcCandErrors * errors(int ib) const {return &m_errors[ib];}

  private:

    int m_quarter;
    // Input candidate registers from the PU Cnv (max=48)
    CandRegisterHandler m_candRegHandlerPU[12][4];
    // Input candidate registers from the BCSU Cnv (max=12)
    CandRegisterHandler m_candRegHandlerBCSU[12];  // filled by the emulator

    ProcCandErrors m_errors[12];

    int m_ref_l0_B_Id;
    int m_ref_l0EventNumber;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_PROCCANDCNV_H
