// $Id: L0DUTELL1Base.h,v 1.1 2004-01-26 16:23:46 cattanem Exp $
//
#ifndef      TRGTOOLS_L0DUTELL1BASE_H
#define      TRGTOOLS_L0DUTELL1BASE_H 1


/** @class L0DUTELL1Base L0DUTELL1Base.h TrgTools/L0DUTELL1Base.h

    Namespace for code/decode rules of L0DUTELL1 class

    @author  olivier Deschamps 
    @date    13/01/2004
*/


namespace L0DUTELL1Base {
  /*
    Bit assignment for the L0DU Blocklets (16 bits) to be sent to the L1Buffer 
  */
  // Pu Position decoding constant
  static const double zPos_BIN_WIDTH = 1.;
  
  // L0DU Header : # L0PU Hits + # L0Muons (8+4 bits)
  static const unsigned int BitPuHits        = 8 ;
  static const unsigned int BitMuons         = 4 ;
  // PuVeto Peak position and contents ( 8+8 bits)
  static const unsigned int BitPuPos         = 8 ;
  static const unsigned int BitPuSum         = 8 ;
  // Muon position (including quarter) (12 + 2  bits)
  static const unsigned int BitMuR           = 2 ; 
  static const unsigned int BitMuX           = 6 ;
  static const unsigned int BitMuY           = 4 ;
  static const unsigned int BitMuQ           = 2 ;
  // Muon content : Pt + sign + status (12 bits)
  static const unsigned int BitMuStatus      = 4 ;
  static const unsigned int BitMuPtSign      = 1 ;
  static const unsigned int BitMuPt          = 7 ;

  /*
    Built corresponding masks
  */
  static const unsigned int ShiftPuHits  = 0 ;
  static const unsigned int ShiftMuons = ShiftPuHits + BitPuHits ;
  //
  static const unsigned int ShiftPuPos     = 0 ;
  static const unsigned int ShiftPuSum = ShiftPuPos + BitPuPos;
  //
  static const unsigned int ShiftMuX      = 0 ;
  static const unsigned int ShiftMuY      = ShiftMuX      + BitMuX  ;
  static const unsigned int ShiftMuR = ShiftMuY      + BitMuY  ;
  static const unsigned int ShiftMuQ= ShiftMuR + BitMuQ;
  //
  static const unsigned int ShiftMuPt     = 0 ;  
  static const unsigned int ShiftMuPtSign = ShiftMuPt     + BitMuPt ;  
  static const unsigned int ShiftMuStatus = ShiftMuPtSign + BitMuPtSign;
  //
  static const unsigned int  MaskPuHits =
  ( ( ( (unsigned int) 1 ) << BitPuHits ) - 1  ) << ShiftPuHits ;
  static const unsigned int  MaskMuons =
  ( ( ( (unsigned int) 1 ) << BitMuons ) - 1  ) << ShiftMuons ;
  //
  static const unsigned int  MaskPuPos =
  ( ( ( (unsigned int) 1 ) << BitPuPos ) - 1  ) << ShiftPuPos ;
  static const unsigned int  MaskPuSum =
  ( ( ( (unsigned int) 1 ) << BitPuSum ) - 1  ) << ShiftPuSum ;
  //
  static const unsigned int  MaskMuX     =
  ( ( ( (unsigned int) 1 ) << BitMuX  ) - 1  ) << ShiftMuX  ;
  static const unsigned int  MaskMuY     =
  ( ( ( (unsigned int) 1 ) << BitMuY  ) - 1  ) << ShiftMuY  ;
  static const unsigned int  MaskMuR     =
  ( ( ( (unsigned int) 1 ) << BitMuR  ) - 1  ) << ShiftMuR  ;
  static const unsigned int  MaskMuQ    =
  ( ( ( (unsigned int) 1 ) << BitMuQ  ) - 1  ) << ShiftMuQ ;
  //
  static const unsigned int  MaskMuPt     =
  ( ( ( (unsigned int) 1 ) << BitMuPt  ) - 1  ) << ShiftMuPt   ;
  static const unsigned int  MaskMuPtSign     =
  ( ( ( (unsigned int) 1 ) << BitMuPtSign  ) - 1  ) << ShiftMuPtSign   ;
  static const unsigned int  MaskMuStatus     =
  ( ( ( (unsigned int) 1 ) << BitMuStatus  ) - 1  ) << ShiftMuStatus  ;

};
#endif   // TRGTOOLS_L0DUTELL1BASE_H
