// $Id: L0MuonBase.h,v 1.2 2002-05-07 08:17:28 atsareg Exp $
//
#ifndef      L0MUON_L0MUONBASE_H
#define      L0MUON_L0MUONBASE_H 1


/** @class L0MuonBase L0MuonBase.h L0Muon/L0MuonBase.h

    Namespace for code/decode rules of L0MuonCandidate class

    @author  Andrei Tsaregorodtsev
    @date    9/01/2002
*/


namespace L0MuonBase {

  typedef unsigned int   ContentType ; 
  //
  static const unsigned int BitsBCID         = 8 ;
  static const unsigned int BitsStatus       = 4 ;
  static const unsigned int BitsPtSign       = 1 ;
  static const unsigned int BitsPt           = 7 ;
  static const unsigned int BitsRegion       = 2 ; 
  static const unsigned int BitsX            = 6 ;
  static const unsigned int BitsY            = 4 ;
  //
  static const unsigned int ShiftX        = 0 ;
  static const unsigned int ShiftY        = ShiftX       + BitsX  ;
  static const unsigned int ShiftRegion   = ShiftY       + BitsY  ;
  static const unsigned int ShiftPt       = ShiftRegion  + BitsRegion ;  
  static const unsigned int ShiftPtSign   = ShiftPtSign  + BitsPtSign ;  
  static const unsigned int ShiftStatus   = ShiftPt      + BitsPt ;
  static const unsigned int ShiftBCID     = ShiftStatus  + BitsStatus  ;
  //
  static const ContentType  MaskX     =
  ( ( ( (ContentType) 1 ) << BitsX  ) - 1  ) << ShiftX  ;
  static const ContentType  MaskY     =
  ( ( ( (ContentType) 1 ) << BitsY  ) - 1  ) << ShiftY  ;
  static const ContentType  MaskRegion     =
  ( ( ( (ContentType) 1 ) << BitsRegion  ) - 1  ) << ShiftRegion  ;
  static const ContentType  MaskPt     =
  ( ( ( (ContentType) 1 ) << BitsPt  ) - 1  ) << ShiftPt   ;
  static const ContentType  MaskPtSign     =
  ( ( ( (ContentType) 1 ) << BitsPtSign  ) - 1  ) << ShiftPtSign   ;
  static const ContentType  MaskStatus     =
  ( ( ( (ContentType) 1 ) << BitsStatus  ) - 1  ) << ShiftStatus  ;
  static const ContentType  MaskBCID     =
  ( ( ( (ContentType) 1 ) << BitsBCID  ) - 1  ) << ShiftBCID  ;
  
  // Pt decoding constants
  static const double PT_BIN_WIDTH = 40.;
  
};
#endif   // L0MUON_L0MUONBASE_H
