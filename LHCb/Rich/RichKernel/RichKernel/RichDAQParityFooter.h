
//=============================================================================================
/** @file RichDAQParityFooter.h
 *
 *  Header file for RICH DAQ utility class : Rich::DAQ::ParityFooter
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/09/2006
 */
//=============================================================================================

#ifndef RICHDAQ_RICHDAQPARITYFOOTER_H
#define RICHDAQ_RICHDAQPARITYFOOTER_H 1

#include "RichKernel/RichDAQFooterPDBase.h"

namespace Rich
{
  namespace DAQ
  {

    //=============================================================================================
    /** @class ParityFooter RichDAQParityFooter.h
     *
     *  Implements a single word parity footer.
     *  By default the word has zero size to represent the fact that under normal conditions
     *  the foot is missing. It is create on-demand when set.
     *
     *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
     *  @date   05/09/2006
     */
    //=============================================================================================

    class ParityFooter : public FooterPDBase,
                         public LHCb::MemPoolAlloc<ParityFooter>
    {

    public:

      /// Default Constructor
      ParityFooter( ) { }

      /// Constructor with given size
      ParityFooter( const ShortType nWords,
                    const LongType  wordInit = 0 ) 
        : FooterPDBase(nWords,wordInit) { }

      /// Constructor from raw footer word(s)
      explicit ParityFooter( const FooterPDBase::FooterWords & words )
        : FooterPDBase(words) { }
      
      /// Destructor
      ~ParityFooter( ) { }

      /// Does this foot have a parity word ?
      inline bool hasParityWord() const 
      { 
        return ( nFooterWords() != 0 );
      }

      /// Access the parity word
      inline Rich::DAQ::LongType parityWord() const
      {
        return ( hasParityWord() ? footerWords()[0] : 0 );
      }

      /// Set the parity word
      inline void setParityWord( const Rich::DAQ::LongType word )
      {
        makeParityAvailable();
        this->setWord( 0, word );
      }

      /// Test the parity word
      inline bool testParityWord( const LongType /* refWord */ ) const 
      { 
        return ( 0 == this->parityWord() );
      }

    private:

      /// Make sure this footer has the parity word available
      inline void makeParityAvailable()
      {
        if ( !hasParityWord() )
        {
          footerWords() = FooterPDBase::FooterWords(1,0);
        } 
      }

    };

  }
}

#endif // RICHDAQ_RICHDAQPARITYFOOTER_H
