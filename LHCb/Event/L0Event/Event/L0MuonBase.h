// $Id: L0MuonBase.h,v 1.2 2005-02-24 16:54:08 dlunesu Exp $
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

  //For L1Buffer
  static const unsigned int BitsBufStatus    = 2 ;
  static const unsigned int BitsBufPtSign    = 1 ;
  static const unsigned int BitsBufPt        = 7 ;
  static const unsigned int BitsBufQuarter   = 2 ;
  static const unsigned int BitsBufRegion    = 2 ;
  static const unsigned int BitsBufPBx       = 1 ;
  static const unsigned int BitsBufPBy       = 1 ;
  
  static const unsigned int BitsBufPUx       = 1 ;
  static const unsigned int BitsBufPUy       = 1 ;
  static const unsigned int BitsBufM3x       = 5 ;
  static const unsigned int BitsBufM3y       = 2 ;
  static const unsigned int BitsBufOffM2     = 4 ;
  static const unsigned int BitsBufOffM1     = 3 ;

  //
  static const unsigned int ShiftX        = 0 ;
  static const unsigned int ShiftY        = ShiftX       + BitsX  ;
  static const unsigned int ShiftRegion   = ShiftY       + BitsY  ;
  static const unsigned int ShiftPt       = ShiftRegion  + BitsRegion ;  
  static const unsigned int ShiftPtSign   = ShiftPt      + BitsPt ;  
  static const unsigned int ShiftStatus   = ShiftPtSign  + BitsPtSign ;
  static const unsigned int ShiftBCID     = ShiftStatus  + BitsStatus  ;

  //For L1Buffer
  static const unsigned int ShiftBufStatus  = 0 ;
  static const unsigned int ShiftBufPtSign  = ShiftBufStatus  + BitsBufStatus ;
  static const unsigned int ShiftBufPt      = ShiftBufPtSign  + BitsBufPtSign ;
  static const unsigned int ShiftBufQuarter = ShiftBufPt      + BitsBufPt ;
  static const unsigned int ShiftBufPB      = ShiftBufQuarter + BitsBufQuarter ;
  static const unsigned int ShiftBufRegion  = ShiftBufQuarter + BitsBufQuarter ;
  static const unsigned int ShiftBufPBx     = ShiftBufRegion  + BitsBufRegion ;
  static const unsigned int ShiftBufPBy     = ShiftBufPBx     + BitsBufPBx ;

  static const unsigned int ShiftBufPUx       = 0 ;
  static const unsigned int ShiftBufPUy       = ShiftBufPUx       + BitsBufPUx ;
  static const unsigned int ShiftBufM3x       = ShiftBufPUy       + BitsBufPUy ;
  static const unsigned int ShiftBufM3y       = ShiftBufM3x       + BitsBufM3x ;
  
  static const unsigned int ShiftBufOffM2     = ShiftBufM3y + BitsBufM3y ;
  static const unsigned int ShiftBufOffM1     = ShiftBufOffM2 + BitsBufOffM2 ;


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
  

  //For L1Buffer                                                                                        
  static const ContentType  MaskBufStatus     =
  ( ( ( (ContentType) 1 ) << BitsBufStatus  ) - 1  ) << ShiftBufStatus ;
  static const ContentType  MaskBufQuarter     =
  ( ( ( (ContentType) 1 ) << BitsBufQuarter  ) - 1  ) << ShiftBufQuarter ;
  static const ContentType  MaskBufRegion     =
  ( ( ( (ContentType) 1 ) << BitsBufRegion  ) - 1  ) << ShiftBufRegion ;
  static const ContentType  MaskBufPBx     =
  ( ( ( (ContentType) 1 ) << BitsBufPBx  ) - 1  ) << ShiftBufPBx ;
  static const ContentType  MaskBufPBy     =
  ( ( ( (ContentType) 1 ) << BitsBufPBy  ) - 1  ) << ShiftBufPBy ;

  static const ContentType  MaskBufPUx     =
  ( ( ( (ContentType) 1 ) << BitsBufPUx  ) - 1  ) << ShiftBufPUx ;
  static const ContentType  MaskBufPUy     =
  ( ( ( (ContentType) 1 ) << BitsBufPUy  ) - 1  ) << ShiftBufPUy ;
  static const ContentType  MaskBufM3x     =
  ( ( ( (ContentType) 1 ) << BitsBufM3x  ) - 1  ) << ShiftBufM3x ;
  static const ContentType  MaskBufM3y     =
  ( ( ( (ContentType) 1 ) << BitsBufM3y  ) - 1  ) << ShiftBufM3y ;

  static const ContentType  MaskBufOffM2     =
  ( ( ( (ContentType) 1 ) << BitsBufOffM2  ) - 1  ) << ShiftBufOffM2 ;
  static const ContentType  MaskBufOffM1     =
  ( ( ( (ContentType) 1 ) << BitsBufOffM1  ) - 1  ) << ShiftBufOffM1 ;  

  static const ContentType  MaskBufPtSign     =
  ( ( ( (ContentType) 1 ) << BitsBufPtSign  ) - 1  ) << ShiftBufPtSign ;
  static const ContentType  MaskBufPt     =
  ( ( ( (ContentType) 1 ) << BitsBufPt  ) - 1  ) << ShiftBufPt ;
  

//define M1 offset extrapolation relatively at M3
  static const int EXTR_OFFSET_M1_M3[6] = {0,4,7,11,14,18};


// Pt decoding constants
  static const double PT_BIN_WIDTH = 40.;
  
};
#endif   // L0MUON_L0MUONBASE_H
