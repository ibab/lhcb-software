// $Id: MuonBase.h,v 1.1.1.1 2002-01-28 08:48:23 atsareg Exp $
//
#ifndef      L0MUON_MUONBASE_H
#define      L0MUON_MUONBASE_H 1


/** @class MuonBase MuonBase.h L0Muon/MuonBase.h

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
  static const unsigned int BitsReadout   = 1 ;
  static const unsigned int BitsLayer     = 2 ;
  static const unsigned int BitsStation   = 3 ; 
  static const unsigned int BitsLayoutX   = 6 ;
  static const unsigned int BitsLayoutY   = 4 ;
 
  //
  static const unsigned int BitsIndex = BitsX + BitsY + BitsRegion + BitsQuarter;
  static const unsigned int BitsTile = BitsIndex + 
                                       BitsReadout + BitsLayer + BitsStation;
  //
  static const unsigned int ShiftX        = 0 ;
  static const unsigned int ShiftY        = ShiftX       + BitsX  ;
  static const unsigned int ShiftQuarter  = ShiftY       + BitsY  ;
  static const unsigned int ShiftRegion   = ShiftQuarter + BitsQuarter ;  
  static const unsigned int ShiftReadout  = ShiftRegion  + BitsRegion  ;
  static const unsigned int ShiftLayer    = ShiftReadout + BitsReadout ;
  static const unsigned int ShiftStation  = ShiftLayer	 + BitsLayer ;
  static const unsigned int ShiftLayoutX  = ShiftStation + BitsStation ;
  static const unsigned int ShiftLayoutY  = ShiftLayoutX + BitsLayoutX ;  
  //
  static const unsigned int ShiftIndex = 0;
  static const unsigned int ShiftTile = 0;
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
  static const ContentType  MaskTile       =
  ( ( ( (ContentType) 1 ) << BitsTile  ) - 1  ) << ShiftTile   ;
  
};

#endif  // L0MUON_MUONBASE_Hs
