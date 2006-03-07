#ifndef L0MUONKERNEL_RAWBUFFERUNIT_H
#define L0MUONKERNEL_RAWBUFFERUNIT_H 

/** @class RawBufferUnit RawBufferlUnit.h  L0MuonKernel/RawBufferUnit.h
 
  class for constructing the L0Buffer 

  @author
  @date

*/
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/CandRegisterHandler.h"
#include <boost/dynamic_bitset.hpp>

namespace L0Muon {

  
  static const ContentType encodingPrefixSizeRawBuf = 5;

  static const ContentType BitsRawBufTot     = 0;
  
  static const ContentType BitsRawBufPT      =  7;
  static const ContentType BitsRawBufCHARGE  =  1;
  static const ContentType BitsRawBufADDM3   =  7;
  static const ContentType BitsRawBufOFFM2   =  4;
  static const ContentType BitsRawBufOFFM1   =  4;
  static const ContentType BitsRawBufPU      =  2;
  static const ContentType BitsRawBufBOARD   =  4;
  static const ContentType BitsRawBufQUARTER =  2;

  static const ContentType BitsRawBufCAND    = BitsRawBufPT + BitsRawBufCHARGE
                                                  +BitsRawBufADDM3 + BitsRawBufOFFM2 + BitsRawBufOFFM1
                                                  +BitsRawBufPU + BitsRawBufBOARD + BitsRawBufQUARTER;


  static const ContentType RelShiftRawBufPT      =  0 ;
  static const ContentType RelShiftRawBufCHARGE  =  RelShiftRawBufPT      + BitsRawBufPT     ;
  static const ContentType RelShiftRawBufADDM3   =  RelShiftRawBufCHARGE  + BitsRawBufCHARGE ;
  static const ContentType RelShiftRawBufOFFM2   =  RelShiftRawBufADDM3   + BitsRawBufADDM3  ;
  static const ContentType RelShiftRawBufOFFM1   =  RelShiftRawBufOFFM2   + BitsRawBufOFFM2  ;
  static const ContentType RelShiftRawBufPU      =  RelShiftRawBufOFFM1   + BitsRawBufOFFM1  ;
  static const ContentType RelShiftRawBufBOARD   =  RelShiftRawBufPU      + BitsRawBufPU     ;
  static const ContentType RelShiftRawBufQUARTER =  RelShiftRawBufBOARD   + BitsRawBufBOARD  ;
  static const ContentType RelShiftRawBufCAND    =  RelShiftRawBufQUARTER + BitsRawBufQUARTER;

  static const ContentType RelMaskRawBufPT      =  
  ( ( ( (ContentType) 1 ) <<  BitsRawBufPT       ) - 1  ) << RelShiftRawBufPT       ;
  static const ContentType RelMaskRawBufCHARGE  =  
  ( ( ( (ContentType) 1 ) <<  BitsRawBufCHARGE   ) - 1  ) << RelShiftRawBufCHARGE   ;
  static const ContentType RelMaskRawBufADDM3   =  
  ( ( ( (ContentType) 1 ) <<  BitsRawBufADDM3    ) - 1  ) << RelShiftRawBufADDM3    ;
  static const ContentType RelMaskRawBufOFFM2   =  
  ( ( ( (ContentType) 1 ) <<  BitsRawBufOFFM2    ) - 1  ) << RelShiftRawBufOFFM2    ;
  static const ContentType RelMaskRawBufOFFM1   =  
  ( ( ( (ContentType) 1 ) <<  BitsRawBufOFFM1    ) - 1  ) << RelShiftRawBufOFFM1    ;
  static const ContentType RelMaskRawBufPU      =  
  ( ( ( (ContentType) 1 ) <<  BitsRawBufPU       ) - 1  ) << RelShiftRawBufPU       ;
  static const ContentType RelMaskRawBufBOARD   =  
  ( ( ( (ContentType) 1 ) <<  BitsRawBufBOARD    ) - 1  ) << RelShiftRawBufBOARD    ;
  static const ContentType RelMaskRawBufQUARTER =
  ( ( ( (ContentType) 1 ) <<  BitsRawBufQUARTER  ) - 1  ) << RelShiftRawBufQUARTER  ;
  static const ContentType RelMaskRawBufCAND    = 
  ( ( ( (ContentType) 1 ) <<  BitsRawBufCAND     ) - 1  ) << RelShiftRawBufCAND     ;

  class RawBufferUnit : public L0MUnit {

  public:

    /// Default Constructor
    RawBufferUnit();

    /// Constructor
    RawBufferUnit(LHCb::MuonTileID id);

    /// Constructor
    RawBufferUnit(DOMNode* pNode);

    /// Destructor
    ~RawBufferUnit();

    void initialize();
    void execute();
    void postexecute();
    
  private:
    boost::dynamic_bitset<>  encodeRLE(int nchar, bool ch);
    boost::dynamic_bitset<>  applyRLE(boost::dynamic_bitset<> bitset_to_compress);

    boost::dynamic_bitset<>  apply0Counting(boost::dynamic_bitset<> bitset_to_compress);

  protected:
    unsigned long m_evtCounter; // Evt counter

    int compress(Register * reg, int start=-1, int stop=-1);
    bool m_compress;

    unsigned long formattedCandidate(CandRegisterHandler candRegHandler, int icand, int quarter=-1);

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_RAWBUFFERUNIT_H
