
//=============================================================================================
/** @file RichDAQFooterPDBase.h
 *
 *  Header file for RICH DAQ utility class : Rich::DAQ::FooterPDBase
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/09/2006
 */
//=============================================================================================

#ifndef RICHDAQ_RichDAQFooterPDBase_H
#define RICHDAQ_RichDAQFooterPDBase_H 1

// Gaudi
#include "GaudiKernel/GaudiException.h"

// numberings
#include "RichKernel/RichDAQDefinitions.h"

// Kernel
#include "Kernel/MemPoolAlloc.h"
#include "Kernel/FastAllocVector.h"

namespace Rich
{
  namespace DAQ
  {

    //=============================================================================================
    /** @class FooterPDBase RichDAQFooterPDBase.h
     *
     *  Base class for RICH HPD footers. Implements basic common functions
     *
     *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
     *  @date   05/09/2006
     */
    //=============================================================================================
    class FooterPDBase
    {

    public:

      /// Type for header words
      typedef LHCb::PoolAllocVector<LongType> FooterWords;

    public:

      /// Default Constructor (0 words)
      FooterPDBase() { }

      /// Constructor with number of header words
      explicit FooterPDBase ( const ShortType nWords,
                              const LongType  wordInit = 0 )
        : m_footerWords(nWords,wordInit) { }

      /// Copy constructor
      FooterPDBase ( const FooterPDBase & footer )
        : m_footerWords(footer.footerWords()) { }

      /// Constructor from raw footer word(s)
      explicit FooterPDBase( const FooterWords & words )
        : m_footerWords(words) { }

      /// Destructor
      ~FooterPDBase ( ) { }

    public: // methods

      /// Read only access to footer words
      inline const FooterWords & footerWords() const { return m_footerWords; }

      /// Returns the number of data words in the footer
      inline unsigned int nFooterWords() const
      {
        return footerWords().size();
      }

    public: // reading and writing

      /// Read given number of data words from stream
      /// Note, after this call data pointer is incremented to the next word after the footer
      inline void readFromDataStream( const LongType *& data )
      {
        for ( unsigned int i = 0; i < nFooterWords(); ++i )
        {
          footerWords()[i] = *(data++);
        }
      }

      /// Write this head to a RAWBank of data words
      inline void fillRAWBank ( RAWBank & rawData ) const
      {
        for ( const auto& F : footerWords() ) { rawData.push_back(F); }
      }

    protected: // methods

      /// Set the data value using the given mask and shift values
      inline bool set( const ShortType value,
                       const ShortType shift,
                       const LongType  mask )
      {
        footerWords()[0] = ( ((value << shift) & mask) | (footerWords()[0] & ~mask) );
        return true;
      }

      /// tests whether a given value is in range for a given data field
      inline bool dataInRange( const ShortType value,
                               const ShortType max ) const
      {
        return ( value <= max );
      }

      /// Read/Write access to footer words
      inline FooterWords & footerWords() { return m_footerWords; }

      /// Set the given footer word
      void setWord( const ShortType word,
                    const LongType data )
      {
        footerWords()[word] = data;
      }

    private: // data

      /// The footer words
      FooterWords m_footerWords;

    };

  }
}

#endif // RICHDAQ_RichDAQFooterPDBase_H
