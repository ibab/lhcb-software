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
  static const ShortType BitsRich       = 1;

  /// The number of bits dedicated to the RICH panel number (0-1)
  static const ShortType BitsPanel      = 1;

  /// The number of bits dedicated to the RICH photon detector row number (0-15)
  static const ShortType BitsPDRow      = 4;

  /// The number of bits dedicated to the RICH photon detector column number (0-15)
  static const ShortType BitsPDCol      = 4;

  /// The number of bits dedicated to the RICH pixel row number (0-63)
  // NB : 1 bit bigger then needed due to SICB funnies
  static const ShortType BitsPixelRow   = 6;

  /// The number of bits dedicated to the RICH pixel column number (0-63)
  // NB : 1 bit bigger then needed due to SICB funnies
  static const ShortType BitsPixelCol   = 6;

  /// The number of bits dedicated to the RICH sub-pixel number (0-7)
  static const ShortType BitsSubPixel   = 3;

  /// Status bit for RICH detector field
  static const ShortType BitsRichOK     = 1;

  /// Status bit for RICH panel field
  static const ShortType BitsPanelOK    = 1;

  /// Status bit for RICH photon detector row field
  static const ShortType BitsPDRowOK    = 1;

  /// Status bit for RICH photon detector column field
  static const ShortType BitsPDColOK    = 1;

  /// Status bit for RICH pixel row field
  static const ShortType BitsPixelRowOK = 1;

  /// Status bit for RICH pixel column field
  static const ShortType BitsPixelColOK = 1;

  /// Status bit for RICH sub-pixel field
  static const ShortType BitsSubPixelOK = 1;

  /// The total number of used bits
  static const ShortType BitsIndex  = ( BitsSubPixel + BitsPDCol + BitsPDRow +
                                        BitsPixelCol + BitsPixelRow +
                                        BitsPanel + BitsRich +
                                        BitsRichOK + BitsPanelOK + BitsPDRowOK +
                                        BitsPDColOK + BitsPixelRowOK +
                                        BitsPixelColOK + BitsSubPixelOK );

  // Create the shift registers for data fields
  static const ShortType ShiftSubPixel   = 0;
  static const ShortType ShiftPDCol      = ShiftSubPixel   + BitsSubPixel;
  static const ShortType ShiftPDRow      = ShiftPDCol      + BitsPDCol;
  static const ShortType ShiftPixelCol   = ShiftPDRow      + BitsPDRow;
  static const ShortType ShiftPixelRow   = ShiftPixelCol   + BitsPixelCol;
  static const ShortType ShiftPanel      = ShiftPixelRow   + BitsPixelRow;
  static const ShortType ShiftRich       = ShiftPanel      + BitsPanel;
  static const ShortType ShiftRichOK     = ShiftRich       + BitsRich;
  static const ShortType ShiftPanelOK    = ShiftRichOK     + BitsRichOK;
  static const ShortType ShiftPDRowOK    = ShiftPanelOK    + BitsPanelOK;
  static const ShortType ShiftPDColOK    = ShiftPDRowOK    + BitsPDRowOK;
  static const ShortType ShiftPixelRowOK = ShiftPDColOK    + BitsPDColOK;
  static const ShortType ShiftPixelColOK = ShiftPixelRowOK + BitsPixelRowOK;
  static const ShortType ShiftSubPixelOK = ShiftPixelColOK + BitsPixelColOK;

  // Create the masks for the data fields
  static const LongType MaskSubPixel  = ( ( (LongType)1 << BitsSubPixel ) - 1 ) << ShiftSubPixel;
  static const LongType MaskPixelRow  = ( ( (LongType)1 << BitsPixelRow ) - 1 ) << ShiftPixelRow;
  static const LongType MaskPixelCol  = ( ( (LongType)1 << BitsPixelCol ) - 1 ) << ShiftPixelCol;
  static const LongType MaskPDCol     = ( ( (LongType)1 << BitsPDCol ) - 1 ) << ShiftPDCol;
  static const LongType MaskPDRow     = ( ( (LongType)1 << BitsPDRow ) - 1 ) << ShiftPDRow;
  static const LongType MaskPanel     = ( ( (LongType)1 << BitsPanel ) - 1 ) << ShiftPanel;
  static const LongType MaskRich      = ( ( (LongType)1 << BitsRich ) - 1 ) << ShiftRich;

  // Create the masks for the validity fields
  static const LongType MaskRichOK  = ( ( (LongType)1 << BitsRichOK) - 1 ) << ShiftRichOK;
  static const LongType MaskPanelOK = ( ( (LongType)1 << BitsPanelOK) - 1 ) << ShiftPanelOK;
  static const LongType MaskPDRowOK = ( ( (LongType)1 << BitsPDRowOK) - 1 ) << ShiftPDRowOK;
  static const LongType MaskPDColOK = ( ( (LongType)1 << BitsPDColOK) - 1 ) << ShiftPDColOK;
  static const LongType MaskPixelRowOK = ( ( (LongType)1 << BitsPixelRowOK) - 1 ) << ShiftPixelRowOK;
  static const LongType MaskPixelColOK = ( ( (LongType)1 << BitsPixelColOK) - 1 ) << ShiftPixelColOK;
  static const LongType MaskSubPixelOK = ( ( (LongType)1 << BitsSubPixelOK) - 1 ) << ShiftSubPixelOK;
  static const LongType SetRichOK  = ( (RichSmartCode::LongType)1 << RichSmartCode::ShiftRichOK) & MaskRichOK;
  static const LongType SetPanelOK = ( (RichSmartCode::LongType)1 << RichSmartCode::ShiftPanelOK) & MaskPanelOK;
  static const LongType SetPDRowOK = ( (RichSmartCode::LongType)1 << RichSmartCode::ShiftPDRowOK) & MaskPDRowOK;
  static const LongType SetPDColOK = ( (RichSmartCode::LongType)1 << RichSmartCode::ShiftPDColOK) & MaskPDColOK;
  static const LongType SetPixelRowOK = ( (RichSmartCode::LongType)1 << RichSmartCode::ShiftPixelRowOK) & MaskPixelRowOK;
  static const LongType SetPixelColOK = ( (RichSmartCode::LongType)1 << RichSmartCode::ShiftPixelColOK) & MaskPixelColOK;
  static const LongType SetSubPixelOK = ( (RichSmartCode::LongType)1 << RichSmartCode::ShiftSubPixelOK) & MaskSubPixelOK;

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
