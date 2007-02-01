
//=============================================================================================
/** @file RichDAQParityFooter.h
 *
 *  Header file for RICH DAQ utility class : Rich::DAQ::ParityFooter
 *
 *  CVS Log :-
 *  $Id: RichDAQParityFooterTB2006.h,v 1.1 2007-02-01 17:42:29 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/09/2006
 */
//=============================================================================================

#ifndef RICHDAQ_RICHDAQPARITYFOOTERTB2006_H
#define RICHDAQ_RICHDAQPARITYFOOTERTB2006_H 1

#include "RichDAQFooterPDBase.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace DAQ
   *
   *  namespace for RICH DAQ software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace DAQ
  {

    //=============================================================================================
    /** @class ParityFooterTB2006 RichDAQParityFooterTB2006.h
     *
     *  Implements a single word parity footer, as used in the 2006 RICH testbeam
     *
     *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
     *  @date   05/09/2006
     */
    //=============================================================================================

    class ParityFooterTB2006 : public FooterPDBase
    {

    public:

      /// Standard constructor
      ParityFooterTB2006( ) : FooterPDBase(1) {}

      /// Destructor
      ~ParityFooterTB2006( ) { }

      /// Does this foot have a parity word ?
      inline bool hasParityWord() const { return true; }

      /// Access the parity word
      inline Rich::DAQ::LongType parityWord() const
      {
        return footerWords()[0];
      }

      /// Set the parity word
      inline void setParityWord( const Rich::DAQ::LongType word )
      {
        this->setWord( 0, word );
      }

      /// Test the parity word
      inline bool testParityWord( const LongType refWord ) const 
      { 
        return ( refWord == this->parityWord() );
      }

    };

  }
}

#endif // RICHDAQ_RICHDAQPARITYFOOTERTB2006_H
