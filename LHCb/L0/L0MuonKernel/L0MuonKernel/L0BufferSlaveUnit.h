#ifndef L0MUONKERNEL_L0BUFFERSLAVEUNIT_H
#define L0MUONKERNEL_L0BUFFERSLAVEUNIT_H 

/** @class L0BufferSlaveUnit L0BufferSlaveUnit.h  L0MuonKernel/L0BufferSlaveUnit.h
 
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
  static const ContentType BitsL0BufSuL0BID     = 12;
  static const ContentType BitsL0BufSuL0EVTNUM  = 12;
  static const ContentType BitsL0BufSuSIM       =  1;

  // Candidate
  static const ContentType BitsL0BufSuOFFM1  = 4;
  static const ContentType BitsL0BufSuOFFM2  = 4;
  static const ContentType BitsL0BufSuPU     = 2;
  static const ContentType BitsL0BufSuCHARGE = 1;
  static const ContentType BitsL0BufSuBOARD  = 4;
  static const ContentType BitsL0BufSuBCID   = 4;
  static const ContentType BitsL0BufSuSTATUS = 4;
  static const ContentType BitsL0BufSuCANDPAIR = 3*16;

  // Error
  static const ContentType BitsL0BufSuERROR    = 4*16;

  // Field position
  // --------------

  // Outputs
  static const ContentType ShiftL0BufSuOUTPUT    = 0 ;

  // Inputs 
  static const ContentType ShiftL0BufSuINPUT     = ShiftL0BufSuOUTPUT + BitsL0BufSuCANDPAIR  ;

  // Error
  static const ContentType ShiftL0BufSuERROR     = ShiftL0BufSuINPUT + 12 * BitsL0BufSuCANDPAIR;
  
  // Identification
  static const ContentType ShiftL0BufSuL0BID     = ShiftL0BufSuERROR    + BitsL0BufSuERROR    + 0;
  static const ContentType ShiftL0BufSuL0EVTNUM  = ShiftL0BufSuL0BID    + BitsL0BufSuL0BID    + 4;
  static const ContentType ShiftL0BufSuSIM       = ShiftL0BufSuL0EVTNUM + BitsL0BufSuL0EVTNUM + 3;

  // Encoding of candidate pair
  // --------------------------

  static const ContentType RelShiftL0BufSuBCID     =  0;
  static const ContentType RelShiftL0BufSuSTATUS   =  RelShiftL0BufSuBCID    + BitsL0BufSuBCID   + 0;
  static const ContentType RelShiftL0BufSuCHARGE2  =  RelShiftL0BufSuSTATUS  + BitsL0BufSuSTATUS + 0;
  static const ContentType RelShiftL0BufSuCHARGE1  =  RelShiftL0BufSuCHARGE2 + BitsL0BufSuCHARGE + 3;
  static const ContentType RelShiftL0BufSuOFFM1_2  =  RelShiftL0BufSuCHARGE1 + BitsL0BufSuCHARGE + 3;
  static const ContentType RelShiftL0BufSuOFFM2_2  =  RelShiftL0BufSuOFFM1_2 + BitsL0BufSuOFFM1  + 0;
  static const ContentType RelShiftL0BufSuPU2      =  RelShiftL0BufSuOFFM2_2 + BitsL0BufSuOFFM2  + 0;
  static const ContentType RelShiftL0BufSuBOARD2   =  RelShiftL0BufSuPU2     + BitsL0BufSuPU     + 2;
  static const ContentType RelShiftL0BufSuOFFM1_1  =  RelShiftL0BufSuBOARD2  + BitsL0BufSuBOARD  + 0;
  static const ContentType RelShiftL0BufSuOFFM2_1  =  RelShiftL0BufSuOFFM1_1 + BitsL0BufSuOFFM1  + 0;
  static const ContentType RelShiftL0BufSuPU1      =  RelShiftL0BufSuOFFM2_1 + BitsL0BufSuOFFM2  + 0;
  static const ContentType RelShiftL0BufSuBOARD1   =  RelShiftL0BufSuPU1     + BitsL0BufSuPU     + 2;

  static const int L0BufferSlaveSize  =  45*16 ;
  

class L0BufferSlaveUnit : public L0BufferUnit {

  public:

    /// Default Constructor
    L0BufferSlaveUnit();

    /// Constructor
    L0BufferSlaveUnit(LHCb::MuonTileID id, int l0BufferMode=0);

    /// Constructor
    L0BufferSlaveUnit(DOMNode* pNode);

    /// Destructor
    ~L0BufferSlaveUnit();

    void initialize();
    void execute();

   /// Give a static type name to the unit
    std::string type() {
      return "L0BufferSlaveUnit";
    }

    /// Give a static size to the L0Buffer
    int bufferSize() { return L0BufferSlaveSize;}

  private:

    // Input candidate registers from the BCSU Unit (max=12)
    std::map<int,CandRegisterHandler> m_candRegHandlerBCSU;
    // Input candidate register from the Ctrl Unit
    CandRegisterHandler m_candRegHandlerCtrl;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_L0BUFFERSLAVEUNIT_H
