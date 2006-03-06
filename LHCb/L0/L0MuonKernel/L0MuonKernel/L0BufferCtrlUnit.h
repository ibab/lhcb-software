#ifndef L0MUONKERNEL_L0BUFFERCTRLUNIT_H
#define L0MUONKERNEL_L0BUFFERCTRLUNIT_H 

/** @class L0BufferCtrlUnit L0BufferCtrlUnit.h  L0MuonKernel/L0BufferCtrlUnit.h
 
  class for constructing the L0Buffer 

  @author
  @date

*/
#include <stdio.h>
#include <boost/dynamic_bitset.hpp>
#include "L0MuonKernel/L0BufferUnit.h"
#include "L0MuonKernel/CandRegisterHandler.h"

namespace L0Muon {

  // Field size
  // ----------

  // Identification
  static const ContentType BitsL0BufCuL0BID     = 12;
  static const ContentType BitsL0BufCuL0EVTNUM  = 12;
  static const ContentType BitsL0BufCuSIM       =  1;

  // Candidate
  static const ContentType BitsL0BufCuADD    = 7;
  static const ContentType BitsL0BufCuPT     = 7;
  static const ContentType BitsL0BufCuBCID   = 4;
  static const ContentType BitsL0BufCuCANDPAIR  = 3*16;

  // Error
  static const ContentType BitsL0BufCuERROR    = 3*16;

  // Field position
  // --------------

  // Outputs
  static const ContentType ShiftL0BufCuOUTPUT    = 0 ;

  // Inputs
  static const ContentType ShiftL0BufCuINPUT     = ShiftL0BufCuOUTPUT + BitsL0BufCuCANDPAIR  ;

  // Error
  static const ContentType ShiftL0BufCuERROR     = ShiftL0BufCuINPUT + 12 * BitsL0BufCuCANDPAIR;
  
  // Identification
  static const ContentType ShiftL0BufCuL0BID     = ShiftL0BufCuERROR    + BitsL0BufCuERROR    +  0;
  static const ContentType ShiftL0BufCuL0EVTNUM  = ShiftL0BufCuL0BID    + BitsL0BufCuL0BID    +  4;
  static const ContentType ShiftL0BufCuSIM       = ShiftL0BufCuL0EVTNUM + BitsL0BufCuL0EVTNUM +  3;

  // Encoding of candidate pair
  // --------------------------

  static const ContentType RelShiftL0BufCuBCID   =  0  ;
  static const ContentType RelShiftL0BufCuPT2    = RelShiftL0BufCuBCID  + BitsL0BufCuBCID  + 12;
  static const ContentType RelShiftL0BufCuADD2   = RelShiftL0BufCuPT2   + BitsL0BufCuPT    +  1;
  static const ContentType RelShiftL0BufCuPT1    = RelShiftL0BufCuADD2  + BitsL0BufCuADD   +  1;
  static const ContentType RelShiftL0BufCuADD1   = RelShiftL0BufCuPT1   + BitsL0BufCuPT    +  1;

  static const int L0BufferCtrlSize  =  44*16 ;

  class L0BufferCtrlUnit : public L0BufferUnit {

  public:

    /// Default Constructor
    L0BufferCtrlUnit();

    /// Constructor
    L0BufferCtrlUnit(LHCb::MuonTileID id,int l0BufferMode=0);

    /// Constructor
    L0BufferCtrlUnit(DOMNode* pNode);

    /// Destructor
    ~L0BufferCtrlUnit();

    void initialize();
    void execute();

   /// Give a static type name to the unit
    std::string type() {
      return "L0BufferCtrlUnit";
    }

    /// Give a static size to the L0Buffer
    int bufferSize() { return L0BufferCtrlSize;}

  private:

    // Input candidate registers from the BCSU Unit (max=12)
    std::map<int,CandRegisterHandler> m_candRegHandlerBCSU;
    // Input candidate register from the Ctrl Unit
    CandRegisterHandler m_candRegHandlerCtrl;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_L0BUFFERCTRLUNIT_H
