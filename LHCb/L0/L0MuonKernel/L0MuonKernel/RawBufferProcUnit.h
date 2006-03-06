#ifndef L0MUONKERNEL_RAWBUFFERPROCUNIT_H
#define L0MUONKERNEL_RAWBUFFERPROCUNIT_H 

/** @class RawBufferProcUnit RawBufferProcUnit.h  L0MuonKernel/RawBufferProcUnit.h
 
  class for constructing the L0Buffer 

  @author
  @date

*/
#include "L0MuonKernel/RawBufferUnit.h"
#include "L0MuonKernel/CandRegisterHandler.h"

namespace L0Muon {

  typedef unsigned int   ContentType ; 

  static const ContentType BitsRawBufProcTot = 0;

  static const ContentType BitsRawBufProcL0EVTNUM  = 14;
  static const ContentType BitsRawBufProcL0BID     = 14;
  static const ContentType BitsRawBufProcIDERROR   =  1;
  static const ContentType BitsRawBufProcNBCSUCAND =  5;
  static const ContentType BitsRawBufProcNPUCAND   =  6;
  static const ContentType BitsRawBufProcPROCID    = 48;

  static const ContentType BitsRawBufProcHEADER  = BitsRawBufProcL0EVTNUM + BitsRawBufProcL0BID
                                                  +BitsRawBufProcIDERROR;

  static const ContentType ShiftRawBufProcL0EVTNUM   = 0;
  static const ContentType ShiftRawBufProcL0BID      = ShiftRawBufProcL0EVTNUM  + BitsRawBufProcL0EVTNUM  ;
  static const ContentType ShiftRawBufProcIDERROR    = ShiftRawBufProcL0BID     + BitsRawBufProcL0BID     ;
  static const ContentType ShiftRawBufProcNBCSUCAND  = ShiftRawBufProcIDERROR   + BitsRawBufProcIDERROR ;
  static const ContentType ShiftRawBufProcFIRSTCAND  = ShiftRawBufProcNBCSUCAND + BitsRawBufProcNBCSUCAND ;

  // Input data
  static const ContentType BitsRawBufProcINPUTDATA   = 29*16;
 
  
  class RawBufferProcUnit : public RawBufferUnit {

  public:

    /// Default Constructor
    RawBufferProcUnit();

    /// Constructor
    RawBufferProcUnit(LHCb::MuonTileID id);

    /// Constructor
    RawBufferProcUnit(DOMNode* pNode);

    /// Destructor
    ~RawBufferProcUnit();

    void initialize();
    void execute();

   /// Give a static type name to the unit
    std::string type() {
      return "RawBufferProcUnit";
    }

  private:
    // Input candidate registers from the Proc/core Unit (max=48)
    std::map<std::pair<int,int>, CandRegisterHandler> m_candRegHandlerProc;
    // Input candidate registers from the BCSU Unit (max=12)
    std::map<int,CandRegisterHandler> m_candRegHandlerBCSU;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_RAWBUFFERPROCUNIT_H
