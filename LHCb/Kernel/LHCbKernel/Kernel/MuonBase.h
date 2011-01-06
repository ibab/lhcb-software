// $Id: MuonBase.h,v 1.1 2005-12-02 16:28:07 cattanem Exp $
//
#ifndef KERNEL_MUONBASE_H
#define KERNEL_MUONBASE_H 1


/** @class MuonBase MuonBase.h Kernel/MuonBase.h

    Namespace for code/decode rules of MuonID class

    @author  Andrei Tsaregorodtsev
    @date    9/01/2002
*/


namespace MuonBase {

  typedef unsigned int   ContentType ; 
  //
  static const unsigned int BitsX         = 7 ;
  static const unsigned int BitsY         = 4 ;
  static const unsigned int BitsQuarter   = 2 ; 
  static const unsigned int BitsRegion    = 2 ;
  static const unsigned int BitsReadout   = 0 ;
  static const unsigned int BitsLayer     = 0 ;
  static const unsigned int BitsStation   = 3 ; 
  static const unsigned int BitsLayoutX   = 6 ;
  static const unsigned int BitsLayoutY   = 4 ;
 
  //
  static const unsigned int BitsIndex = BitsX + BitsY + BitsRegion + BitsQuarter;
  static const unsigned int BitsKey = BitsIndex + 
                                       BitsReadout + BitsLayer + BitsStation;
  //
  static const unsigned int ShiftY        = 0 ;
  static const unsigned int ShiftX        = ShiftY       + BitsY  ;
  static const unsigned int ShiftQuarter  = ShiftX       + BitsX  ;
  static const unsigned int ShiftRegion   = ShiftQuarter + BitsQuarter ;  
  static const unsigned int ShiftReadout  = ShiftRegion  + BitsRegion  ;
  static const unsigned int ShiftLayer    = ShiftReadout + BitsReadout ;
  static const unsigned int ShiftStation  = ShiftLayer	 + BitsLayer ;
  static const unsigned int ShiftLayoutX  = ShiftStation + BitsStation ;
  static const unsigned int ShiftLayoutY  = ShiftLayoutX + BitsLayoutX ;  
  //
  static const unsigned int ShiftIndex = 0;
  static const unsigned int ShiftKey = 0;
  //
  static const ContentType  MaskX     =
  ( ( ( (ContentType) 1 ) << BitsX  ) - 1  ) << ShiftX  ;
  static const ContentType  MaskY     =
  ( ( ( (ContentType) 1 ) << BitsY  ) - 1  ) << ShiftY  ;
  static const ContentType  MaskRegion     =
  ( ( ( (ContentType) 1 ) << BitsRegion  ) - 1  ) << ShiftRegion  ;
  static const ContentType  MaskQuarter     =
  ( ( ( (ContentType) 1 ) << BitsQuarter  ) - 1  ) << ShiftQuarter   ;
  static const ContentType  MaskLayoutX     =
  ( ( ( (ContentType) 1 ) << BitsLayoutX  ) - 1  ) << ShiftLayoutX  ;
  static const ContentType  MaskLayoutY     =
  ( ( ( (ContentType) 1 ) << BitsLayoutY  ) - 1  ) << ShiftLayoutY  ;
  static const ContentType  MaskReadout     =
  ( ( ( (ContentType) 1 ) << BitsReadout  ) - 1  ) << ShiftReadout  ;
  static const ContentType  MaskLayer     =
  ( ( ( (ContentType) 1 ) << BitsLayer  ) - 1  ) << ShiftLayer   ;
  static const ContentType  MaskStation     =
  ( ( ( (ContentType) 1 ) << BitsStation  ) - 1  ) << ShiftStation   ;
  //
  static const ContentType  MaskIndex       =
  ( ( ( (ContentType) 1 ) << BitsIndex  ) - 1  ) << ShiftIndex   ;
  static const ContentType  MaskKey       =
  ( ( ( (ContentType) 1 ) << BitsKey  ) - 1  ) << ShiftKey   ;
  
  static const int CENTER      = 0;
  static const int UP          = 1;
  static const int DOWN        = -1;
  static const int RIGHT       = 1;
  static const int LEFT        = -1;    
  
}

#endif  // KERNEL_MUONBASE
