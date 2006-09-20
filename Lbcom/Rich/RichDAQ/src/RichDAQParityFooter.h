
//=============================================================================================
/** @file RichDAQFooterPDBase.h
 *
 *  Header file for RICH DAQ utility class : RichDAQFooterPDBase
 *
 *  CVS Log :-
 *  $Id: RichDAQParityFooter.h,v 1.2 2006-09-20 13:07:12 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/09/2006
 */
//=============================================================================================

#ifndef RICHDAQ_RICHDAQPARITYFOOTER_H 
#define RICHDAQ_RICHDAQPARITYFOOTER_H 1

#include "RichDAQFooterPDBase.h"

//=============================================================================================
/** @class RichDAQParityFooter RichDAQParityFooter.h
 *
 *  Implements a single word parity footer
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   05/09/2006
 */
//=============================================================================================

class RichDAQParityFooter : public RichDAQFooterPDBase
{
  
public: 

  /// Standard constructor
  RichDAQParityFooter( ) : RichDAQFooterPDBase(1) {} 

  /// Destructor
  ~RichDAQParityFooter( ) { }

  /// Does this foot have a parity word ?
  inline bool hasParityWord() const { return true; }

  /// Access the parity word
  inline RichDAQ::LongType parityWord() const 
  {
    return footerWords()[0];
  }

  /// Set the parity word
  inline void setParityWord( const RichDAQ::LongType word )
  {
    this->setWord( 0, word );
  }

};

#endif // RICHDAQ_RICHDAQPARITYFOOTER_H
