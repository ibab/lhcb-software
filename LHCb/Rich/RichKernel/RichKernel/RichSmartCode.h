#ifndef RICHKERNEL_RICHSMARTCODE_H
#define RICHKERNEL_RICHSMARTCODE_H 1

/** @class RichSmartCode RichSmartCode.h RichGen/RichSmartCode.h
 *
 *  Namespace for all code/decode rules of RichSmartID class
 *
 *  @author Andy Presland   (andrew.presland@cern.ch)
 *  @author Chris Jones     (christopher.rob.jones@cern.ch)
 *
 *  created Tue Feb 26 09:25:55 2002
 */

namespace RichSmartCode {

  /// 32 bits in the transient  representation
  typedef unsigned long int ContentType;

  /// 16 bits in the persistent representation
  typedef unsigned short WriteType;

  static const unsigned int BitsRich      = 1;
  static const unsigned int BitsPanel     = 2;
  static const unsigned int BitsHPDRow    = 6;
  static const unsigned int BitsHPDCol    = 6;
  static const unsigned int BitsPixelRow  = 8;
  static const unsigned int BitsPixelCol  = 8;
  static const unsigned int BitsIndex     = 
   BitsHPDCol + BitsHPDRow + BitsPixelRow + BitsPixelCol + BitsPanel + BitsRich;

  static const unsigned int BitsAll       =
   BitsHPDCol + BitsHPDRow + BitsPixelRow + BitsPixelCol + BitsPanel + BitsRich;

  static const unsigned int BitsTotal     = 32 ;
  static const unsigned int BitsRest      = BitsTotal - BitsAll;

  static const unsigned int ShiftHPDCol   = 0 ;
  static const unsigned int ShiftHPDRow   = ShiftHPDCol   + BitsHPDCol;
  static const unsigned int ShiftPixelRow = ShiftHPDRow   + BitsHPDRow;
  static const unsigned int ShiftPixelCol = ShiftPixelRow + BitsPixelRow;
  static const unsigned int ShiftPanel    = ShiftPixelCol + BitsPixelCol;
  static const unsigned int ShiftRich     = ShiftPanel    + BitsPanel;

  static const unsigned int ShiftIndex    = ShiftHPDCol;
  static const unsigned int ShiftAll      = ShiftHPDCol;
  static const unsigned int ShiftRest     = ShiftRich + BitsRich;

  static const ContentType  MaskHPDCol    =
  ( ( ( (ContentType) 1 ) << BitsHPDCol  ) - 1  ) << ShiftHPDCol  ;

  static const ContentType  MaskHPDRow    =
  ( ( ( (ContentType) 1 ) << BitsHPDRow  ) - 1  ) << ShiftHPDRow  ;

  static const ContentType  MaskPixelRow  =
  ( ( ( (ContentType) 1 ) << BitsPixelRow )  - 1  ) << ShiftPixelRow;

  static const ContentType  MaskPixelCol  =
  ( ( ( (ContentType) 1 ) << BitsPixelCol )  - 1  ) << ShiftPixelCol;

  static const ContentType  MaskPanel     =
  ( ( ( (ContentType) 1 ) << BitsPanel ) - 1  ) << ShiftPanel ;

  static const ContentType  MaskRich      =
  ( ( ( (ContentType) 1 ) << BitsRich ) - 1  ) << ShiftRich ;

  static const ContentType  MaskIndex     =
  ( ( ( (ContentType) 1 ) << BitsIndex) - 1  ) << ShiftIndex ;

  static const ContentType  MaskAll       =
  ( ( ( (ContentType) 1 ) << BitsAll  ) - 1  ) << ShiftAll   ;

  static const ContentType  MaskRest      =
  ( ( ( (ContentType) 1 ) << BitsRest ) - 1  ) << ShiftRest  ;

};

#endif // RICHKERNEL_RICHSMARTCODE_H












