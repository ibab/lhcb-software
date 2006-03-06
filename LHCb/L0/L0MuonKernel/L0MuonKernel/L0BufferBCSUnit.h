#ifndef L0MUONKERNEL_L0BUFFERBCSUNIT_H
#define L0MUONKERNEL_L0BUFFERBCSUNIT_H 

/** @class L0BufferBCSUnit L0BufferBCSUnit.h  L0MuonKernel/L0BufferBCSUnit.h
 
  class for constructing the L0Buffer 

  @author
  @date

*/
#include "L0MuonKernel/L0BufferUnit.h"
#include "L0MuonKernel/CandRegisterHandler.h"

namespace L0Muon {

  // Encoding of candidate address
  // -----------------------------

  static const ContentType BitsL0BufBcsuADDM3      =  7;
  static const ContentType BitsL0BufBcsuOFFM2      =  4;
  static const ContentType BitsL0BufBcsuOFFM1      =  4;
  static const ContentType BitsL0BufBcsuCAND       = BitsL0BufBcsuADDM3 + BitsL0BufBcsuOFFM2 + BitsL0BufBcsuOFFM1 ; // 7+4+4=15

  static const ContentType RelShiftL0BufBcsuOFFM1  =  0;
  static const ContentType RelShiftL0BufBcsuOFFM2  = RelShiftL0BufBcsuOFFM1   + BitsL0BufBcsuOFFM1 ;
  static const ContentType RelShiftL0BufBcsuADDM3  = RelShiftL0BufBcsuOFFM2   + BitsL0BufBcsuOFFM2 ;
  static const ContentType RelShiftL0BufBcsuCAND   = RelShiftL0BufBcsuADDM3   + BitsL0BufBcsuADDM3 + 1;

  static const ContentType RelMaskL0BufBcsuADDM3   =  
  ( ( ( (ContentType) 1 ) <<  BitsL0BufBcsuADDM3    ) - 1  ) << RelShiftL0BufBcsuADDM3    ;
  static const ContentType RelMaskL0BufBcsuOFFM2   =  
  ( ( ( (ContentType) 1 ) <<  BitsL0BufBcsuOFFM2    ) - 1  ) << RelShiftL0BufBcsuOFFM2    ;
  static const ContentType RelMaskL0BufBcsuOFFM1   =  
  ( ( ( (ContentType) 1 ) <<  BitsL0BufBcsuOFFM1    ) - 1  ) << RelShiftL0BufBcsuOFFM1    ;


  // Field size
  // ----------

  // Identification
  static const ContentType BitsL0BufBcsuL0EVTNUM  = 12;
  static const ContentType BitsL0BufBcsuL0BID     = 12;
  static const ContentType BitsL0BufBcsuSIM       =  1;

  // Candidates 
  static const ContentType BitsL0BufBcsuADD       = BitsL0BufBcsuADDM3 + BitsL0BufBcsuOFFM2 + BitsL0BufBcsuOFFM1 ;
  static const ContentType BitsL0BufBcsuPT        =  7;
  static const ContentType BitsL0BufBcsuCHARGE    =  1;
  static const ContentType BitsL0BufBcsuPU        =  2;
  static const ContentType BitsL0BufBcsuBCID      =  4;
  static const ContentType BitsL0BufBcsuSTATUS    =  4;
  static const ContentType BitsL0BufBcsuCANDPAIR  = 4*16;


  // Field position
  // --------------

  // Candidates 
  static const ContentType ShiftL0BufBcsuINPUT     =  0 ;
  static const ContentType ShiftL0BufBcsuOUTPUT    = 4*BitsL0BufBcsuCANDPAIR  ;

  // Identification
  static const ContentType ShiftL0BufBcsuL0BID     = ShiftL0BufBcsuOUTPUT   + BitsL0BufBcsuCANDPAIR + 0;
  static const ContentType ShiftL0BufBcsuL0EVTNUM  = ShiftL0BufBcsuL0BID    + BitsL0BufBcsuL0BID    + 4;
  static const ContentType ShiftL0BufBcsuSIM       = ShiftL0BufBcsuL0EVTNUM + BitsL0BufBcsuL0EVTNUM + 3;

  // Encoding of candidate pair
  // --------------------------

  static const ContentType RelShiftL0BufBcsuBCID     = 0;
  static const ContentType RelShiftL0BufBcsuSTATUS   = RelShiftL0BufBcsuBCID     + BitsL0BufBcsuBCID    + 0;
  static const ContentType RelShiftL0BufBcsuPU2      = RelShiftL0BufBcsuSTATUS   + BitsL0BufBcsuSTATUS  + 0;
  static const ContentType RelShiftL0BufBcsuPU1      = RelShiftL0BufBcsuPU2      + BitsL0BufBcsuPU      + 2;
  static const ContentType RelShiftL0BufBcsuADD2     = RelShiftL0BufBcsuPU1      + BitsL0BufBcsuPU      + 2;
  static const ContentType RelShiftL0BufBcsuADD1     = RelShiftL0BufBcsuADD2     + BitsL0BufBcsuADD     + 1;
  static const ContentType RelShiftL0BufBcsuPT2      = RelShiftL0BufBcsuADD1     + BitsL0BufBcsuADD     + 1;
  static const ContentType RelShiftL0BufBcsuCHARGE2  = RelShiftL0BufBcsuPT2      + BitsL0BufBcsuPT      + 0;
  static const ContentType RelShiftL0BufBcsuPT1      = RelShiftL0BufBcsuCHARGE2  + BitsL0BufBcsuCHARGE  + 0;
  static const ContentType RelShiftL0BufBcsuCHARGE1  = RelShiftL0BufBcsuPT1      + BitsL0BufBcsuPT      + 0;

  static const int L0BufferBCSUSize  =  22*16 ;

  class L0BufferBCSUnit : public L0BufferUnit {

  public:

    /// Default Constructor
    L0BufferBCSUnit();

    /// Constructor
    L0BufferBCSUnit(LHCb::MuonTileID id, int l0BufferMode=0);

    /// Constructor
    L0BufferBCSUnit(DOMNode* pNode);

    /// Destructor
    ~L0BufferBCSUnit();

    void initialize();
    void execute();

   /// Give a static type name to the unit
    std::string type() {
      return "L0BufferBCSUnit";
    }

    /// Give a static size to the L0Buffer
    int bufferSize() { return L0BufferBCSUSize;}

  private:

    // Input candidate registers from the Proc/Core Unit (max=4)
    std::map<int,CandRegisterHandler> m_candRegHandlerProc;
    // Input candidate register from the BCSU Unit 
    CandRegisterHandler m_candRegHandlerBCSU;


  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_L0BUFFERBCSUNIT_H
