// $Id: L0MuonBase.h,v 1.3 2005-12-20 13:47:37 ocallot Exp $
//
#ifndef      L0MUON_L0MUONBASE_H
#define      L0MUON_L0MUONBASE_H 1


/** @class L0MuonBase L0MuonBase.h L0Muon/L0MuonBase.h

    Namespace for code/decode rules of L0ProcessorData class for muon

    @author  Andrei Tsaregorodtsev
    @date    9/01/2002
*/


namespace L0MuonBase {

  typedef unsigned int   ContentType ; 

  // Empty L0ProcessorData
  static const unsigned int emptyMuonData    = 1<<16 ;

  // Bits reserved for the various fields
  static const unsigned int BitsPt           = 7 ;
  static const unsigned int BitsAddM3        = 4 ;
  static const unsigned int BitsPU           = 2 ;
  static const unsigned int BitsBoard        = 4 ; 
  static const unsigned int BitsCharge       = 1 ;


  // CU Word (16 bits per candidate)
  static const unsigned int ShiftPt     =  1 ;
  static const unsigned int ShiftAddM3  =  8 ;

  // SU Word (16 bits per candidate)
  static const unsigned int ShiftPU     =  1 ; 
  static const unsigned int ShiftBoard  =  3 ; 
  static const unsigned int ShiftCharge =  7 ;

  //
  static const ContentType  MaskPt     =  
  ( ( ( (ContentType) 1 ) << BitsPt     ) - 1  ) << ShiftPt     ;
  static const ContentType  MaskAddM3  =  
  ( ( ( (ContentType) 1 ) << BitsAddM3  ) - 1  ) << ShiftAddM3  ;
  static const ContentType  MaskPU     =  
  ( ( ( (ContentType) 1 ) << BitsPU     ) - 1  ) << ShiftPU     ;
  static const ContentType  MaskBoard  =  
  ( ( ( (ContentType) 1 ) << BitsBoard  ) - 1  ) << ShiftBoard  ;
  static const ContentType  MaskCharge =  
  ( ( ( (ContentType) 1 ) << BitsCharge ) - 1  ) << ShiftCharge ;
  
};
#endif   // L0MUON_L0MUONBASE_H
