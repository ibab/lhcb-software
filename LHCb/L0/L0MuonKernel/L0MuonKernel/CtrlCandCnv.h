#ifndef L0MUONKERNEL_CTRLFINALCNV_H
#define L0MUONKERNEL_CTRLFINALCNV_H 

#include "L0MuonKernel/CandRegisterHandler.h"
#include "L0MuonKernel/MuonCandidate.h"
#include <string>
#include <map>
#include <vector>
#include "L0MuonKernel/CtrlCandErrors.h"

namespace L0Muon {
  
  /** @class CtrlCandCnv CtrlCandCnv.h  L0MuonKernel/CtrlCandCnv.h
 
  Class to interface the data in the L0MuonCtrlCand bank with the Register instances.

  The L0MuonCtrlCand bank content is described in the L0Muon TELL1 specifications (see EDMS 818559).

  As there is one L0MuonCtrlCand per side (i.e. for 2 quarters), this class handle a side (index stored in #m_side).
  Therefore, it deals with 2 quarters :
  - side A (m_side=0) : Q1 (iq=0) and Q2 (iq=1)
  - side C (m_side=1) : Q3 (iq=0) and Q4 (iq=1)

  It holds pointers to the Register instances containing the data written in the L0MuonCtrlCand bank :
  - CAND_CTRLQ<q>, where q(=0,...,3) is the quarter : candidates selected by the controller board in quarter q
  - BOARDCAND_Q<q>_<ib>, where q(=0,...,3) is the quarter and ib(=0,...,11) is the board index in the quarter : 
  candidates in quarter q sent by the board ib to the controller board     

  It also owns the #m_errors attribute which stores the errors related to the L0MuonCtrlCand bank 
  (either errors reported by the hardware or which occured during the decoding itself) in CtrlCandErrors objects. 
  
  It implements methods to :
  - access the informations contained in the Register instances described above :
    - list of muon candidate selected by the controller board (CtrlCandCnv::muonCandidates)
    - list of muon candidate received by the controller board (CtrlCandCnv::muonCandidatesBCSU)) 
  - fill these registers from the data in the L0MuonCtrlCand bank (bank decoding) (CtrlCandCnv::decodeBank)
  - fill the L0MuonCtrlCand bank (CtrlCandCnv::rawBank)

  It is used by the L0MuonOutput tool in the L0/L0Muon package (belonging to Lbcom project).

  @author Julien Cogan
  @date 2008 June the 5th
  
  */
  class CtrlCandCnv  {

  public:

    /// Default Constructor
    CtrlCandCnv();

    /** Constructor with side identifier (side A=0, side C=1)

    @param side : identifier of the side attached to this converter (0 for A side, 1 for C side)
    */
    CtrlCandCnv(int side);

    /// Destructor
    ~CtrlCandCnv();

    /** Returns the MuonTileID of the board (BCSU) defined by its quarter and board index
       
        @param iq : index of the quarter in the side (0 to 1)
        @param ib : index of the board (0 to 11)
     */
    LHCb::MuonTileID mid_BCSU(int iq, int ib);
    
    /// Clear the registers
    void release();
    
    /// Returns the muon candidates selected by both quarters of the side attached to this converter 
    std::vector<PMuonCandidate> muonCandidates();

    /** Returns the muon candidates selected by quarter iq
        
        @param iq : index of the quarter in the side (0 to 1)
    */
    std::vector<PMuonCandidate> muonCandidates(int iq);

    /// Returns the muon candidates sent by the BCSU of both quarters of the side attached to this converter
    std::vector<PMuonCandidate> muonCandidatesBCSU();

    /** Returns the muon candidates sent by the BCSU of quarter iq
        
        @param iq : index of the quarter in the side (0 to 1)
    */
    std::vector<PMuonCandidate> muonCandidatesBCSU(int iq);

    /** Returns the muon candidates sent by the BCSU of board ib in quarter iq

        @param iq : index of the quarter in the side (0 to 1)
        @param ib : index of the board (0 to 11)
    */
    std::vector<PMuonCandidate> muonCandidatesBCSU(int iq, int ib);

    /** Decode the L0MuonCtrlCand bank. 

        From the data in the bank, it :
        - fills the registers with the candidates selected by controller board
        - fills the registers with the candidates sent by the processing board (if mode>1)
        - fills the CtrlCandErrors errors with the error encountered in the decoding 

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
    int decodeBank(const std::vector<unsigned int> &raw, int version,int mode);

    /** Write the L0MuonCtrlCand bank data.
        Return values :
        - 0 : error
        - 1 : only 1st first part was written
        - 2 : the two parts were written

        @param raw : vector of unsigned int to be filled with the L0MuonCtrlCand bank data
        @param ievt : current event number
        @param mode : decoding mode (<=1: decode only the 1st part of the bank, >1: decode both parts)
        @param version : version of the L0MuonCtrlCand bank to write
        @param compression : flag to activate data compression
    */
    int rawBank(std::vector<unsigned int> &raw, int ievt, int version, int mode, bool compression);

    /// Obsolete versions only used in the simulation (before commissionning)
    int decodeBankDC06(const std::vector<unsigned int> &raw, int version);
    /// Obsolete versions only used in the simulation (before commissionning)
    int rawBankDC06(std::vector<unsigned int> &raw);
    /// Obsolete versions only used in the simulation (before commissionning)
    int decodeBankFinalCandidates(const std::vector<unsigned int> &raw, int version);
    /// Obsolete versions only used in the simulation (before commissionning)
    int rawBankFinalCandidates(std::vector<unsigned int> &raw,  int ievt);
    /// Obsolete versions only used in the simulation (before commissionning)
    int decodeBankBcsuCandidates(const std::vector<unsigned int> &raw, int version);
    /// Obsolete versions only used in the simulation (before commissionning)
    int rawBankBcsuCandidates(std::vector<unsigned int> &raw);

    /// Dump the TileRegisters
    void dump(std::string tab="") {
      for (int i= 0; i<2; ++i) {
        m_candRegHandler[i].dump(-1,tab);
        for (int ib= 0; ib<12; ++ib) {
          m_candRegHandlerBCSU[i][ib].dump(-1,tab);
        } 
      }
    }

    /// Returns the l0_B_Id used as a reference
    int  ref_l0_B_Id(){return m_ref_l0_B_Id;}

    /// Returns the l0EventNumber used as a reference
    int  ref_l0EventNumber() {return m_ref_l0EventNumber;}

    /** Submit a l0_B_Id to be used as a reference; will be used only if no reference is currently defined. 

        @param l0_B_Id : l0_B_Id submitted as a reference 
    */
    void submitL0_B_Id(int l0_B_Id){ if (m_ref_l0_B_Id==-1) m_ref_l0_B_Id=l0_B_Id; }

    /** Submit a l0EventNumber to be used as a reference; will be used only if no reference is currently defined. 
        
        @param l0EventNumber : l0EventNumber submitted as a reference 
    */
    void submitL0EventNumber(int l0EventNumber){ if (m_ref_l0EventNumber==-1) m_ref_l0EventNumber=l0EventNumber; }
    
    /// Clear the reference l0_B_Id and l0EventNumber 
    void clearRef();
    
    /** Returns true if an error was detected in the L0MuonCtrlCand bank part 
        corresponding to the given quarter (see CtrlCandErrors::inError() const).
        
        @param iq : index of the quarter in the side (0 to 1)
    */
    const bool inError(int iq) const { return m_errors[iq].inError();}

    /** Returns true if an error was detected in the L0MuonCtrlCand bank part 
        corresponding to the given board of the given quarter (see CtrlCandErrors::inError(int) const).

        @param iq : index of the quarter in the side (0 to 1)
        @param ib : index of the board (0 to 11)
    */
    const bool inError(int iq, int ib)const { return m_errors[iq].inError(ib);}

    
    /** Returns the decoding error code for the given quarter 
        (see CtrlCandErrors::decodingError() const ).
        
        @param iq : index of the quarter in the side (0 to 1)
    */
    const int  decodingError(int iq) const { return int(m_errors[iq].decodingError());}

    /** Returns the hardware error code for the given quarter 
        (see CtrlCandErrors::hardwareError() const ).

        @param iq : index of the quarter in the side (0 to 1)
    */
    const int  hardwareError(int iq) const {return m_errors[iq].hardwareError();}
 
    /** Returns the hardware error code for the given board of the given quarter 
        (see CtrlCandErrors::hardwareError(int ib) const).

        @param iq : index of the quarter in the side (0 to 1)
        @param ib : index of the board (0 to 11)
    */
    const int  hardwareError(int iq, int ib) const {return m_errors[iq].hardwareError(ib);}

    /** Returns the status error bits for the best candidates of the given quarter 
        (see CtrlCandErrors::statusError() const).

        @param iq : index of the quarter in the side (0 to 1)
        @param ib : index of the board (0 to 11)
    */
    const int  statusError(int iq) const { return m_errors[iq].statusError();}

    /** Returns the status error bits for the candidates of the given board in the given quarter 
        (see CtrlCandErrors::statusError(int) const).

        @param iq : index of the quarter in the side (0 to 1)
        @param ib : index of the board (0 to 11)
    */
    const int  statusError(int iq,int ib) const { return m_errors[iq].statusError(ib);}

    /** Returns the bcid error for the given quarter 
        (see CtrlCandErrors::bcidError() const).

        @param iq : index of the quarter in the side (0 to 1)
    */

    const int  bcidError(int iq) const { return m_errors[iq].bcidError();}

    /** Returns the bcid error for the candidates of the given board in the given quarter 
        (see CtrlCandErrors::bcidError(int) const).

        @param iq : index of the quarter in the side (0 to 1)
        @param ib : index of the board (0 to 11)
    */
    const int  bcidError(int iq,int ib) const { return m_errors[iq].bcidError(ib);}

    /** Returns a pointer to the CtrlCandErrors of the given quarter
        
        @param iq : index of the quarter in the side (0 to 1)
    */
    const CtrlCandErrors * errors(int iq) const {return &m_errors[iq];}
    
  private:
    int m_side; ///< Side attached to the converter (=0 for A side, =1 for C side).

    CandRegisterHandler m_candRegHandler[2]; ///< Final candidates registers (candidates selected by the controller board).

    CandRegisterHandler m_candRegHandlerBCSU[2][12]; ///< BCSU candidates registers (candidates received by the controller board).

    CtrlCandErrors m_errors[2]; ///< Error detected when decoding the L0MuonCtrlCand bank.

    int m_ref_l0_B_Id; ///< Reference l0_B_Id used when decoding the L0MuonCtrlCand bank.

    int m_ref_l0EventNumber; ///< Reference l0EventNumber used when decoding the L0MuonCtrlCand bank.
  
  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_CTRLFINALCNV_H
