#ifndef RICHKERNEL_RICHSMARTCODE_H
#define RICHKERNEL_RICHSMARTCODE_H 1

/** @class RichSmartCode RichSmartCode.h RichKernel/RichSmartCode.h
 *
 *  Namespace for encoding rules of RichSmartID class
 *
 *  @author Chris Jones     (christopher.rob.jones@cern.ch)
 *
 *  @date 2002-2-26
 */

namespace RichSmartCode {

  typedef unsigned long int LongType;
  typedef unsigned int      ShortType;

  /// The number of bits dedicated to the RICH detector number (0-1)
  static const ShortType BitsRich      = 1;

  /// The number of bits dedicated to the RICH panel number (0-1)
  static const ShortType BitsPanel     = 1;

  /// The number of bits dedicated to the RICH photon detector row number (0-63)
  static const ShortType BitsPDRow     = 6;

  /// The number of bits dedicated to the RICH photon detector column number (0-63) 
  static const ShortType BitsPDCol     = 6;

  /// The number of bits dedicated to the RICH pixel row number (0-63) 
  static const ShortType BitsPixelRow  = 6;

  /// The number of bits dedicated to the RICH pixel column number (0-63) 
  static const ShortType BitsPixelCol  = 6;

  /// The number of bits dedicated to the RICH sub-pixel number (0-7)
  static const ShortType BitsSubPixel  = 3;

  /// The number of bits dedicated to the overall status od the identifer (0-1)
  static const ShortType BitsAllOK     = 1;

  /// The total number of used bits
  static const ShortType BitsIndex     = ( BitsSubPixel +
                                           BitsPDCol    +
                                           BitsPDRow    +
                                           BitsPixelCol +
                                           BitsPixelRow +
                                           BitsPanel    +
                                           BitsRich     +
                                           BitsAllOK    );

  // Create the shift registers for data fields
  static const ShortType ShiftIndex    = 0;
  static const ShortType ShiftSubPixel = ShiftIndex;
  static const ShortType ShiftPDCol    = ShiftSubPixel + BitsSubPixel;
  static const ShortType ShiftPDRow    = ShiftPDCol    + BitsPDCol;
  static const ShortType ShiftPixelCol = ShiftPDRow    + BitsPDRow;
  static const ShortType ShiftPixelRow = ShiftPixelCol + BitsPixelCol;
  static const ShortType ShiftPanel    = ShiftPixelRow + BitsPixelRow;
  static const ShortType ShiftRich     = ShiftPanel    + BitsPanel;
  static const ShortType ShiftAllOK    = ShiftRich     + BitsRich;

  // Create the Masks
  static const LongType MaskSubPixel  =
  ( ( ( (LongType) 1 ) << BitsSubPixel ) - 1 ) << ShiftSubPixel;
  static const LongType MaskPixelRow  =
  ( ( ( (LongType) 1 ) << BitsPixelRow ) - 1 ) << ShiftPixelRow;
  static const LongType MaskPixelCol  =
  ( ( ( (LongType) 1 ) << BitsPixelCol ) - 1 ) << ShiftPixelCol;
  static const LongType MaskPDCol     =
  ( ( ( (LongType) 1 ) << BitsPDCol )    - 1 ) << ShiftPDCol;
  static const LongType MaskPDRow     =
  ( ( ( (LongType) 1 ) << BitsPDRow )    - 1 ) << ShiftPDRow;
  static const LongType MaskPanel     =
  ( ( ( (LongType) 1 ) << BitsPanel )    - 1 ) << ShiftPanel;
  static const LongType MaskRich      =
  ( ( ( (LongType) 1 ) << BitsRich )     - 1 ) << ShiftRich;
  static const LongType MaskIndex     =
  ( ( ( (LongType) 1 ) << BitsIndex)     - 1 ) << ShiftIndex;

  // Create the All OK bit setter
  static const LongType MaskAllOK     =
  ( ( ( (LongType) 1 ) << BitsAllOK)     - 1 ) << ShiftAllOK;
  static const LongType SetAllOK     = 
  ( (RichSmartCode::ShortType)1 << RichSmartCode::ShiftAllOK) & MaskAllOK;
  
  // Create the maximum allowed value for each data field
  static const ShortType MaxRich     = ( (ShortType)1 << BitsRich     ) - 1;
  static const ShortType MaxPanel    = ( (ShortType)1 << BitsPanel    ) - 1;
  static const ShortType MaxPDRow    = ( (ShortType)1 << BitsPDRow    ) - 1;
  static const ShortType MaxPDCol    = ( (ShortType)1 << BitsPDCol    ) - 1;
  static const ShortType MaxPixelRow = ( (ShortType)1 << BitsPixelRow ) - 1;
  static const ShortType MaxPixelCol = ( (ShortType)1 << BitsPixelCol ) - 1;
  static const ShortType MaxSubPixel = ( (ShortType)1 << BitsSubPixel ) - 1;

};

#endif // RICHKERNEL_RICHSMARTCODE_H
