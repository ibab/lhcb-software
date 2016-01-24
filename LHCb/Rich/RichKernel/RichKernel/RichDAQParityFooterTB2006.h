
//=============================================================================================
/** @file RichDAQParityFooterTB2006.h
 *
 *  Header file for RICH DAQ utility class : Rich::DAQ::ParityFooterTB2006
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/09/2006
 */
//=============================================================================================

#ifndef RICHDAQ_RICHDAQPARITYFOOTERTB2006_H
#define RICHDAQ_RICHDAQPARITYFOOTERTB2006_H 1

#include "RichKernel/RichDAQFooterPDBase.h"

namespace Rich
{
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

      /// Does this foot have a parity word ?
      inline bool hasParityWord() const noexcept { return true; }

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
