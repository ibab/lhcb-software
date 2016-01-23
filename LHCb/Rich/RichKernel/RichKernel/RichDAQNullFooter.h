
//=============================================================================================
/** @file RichDAQNullFooter.h
 *
 *  Header file for RICH DAQ utility class : Rich::DAQ::NullFooter
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/09/2006
 */
//=============================================================================================

#ifndef RICHDAQ_RICHDAQNULLFOOTER_H
#define RICHDAQ_RICHDAQNULLFOOTER_H 1

// Kernel
#include "RichKernel/RichDAQFooterPDBase.h"

namespace Rich
{
  namespace DAQ
  {

    //=============================================================================================
    /** @class NullFooter RichDAQNullFooter.h
     *
     *  Implements a null footer for the LHCB HPD data blocks
     *
     *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
     *  @date   05/09/2006
     */
    //=============================================================================================

    class NullFooter : public FooterPDBase,
                       public LHCb::MemPoolAlloc<NullFooter>
    {

    public:

      /// Does this foot have a parity word ?
      inline bool hasParityWord() const { return false; }

      /// Access the parity word
      inline Rich::DAQ::LongType parityWord() const { return 0; }

      /// Set the parity word
      inline void setParityWord( const Rich::DAQ::LongType /* word */ ) {;}

      /// Test the parity word
      inline bool testParityWord( const ShortType ) const { return true; }

    };

  }
}

#endif // RICHDAQ_RICHDAQNULLFOOTER_H
