// $Id: MuonHLTBase.h,v 1.3 2008-04-02 11:52:05 asatta Exp $
#ifndef MUONHLTBASE_H 
#define MUONHLTBASE_H 1

// Include files

/** @class MuonHLTBase MuonHLTBase.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2004-01-23
 */
namespace MuonHLTBaseDC06
{
  static const unsigned int BitsAddress   = 8;
  static const unsigned int BitsTime      = 4;

  static const unsigned int BitsFirstHalf     = 16;
  static const unsigned int BitsSecondHalf     = 16;
  
  static const unsigned int ShiftAddress  = 0;
  static const unsigned int ShiftTime     = ShiftAddress +BitsAddress;

  static const unsigned int ShiftFirstHalf    = 0;
  static const unsigned int ShiftSecondHalf    = BitsFirstHalf;
  
  static const unsigned int  MaskAddress     =
  ( ( ( (unsigned int) 1 ) << BitsAddress  ) - 1  ) << ShiftAddress  ;
  static const unsigned int  MaskTime     =
  ( ( ( (unsigned int) 1 ) << BitsTime  ) - 1  ) << ShiftTime  ;
  static const unsigned int  MaskFirstHalf    =
  ( ( ( (unsigned int) 1 ) << BitsFirstHalf  ) - 1  ) << ShiftFirstHalf  ;
  static const unsigned int  MaskSecondHalf    =
  ( ( ( (unsigned int) 1 ) << BitsSecondHalf  ) - 1  ) << ShiftSecondHalf  
;


}
namespace MuonHLTBaseV1
{
  static const unsigned int BitsAddress   = 12;
  static const unsigned int BitsTime      = 4;

  static const unsigned int BitsFirstHalf     = 16;
  static const unsigned int BitsSecondHalf     = 16;
  
  static const unsigned int ShiftAddress  = 0;
  static const unsigned int ShiftTime     = ShiftAddress +BitsAddress;

  static const unsigned int ShiftFirstHalf    = 0;
  static const unsigned int ShiftSecondHalf    = BitsFirstHalf;
  
  static const unsigned int  MaskAddress     =
  ( ( ( (unsigned int) 1 ) << BitsAddress  ) - 1  ) << ShiftAddress  ;
  static const unsigned int  MaskTime     =
  ( ( ( (unsigned int) 1 ) << BitsTime  ) - 1  ) << ShiftTime  ;
  static const unsigned int  MaskFirstHalf    =
  ( ( ( (unsigned int) 1 ) << BitsFirstHalf  ) - 1  ) << ShiftFirstHalf  ;
  static const unsigned int  MaskSecondHalf    =
  ( ( ( (unsigned int) 1 ) << BitsSecondHalf  ) - 1  ) << ShiftSecondHalf  
;


}


#endif // MUONHLTBASE_H
