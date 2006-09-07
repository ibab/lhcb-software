
//=============================================================================================
/** @file RichDAQFooterPDBase.h
 *
 *  Header file for RICH DAQ utility class : RichDAQFooterPDBase
 *
 *  CVS Log :-
 *  $Id: RichDAQParityFooter.h,v 1.1 2006-09-07 17:14:10 jonrob Exp $
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

};

#endif // RICHDAQ_RICHDAQPARITYFOOTER_H
