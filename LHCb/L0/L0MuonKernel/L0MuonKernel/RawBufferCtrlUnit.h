#ifndef L0MUONKERNEL_RAWBUFFERCTRLUNIT_H
#define L0MUONKERNEL_RAWBUFFERCTRLUNIT_H 

/** @class RawBufferCtrlUnit RawBufferCtrlUnit.h  L0MuonKernel/RawBufferCtrlUnit.h
 
  class for constructing the L0Buffer 

  @author
  @date

*/
#include "L0MuonKernel/RawBufferUnit.h"
#include "L0MuonKernel/CandRegisterHandler.h"
#include <boost/dynamic_bitset.hpp>

namespace L0Muon {
  
  static const ContentType BitsRawBufCtrlTot       = 0;

  static const ContentType BitsRawBufCtrlL0EVTNUM  = 14;
  static const ContentType BitsRawBufCtrlL0BID     = 14;
  static const ContentType BitsRawBufCtrlIDCUERROR =  4;
  static const ContentType BitsRawBufCtrlIDSUERROR =  4;
  static const ContentType BitsRawBufCtrlNCTRLCAND =  4;
  static const ContentType BitsRawBufCtrlNBCSUCAND =  5;

  

  static const ContentType BitsRawBufCtrlHEADER  = BitsRawBufCtrlL0EVTNUM + BitsRawBufCtrlL0BID
                                                  +BitsRawBufCtrlIDCUERROR + BitsRawBufCtrlIDSUERROR;


  static const ContentType ShiftRawBufCtrlL0EVTNUM   = 0;
  static const ContentType ShiftRawBufCtrlL0BID      = ShiftRawBufCtrlL0EVTNUM  + BitsRawBufCtrlL0EVTNUM  ;
  static const ContentType ShiftRawBufCtrlIDCUERROR  = ShiftRawBufCtrlL0BID     + BitsRawBufCtrlL0BID     ;
  static const ContentType ShiftRawBufCtrlIDSUERROR  = ShiftRawBufCtrlIDCUERROR + BitsRawBufCtrlIDCUERROR ;
  static const ContentType ShiftRawBufCtrlNCTRLCAND  = ShiftRawBufCtrlIDSUERROR + BitsRawBufCtrlIDSUERROR ;
  static const ContentType ShiftRawBufCtrlFIRSTCAND  = ShiftRawBufCtrlNCTRLCAND + BitsRawBufCtrlNCTRLCAND ;


  class RawBufferCtrlUnit : public RawBufferUnit {

  public:

    /// Default Constructor
    RawBufferCtrlUnit();

    /// Constructor
    RawBufferCtrlUnit(LHCb::MuonTileID id);

    /// Constructor
    RawBufferCtrlUnit(DOMNode* pNode);

    /// Destructor
    ~RawBufferCtrlUnit();

    void initialize();
    void execute();

   /// Give a static type name to the unit
    std::string type() {
      return "RawBufferCtrlUnit";
    }
    
  private:

    // Input candidate registers from the BCSU Unit (max=48)
    std::map<std::pair<int,int>, CandRegisterHandler> m_candRegHandlerBCSU;
    // Input candidate registers from the Ctrl Unit (max=4)
    std::map<int,CandRegisterHandler> m_candRegHandlerCtrl;


  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_RAWBUFFERCTRLUNIT_H
