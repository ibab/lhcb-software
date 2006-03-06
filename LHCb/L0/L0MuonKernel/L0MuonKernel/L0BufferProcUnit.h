#ifndef L0MUONKERNEL_L0BUFFERPROCUNIT_H
#define L0MUONKERNEL_L0BUFFERPROCUNIT_H 

/** @class L0BufferProcUnit L0BufferProcUnit.h  L0MuonKernel/L0BufferProcUnit.h
 
  class for constructing the L0Buffer 

  @author
  @date

*/
#include <stdio.h>
#include <boost/dynamic_bitset.hpp>
#include "L0MuonKernel/L0BufferUnit.h"
#include "L0MuonKernel/CandRegisterHandler.h"

namespace L0Muon {


  //static const ContentType BitsL0BufProcTot       = 0;
  

  // Encoding of candidate address
  // -----------------------------

  static const ContentType BitsL0BufProcADDM3      =  7;
  static const ContentType BitsL0BufProcOFFM2      =  4;
  static const ContentType BitsL0BufProcOFFM1      =  4;
  static const ContentType BitsL0BufProcCAND       = BitsL0BufProcADDM3 + BitsL0BufProcOFFM2 + BitsL0BufProcOFFM1 ;

  static const ContentType RelShiftL0BufProcOFFM1  =  0;
  static const ContentType RelShiftL0BufProcOFFM2  = RelShiftL0BufProcOFFM1   + BitsL0BufProcOFFM1 ;
  static const ContentType RelShiftL0BufProcADDM3  = RelShiftL0BufProcOFFM2   + BitsL0BufProcOFFM2 ;
  static const ContentType RelShiftL0BufProcCAND   = RelShiftL0BufProcADDM3   + BitsL0BufProcADDM3 + 1;

  static const ContentType RelMaskL0BufProcADDM3   =  
  ( ( ( (ContentType) 1 ) <<  BitsL0BufProcADDM3    ) - 1  ) << RelShiftL0BufProcADDM3    ;
  static const ContentType RelMaskL0BufProcOFFM2   =  
  ( ( ( (ContentType) 1 ) <<  BitsL0BufProcOFFM2    ) - 1  ) << RelShiftL0BufProcOFFM2    ;
  static const ContentType RelMaskL0BufProcOFFM1   =  
  ( ( ( (ContentType) 1 ) <<  BitsL0BufProcOFFM1    ) - 1  ) << RelShiftL0BufProcOFFM1    ;


  // Field size
  // ----------

  // Identification
  static const ContentType BitsL0BufProcL0EVTNUM   = 12;
  static const ContentType BitsL0BufProcL0BID      = 12;
  static const ContentType BitsL0BufProcTEST       =  1;
                                                   
  // Candidates 
  static const ContentType BitsL0BufProcCAND1      = BitsL0BufProcCAND;
  static const ContentType BitsL0BufProcCAND2      = BitsL0BufProcCAND;
  static const ContentType BitsL0BufProcBCID       =  4;
  static const ContentType BitsL0BufProcSTATUS     =  4;
                                                   
  // Errors 
  static const ContentType BitsL0BufProcOLERROR    =  8;
  static const ContentType BitsL0BufProcPARERROR   =  8;
  static const ContentType BitsL0BufProcSERERROR   =  6;

  // Input data
  static const ContentType BitsL0BufProcINPUTDATA  = 29*16;
  
  // Field position
  // --------------

  // Input data
  static const ContentType ShiftL0BufProcINPUTDATA = 0;
  
  // Errors 
  static const ContentType ShiftL0BufProcPARERROR  = ShiftL0BufProcINPUTDATA + BitsL0BufProcINPUTDATA  + 0;
  static const ContentType ShiftL0BufProcSERERROR  = ShiftL0BufProcPARERROR  + BitsL0BufProcPARERROR   + 0;
  static const ContentType ShiftL0BufProcOLERROR   = ShiftL0BufProcSERERROR  + BitsL0BufProcSERERROR   + 2;

  // Output candidates 
  static const ContentType ShiftL0BufProcBCID      = ShiftL0BufProcOLERROR +  BitsL0BufProcOLERROR + 8;
  static const ContentType ShiftL0BufProcSTATUS    = ShiftL0BufProcBCID    +  BitsL0BufProcBCID    + 0;
  static const ContentType ShiftL0BufProcCAND2     = ShiftL0BufProcSTATUS  +  BitsL0BufProcSTATUS  + 8;
  static const ContentType ShiftL0BufProcCAND1     = ShiftL0BufProcCAND2   +  BitsL0BufProcCAND2   + 1;


  // Identification
  static const ContentType ShiftL0BufProcL0BID     = ShiftL0BufProcCAND1    + BitsL0BufProcCAND1    + 1;
  static const ContentType ShiftL0BufProcL0EVTNUM  = ShiftL0BufProcL0BID    + BitsL0BufProcL0BID    + 4;
  static const ContentType ShiftL0BufProcTEST      = ShiftL0BufProcL0EVTNUM + BitsL0BufProcL0EVTNUM + 3;

  static const int L0BufferProcSize  =  36*16 ;

  class L0BufferProcUnit : public L0BufferUnit {

  public:

    /// Default Constructor
    L0BufferProcUnit();

    /// Constructor
    L0BufferProcUnit(LHCb::MuonTileID id, int l0BufferMode=0);

    /// Constructor
    L0BufferProcUnit(DOMNode* pNode);

    /// Destructor
    ~L0BufferProcUnit();

    void initialize();
    void execute();
    void postexecute();
 
   /// Give a static type name to the unit
    std::string type() {
      return "L0BufferProcUnit";
    }

    /// Give a static size to the L0Buffer
    int bufferSize() { return L0BufferProcSize;}

  private:

    // Input candidate register from the Proc Unit
    CandRegisterHandler m_candRegHandlerIn;


  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_L0BUFFERPROCUNIT_H
