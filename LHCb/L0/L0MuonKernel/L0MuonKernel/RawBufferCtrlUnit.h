#ifndef L0MUONKERNEL_RAWBUFFERCTRLUNIT_H
#define L0MUONKERNEL_RAWBUFFERCTRLUNIT_H 

/** @class RawBufferCtrlUnit RawBufferCtrlUnit.h  L0MuonKernel/RawBufferCtrlUnit.h
 
  class for constructing the L0Buffer 

  @author
  @date

*/
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/CandRegisterHandler.h"
#include <boost/dynamic_bitset.hpp>

namespace L0Muon {
  
  static const ContentType BitsRawBufCtrlTot       = 0;

  static const ContentType BitsRawBufCtrlL0EVTNUM  = 14;
  static const ContentType BitsRawBufCtrlL0BID     = 14;
  static const ContentType BitsRawBufCtrlIDCUERROR =  4;
  static const ContentType BitsRawBufCtrlIDSUERROR =  4;
  static const ContentType BitsRawBufCtrlNCTRLCAND =  4;

  
  static const ContentType BitsRawBufCtrlPT      =  7;
  static const ContentType BitsRawBufCtrlCHARGE  =  1;
  static const ContentType BitsRawBufCtrlADDM3   =  7;
  static const ContentType BitsRawBufCtrlOFFM2   =  4;
  static const ContentType BitsRawBufCtrlOFFM1   =  4;
  static const ContentType BitsRawBufCtrlPU      =  2;
  static const ContentType BitsRawBufCtrlBOARD   =  4;
  static const ContentType BitsRawBufCtrlQUARTER =  2;

  static const ContentType BitsRawBufCtrlHEADER  = BitsRawBufCtrlL0EVTNUM + BitsRawBufCtrlL0BID
                                                  +BitsRawBufCtrlIDCUERROR + BitsRawBufCtrlIDSUERROR;
  static const ContentType BitsRawBufCtrlCAND    = BitsRawBufCtrlPT + BitsRawBufCtrlCHARGE
                                                  +BitsRawBufCtrlADDM3 + BitsRawBufCtrlOFFM2 + BitsRawBufCtrlOFFM1
                                                  +BitsRawBufCtrlPU + BitsRawBufCtrlBOARD + BitsRawBufCtrlQUARTER;


  static const ContentType ShiftRawBufCtrlL0EVTNUM   = 0;
  static const ContentType ShiftRawBufCtrlL0BID      = ShiftRawBufCtrlL0EVTNUM  + BitsRawBufCtrlL0EVTNUM  ;
  static const ContentType ShiftRawBufCtrlIDCUERROR  = ShiftRawBufCtrlL0BID     + BitsRawBufCtrlL0BID     ;
  static const ContentType ShiftRawBufCtrlIDSUERROR  = ShiftRawBufCtrlIDCUERROR + BitsRawBufCtrlIDCUERROR ;
  static const ContentType ShiftRawBufCtrlNCTRLCAND  = ShiftRawBufCtrlIDSUERROR + BitsRawBufCtrlIDSUERROR ;
  static const ContentType ShiftRawBufCtrlFIRSTCAND  = ShiftRawBufCtrlNCTRLCAND + BitsRawBufCtrlNCTRLCAND ;

  static const ContentType RelShiftRawBufCtrlPT      =  0 ;
  static const ContentType RelShiftRawBufCtrlCHARGE  =  RelShiftRawBufCtrlPT      + BitsRawBufCtrlPT     ;
  static const ContentType RelShiftRawBufCtrlADDM3   =  RelShiftRawBufCtrlCHARGE  + BitsRawBufCtrlCHARGE ;
  static const ContentType RelShiftRawBufCtrlOFFM2   =  RelShiftRawBufCtrlADDM3   + BitsRawBufCtrlADDM3  ;
  static const ContentType RelShiftRawBufCtrlOFFM1   =  RelShiftRawBufCtrlOFFM2   + BitsRawBufCtrlOFFM2  ;
  static const ContentType RelShiftRawBufCtrlPU      =  RelShiftRawBufCtrlOFFM1   + BitsRawBufCtrlOFFM1  ;
  static const ContentType RelShiftRawBufCtrlBOARD   =  RelShiftRawBufCtrlPU      + BitsRawBufCtrlPU     ;
  static const ContentType RelShiftRawBufCtrlQUARTER =  RelShiftRawBufCtrlBOARD   + BitsRawBufCtrlBOARD  ;
  static const ContentType RelShiftRawBufCtrlCAND    =  RelShiftRawBufCtrlQUARTER + BitsRawBufCtrlQUARTER;

  static const ContentType RelMaskRawBufCtrlPT      =  
  ( ( ( (ContentType) 1 ) <<  BitsRawBufCtrlPT       ) - 1  ) << RelShiftRawBufCtrlPT       ;
  static const ContentType RelMaskRawBufCtrlCHARGE  =  
  ( ( ( (ContentType) 1 ) <<  BitsRawBufCtrlCHARGE   ) - 1  ) << RelShiftRawBufCtrlCHARGE   ;
  static const ContentType RelMaskRawBufCtrlADDM3   =  
  ( ( ( (ContentType) 1 ) <<  BitsRawBufCtrlADDM3    ) - 1  ) << RelShiftRawBufCtrlADDM3    ;
  static const ContentType RelMaskRawBufCtrlOFFM2   =  
  ( ( ( (ContentType) 1 ) <<  BitsRawBufCtrlOFFM2    ) - 1  ) << RelShiftRawBufCtrlOFFM2    ;
  static const ContentType RelMaskRawBufCtrlOFFM1   =  
  ( ( ( (ContentType) 1 ) <<  BitsRawBufCtrlOFFM1    ) - 1  ) << RelShiftRawBufCtrlOFFM1    ;
  static const ContentType RelMaskRawBufCtrlPU      =  
  ( ( ( (ContentType) 1 ) <<  BitsRawBufCtrlPU       ) - 1  ) << RelShiftRawBufCtrlPU       ;
  static const ContentType RelMaskRawBufCtrlBOARD   =  
  ( ( ( (ContentType) 1 ) <<  BitsRawBufCtrlBOARD    ) - 1  ) << RelShiftRawBufCtrlBOARD    ;
  static const ContentType RelMaskRawBufCtrlQUARTER =
  ( ( ( (ContentType) 1 ) <<  BitsRawBufCtrlQUARTER  ) - 1  ) << RelShiftRawBufCtrlQUARTER  ;
  static const ContentType RelMaskRawBufCtrlCAND    = 
  ( ( ( (ContentType) 1 ) <<  BitsRawBufCtrlCAND     ) - 1  ) << RelShiftRawBufCtrlCAND     ;


  class RawBufferCtrlUnit : public L0MUnit {

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
    void postexecute();

   /// Give a static type name to the unit
    std::string type() {
      return "RawBufferCtrlUnit";
    }
    
  private:
    unsigned long formattedCandidate(CandRegisterHandler candRegHandler, int icand, int quarter);

  private:

    unsigned long m_evtCounter; // Evt counter

    // Input candidate registers from the BCSU Unit (max=48)
    std::map<std::pair<int,int>, CandRegisterHandler> m_candRegHandlerBCSU;
    // Input candidate registers from the Ctrl Unit (max=4)
    std::map<int,CandRegisterHandler> m_candRegHandlerCtrl;


  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_RAWBUFFERCTRLUNIT_H
