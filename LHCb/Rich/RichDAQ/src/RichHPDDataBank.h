
//-----------------------------------------------------------------------------
/** @file RichHPDDataBank.h
 *
 *  Header file for RICH DAQ utility class : Rich::DAQ::HPDDataBank
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHHPDDATABANK_H
#define RICHDAQ_RICHHPDDATABANK_H 1

// STD
#include <sstream>
#include <stdio.h>

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// Kernel
#include "RichKernel/RichDAQDefinitions.h"
#include "RichKernel/RichDAQHeaderPDBase.h"
#include "RichKernel/RichDAQFooterPDBase.h"

namespace Rich
{
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class HPDDataBank RichHPDDataBank.h
     *
     *  Abstract base class for all Rich HPD data bank implementations.
     *  Provides the interface for encoding and decoding.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2004-12-18
     */
    //-----------------------------------------------------------------------------
    class HPDDataBank
    {

    public:

      /** Decode the data bank to a RichSmartID vector
       *
       *  @param ids     Vector of RichSmartIDs to fill
       *  @param hpdID   RichSmartID for the HPD
       *
       *  @return Number of decoded hits
       */
      virtual ShortType fillRichSmartIDs( LHCb::RichSmartID::Vector & ids,
                                          const LHCb::RichSmartID hpdID ) const = 0;

      /// Destructor
      virtual ~HPDDataBank() {};

      /** Fill a RAWBank with the data for this bank
       *
       *  @param rawData The raw data bank to fill
       */
      virtual void fillRAWBank( RAWBank & rawData ) const = 0;

      /// Returns the L0ID
      virtual Level0ID level0ID() const = 0;

      /// Returns the hit count for this HPD.
      /// To be removed once DC04/DC06 support no longer needed
      virtual ShortType hitCount() const = 0;

      /// Returns the number of header words for this HPD
      virtual ShortType nHeaderWords() const = 0;

      /// Returns the number of footer words for this HPD
      virtual ShortType nFooterWords() const = 0;

      /// Returns the number of data words for this HPD
      virtual ShortType nDataWords() const = 0;

      /// Returns the Event ID word
      virtual EventID eventID() const = 0;

      /// Returns the total number of words for this HPD
      inline ShortType nTotalWords() const
      {
        return nHeaderWords() + nFooterWords() + nDataWords();
      }

      /// Read access to header
      virtual const HeaderPDBase::HeaderWords & headerWords() const = 0;
      
      /// Read access to footer
      virtual const FooterPDBase::FooterWords & footerWords() const = 0;

      /// Is the data from this HPD suppressed
      virtual bool suppressed() const = 0;

      /** Print data bank to message stream
       *  @param os Stream to print to
       */
      virtual void fillMsgStream( MsgStream & os ) const = 0;

      /** Overloaded output to message stream
       *  @param os   Output stream
       *  @param data HPD data bank to print
       */
      friend MsgStream & operator << ( MsgStream & os, const HPDDataBank & data )
      {
        data.fillMsgStream(os);
        return os;
      }

      /// perform any data quality checks that can be done (such as parity word etc.)
      virtual bool checkDataIntegrity( const LHCb::RichSmartID::Vector & ids,
                                       MsgStream & os ) const = 0;

    };

    //-----------------------------------------------------------------------------
    /** @class HPDDataBankImp RichHPDDataBank.h
     *
     *  Implementation base class for all Rich HPD data bank implementations.
     *  Provides some core functionality
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2004-12-18
     */
    //-----------------------------------------------------------------------------
    template< class Version, class Header, class Footer >
    class HPDDataBankImp : public HPDDataBank
    {

    public:

      /// Default Constructor
      HPDDataBankImp( const ShortType maxDataSize = 1 )
        : m_data         ( new LongType[maxDataSize] ),
          m_maxDataSize  ( maxDataSize ),
          m_internalData ( true ) 
      { 
        for ( ShortType i = 0; i < maxDataSize; ++i ) m_data[i] = 0;
        // memset ( m_data, 0, sizeof(m_data) );
        // for ( ShortType i = 0; i < maxDataSize; ++i ) 
        // { std::cout << i << " " << m_data[i] << std::endl; }
      }

    public:

      /** Constructor with internal data representation (encoding)
       *
       *  @param header   Header word for this HPD data bank
       *  @param footer   Footer word for this HPD data bank
       *  @param dataInit Initialisation value for each word in the data bank
       *  @param maxDataSize Max possible data size
       *  @param dataSize Initialisation size for data bank
       */
      HPDDataBankImp( const Header &        header,
                      const Footer &        footer,
                      const LongType        dataInit,
                      const ShortType       maxDataSize,
                      const ShortType       dataSize = 0 )
        : m_header       ( header                    ),
          m_data         ( new LongType[maxDataSize] ),
          m_footer       ( footer                    ),
          m_dataSize     ( dataSize                  ),
          m_maxDataSize  ( maxDataSize               ),
          m_internalData ( true                      )
      {
        for ( ShortType i = 0; i < maxDataSize; ++i ) m_data[i] = dataInit;
      }
      
    public:

      /** Constructor from external data (decoding RawBuffer)
       *
       *  @param data        Pointer to start of data block (including header)
       *  @param maxDataSize Max possible data size
       *  @param dataSize    Initialisation size for data bank (excluding header)
       */
      HPDDataBankImp( const LongType * data,
                      const ShortType  maxDataSize,
                      const ShortType  dataSize = 0 )
        : m_maxDataSize  ( maxDataSize ),
          m_internalData ( false       )
      {
        init( data, dataSize );
      }

    protected:

      /// Reset object to decode a new data stream
      inline void reset( const LongType * data, 
                         const ShortType  dataSize = 0 )
      {
        // reset header and footer to default size if needed
        m_header.reset();
        if ( m_footer.nFooterWords() > 0 ) { m_footer = Footer(); }
        // cleanup internal storage, if required
        cleanUp();
        // initialise
        m_internalData = false;
        init( data, dataSize );
      }

    private:

      /// initialisation for decoding
      void init( const LongType * data,
                 const ShortType  dataSize );

    public:

      /// Destructor
      virtual ~HPDDataBankImp() { cleanUp(); }

    public:

      /// Read access to header
      inline const Header & header() const
      {
        return m_header;
      }

      /// Set the header
      inline void setHeader( const Header & head )
      {
        m_header = head;
      }

      /// Read access to footer
      inline const Footer & footer() const
      {
        return m_footer;
      }

      /// Set the footer
      inline void setFooter( const Footer & footer )
      {
        m_footer = footer;
      }

      /// Read access to header
      virtual const HeaderPDBase::HeaderWords & headerWords() const;
      
      /// Read access to footer
      virtual const FooterPDBase::FooterWords & footerWords() const;

      /** Decode the data bank to a RichSmartID vector
       *
       *  @param ids     Vector of RichSmartIDs to fill
       *  @param hpdID   RichSmartID for the HPD
       */
      virtual ShortType fillRichSmartIDs( LHCb::RichSmartID::Vector & ids,
                                          const LHCb::RichSmartID hpdID ) const = 0;

      /// Returns the L0ID
      virtual Level0ID level0ID() const;

      /// Returns the hit count for this HPD
      /// To be removed once DC04/DC06 support no longer needed
      virtual ShortType hitCount() const = 0;

      /// Returns the number of header words for this HPD
      virtual ShortType nHeaderWords() const;

      /// Returns the number of footer words for this HPD
      virtual ShortType nFooterWords() const;

      /// Returns the number of data words for this HPD
      virtual ShortType nDataWords() const;

      /// Returns the Event ID word
      virtual EventID eventID() const;

      /// Checks for errors in the header word
      virtual bool suppressed() const;

      /** Fill a RAWBank with the data for this bank
       *
       *  @param rawData The raw data bank to fill
       */
      virtual void fillRAWBank( RAWBank & rawData ) const;

      /// Creates the parity word from the list of hoit pixels
      LongType createParityWord( const LHCb::RichSmartID::Vector & ids ) const;

      /// perform any data quality checks that can be done (such as parity word etc.)
      virtual bool checkDataIntegrity( const LHCb::RichSmartID::Vector & ids,
                                       MsgStream & os ) const;

    private: // methods

      /// Clean up data representation
      inline void cleanUp()
      {
        if ( m_internalData && m_data )
        {
          delete[] m_data;
          m_data = NULL;
        }
      }

    protected: // methods

      /// Add data point
      inline void addData( const LongType data )
      {
        if ( m_dataSize > maxDataSize()-1 )
        {
          throw GaudiException("Attempt to fill more than MAX data words",
                               "*Rich::DAQ::HPDDataBankImp*", StatusCode::SUCCESS );
        }
        m_data[m_dataSize++] = data;
      }

      /// Read only access to data bank
      inline const LongType * data() const
      {
        return m_data;
      }

      /// Read/write only access to data bank
      inline LongType * data()
      {
        return m_data;
      }

      /// Access the number of words in the data block
      inline ShortType dataSize() const
      {
        return m_dataSize;
      }

      /// Returns the max possible data size for the data block
      inline ShortType maxDataSize() const
      {
        return m_maxDataSize;
      }

      /// Test if a given bit in a word is set on
      inline bool
      isBitOn( const LongType& data, const ShortType pos ) const
      {
        return ( 0 != (data & (1<<pos)) );
      }

      /// Set a given bit in a data word on
      inline void
      setBit( ShortType & data,
              const ShortType pos,
              const ShortType value = 1 ) const
      {
        data |= value<<pos;
      }

      /// Test if a LongType is even or odd
      inline bool isEven( const LongType data ) const
      {
        return ( (data & 0x1) == 0 );
      }

      /** Dump the raw header and data block to message stream
       *
       *  @param os Stream to print to
       */
      void dumpAllBits( MsgStream & os ) const;

      /** Print data bank to message stream
       *
       *  @param os Stream to print to
       */
      void fillMsgStream( MsgStream & os ) const;

    private:

      /// Turn a number into a hex string, with leading zeros padded to length 8
      inline std::string asHex( const LongType word ) const
      {
        std::ostringstream AsHex;
        AsHex << std::hex << word;
        std::string tmpW = AsHex.str();
        return ( tmpW.size() < 8 ? std::string(8-tmpW.size(),'0')+tmpW : tmpW );
      }

    private: // data

      /// HPD header word(s)
      Header m_header;

      /// Pointer to the data block (excluding header)
      LongType * m_data;

      /// HPD Footer word(s)
      Footer m_footer;

      /// Size of data block (excluding header and footer)
      ShortType m_dataSize;

      /// Maximum data block size (excluding header, 32 LHCb mode, 256 ALICE mode)
      ShortType m_maxDataSize;

      /// Flag to indicate if data is external or internal
      bool m_internalData;

    };

  }
}

#endif // RICHDAQ_RICHHPDDATABANK_H
