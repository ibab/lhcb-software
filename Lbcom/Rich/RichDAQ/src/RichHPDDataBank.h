
//-----------------------------------------------------------------------------
/** @file RichHPDDataBank.h
 *
 *  Header file for RICH DAQ utility class : RichHPDDataBank
 *
 *  CVS Log :-
 *  $Id: RichHPDDataBank.h,v 1.17 2006-09-21 08:30:59 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHHPDDATABANK_H
#define RICHDAQ_RICHHPDDATABANK_H 1

// STD
#include <sstream>

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// Kernel
#include "RichDet/RichDAQDefinitions.h"

//-----------------------------------------------------------------------------
/** @class RichHPDDataBank RichHPDDataBank.h
 *
 *  Abstract base class for all Rich HPD data bank implementations.
 *  Provides the interface for encoding and decoding.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */
//-----------------------------------------------------------------------------
class RichHPDDataBank
{
public:

  /** Decode the data bank to a RichSmartID vector
   *
   *  @param ids     Vector of RichSmartIDs to fill
   *  @param hpdID   RichSmartID for the HPD
   *
   *  @return Number of decoded hits
   */
  virtual RichDAQ::ShortType fillRichSmartIDs( LHCb::RichSmartID::Vector & ids,
                                               const LHCb::RichSmartID hpdID ) const = 0;

  /// Destructor
  virtual ~RichHPDDataBank() {};

  /** Fill a RAWBank with the data for this bank
   *
   *  @param rawData The raw data bank to fill
   */
  virtual void fillRAWBank( RichDAQ::RAWBank & rawData ) const = 0;

  /// Returns the L0ID
  virtual RichDAQ::Level0ID level0ID() const = 0;

  /// Returns the hit count for this HPD
  /// To be removed once DC04/DC06 support no longer needed
  virtual RichDAQ::ShortType hitCount() const = 0;

  /// Returns the number of header words for this HPD
  virtual RichDAQ::ShortType nHeaderWords() const = 0;

  /// Returns the number of footer words for this HPD
  virtual RichDAQ::ShortType nFooterWords() const = 0;

  /// Returns the number of data words for this HPD
  virtual RichDAQ::ShortType nDataWords() const = 0;

  /// Returns the total number of words for this HPD
  inline RichDAQ::ShortType nTotalWords() const
  {
    return nHeaderWords() + nFooterWords() + nDataWords();
  }

  /** Print data bank to message stream
   *
   *  @param os Stream to print to
   */
  virtual void fillMsgStream( MsgStream & os ) const = 0;

  /** Overloaded output to message stream
   *
   *  @param os   Output stream
   *  @param data HPD data bank to print
   */
  friend MsgStream & operator << ( MsgStream & os, const RichHPDDataBank & data )
  {
    data.fillMsgStream(os);
    return os;
  }

  /// perform any data quality checks that can be done (such as parity word etc.)
  virtual bool checkDataIntegrity( const LHCb::RichSmartID::Vector & ids,
                                   MsgStream & os ) const = 0;

};

//-----------------------------------------------------------------------------
/** @class RichHPDDataBank RichHPDDataBank.h
 *
 *  Implementation base class for all Rich HPD data bank implementations.
 *  Provides some core functionality
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */
//-----------------------------------------------------------------------------
template< class Version, class Header, class Footer >
class RichHPDDataBankImp : public RichHPDDataBank
{

public:

  /** Constructor with internal data representation

  *  @param header   Header word for this HPD data bank
  *  @param footer   Footer word for this HPD data bank
  *  @param dataSize Initialisation size for data bank
  *  @param dataInit Initialisation value for each word in the data bank
  *  @param maxDataSize Max possible data size
  */
  RichHPDDataBankImp( const Header &                 header,
                      const Footer &                 footer,
                      const RichDAQ::LongType        dataInit,
                      const RichDAQ::ShortType       maxDataSize,
                      const RichDAQ::ShortType       dataSize = 0 )
    : m_header       ( header                             ),
      m_data         ( new RichDAQ::LongType[maxDataSize] ),
      m_footer       ( footer                             ),
      m_dataSize     ( dataSize                           ),
      m_maxDataSize  ( maxDataSize                        ),
      m_internalData ( true                               )
  {
    for ( RichDAQ::ShortType i = 0; i < maxDataSize; ++i ) m_data[i] = dataInit;
  }

  /** Constructor from external data (RawBuffer)
   *
   *  @param data        Pointer to start of data block (including header)
   *  @param headerSize  Number of words in the header
   *  @param footerSize  Number of words in the footer
   *  @param dataSize    Initialisation size for data bank (excluding header)
   *  @param maxDataSize Max possible data size
   */
  RichHPDDataBankImp( const RichDAQ::LongType * data,
                      const Header &            header,
                      const Footer &            footer,
                      const RichDAQ::ShortType  maxDataSize,
                      const RichDAQ::ShortType  dataSize = 0 );

  /// Destructor
  virtual ~RichHPDDataBankImp()
  {
    cleanUp();
  }

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

  /** Decode the data bank to a RichSmartID vector
   *
   *  @param ids     Vector of RichSmartIDs to fill
   *  @param hpdID   RichSmartID for the HPD
   */
  virtual RichDAQ::ShortType fillRichSmartIDs( LHCb::RichSmartID::Vector & ids,
                                               const LHCb::RichSmartID hpdID ) const = 0;

  /// Returns the L0ID
  virtual RichDAQ::Level0ID level0ID() const;

  /// Returns the hit count for this HPD
  /// To be removed once DC04/DC06 support no longer needed
  virtual RichDAQ::ShortType hitCount() const = 0;

  /// Returns the number of header words for this HPD
  virtual RichDAQ::ShortType nHeaderWords() const;

  /// Returns the number of footer words for this HPD
  virtual RichDAQ::ShortType nFooterWords() const;

  /// Returns the number of data words for this HPD
  virtual RichDAQ::ShortType nDataWords() const;

  /** Fill a RAWBank with the data for this bank
   *
   *  @param rawData The raw data bank to fill
   */
  virtual void fillRAWBank( RichDAQ::RAWBank & rawData ) const;

  /// Creates the parity word from the list of hoit pixels
  RichDAQ::LongType createParityWord( const LHCb::RichSmartID::Vector & ids ) const;

  /// perform any data quality checks that can be done (such as parity word etc.)
  virtual bool checkDataIntegrity( const LHCb::RichSmartID::Vector & ids,
                                   MsgStream & os ) const;

private: // methods

  /// Dis-allow Default constructor
  RichHPDDataBankImp() { }

  /// Clean up data representation
  inline void cleanUp()
  {
    if ( m_internalData && m_data )
    {
      delete[] m_data;
      m_data = 0;
    }
  }

protected: // methods

  /// Add data point
  inline void addData( const RichDAQ::LongType data )
  {
    if ( m_dataSize > maxDataSize()-1 )
    {
      throw GaudiException("Attempt to fill more than MAX data words",
                           "*RichHPDDataBankImp*", StatusCode::SUCCESS );
    }
    m_data[m_dataSize++] = data;
  }

  /// Read only access to data bank
  inline const RichDAQ::LongType * data() const
  {
    return m_data;
  }

  /// Read/write only access to data bank
  inline RichDAQ::LongType * data()
  {
    return m_data;
  }

  /// Access the number of words in the data block
  inline RichDAQ::ShortType dataSize() const
  {
    return m_dataSize;
  }

  /// Returns the max possible data size for the data block
  inline RichDAQ::ShortType maxDataSize() const
  {
    return m_maxDataSize;
  }

  /// Test if a given bit in a word is set on
  inline bool
  isBitOn( const RichDAQ::LongType data, const RichDAQ::ShortType pos ) const
  {
    return ( 0 != (data & (1<<pos)) );
  }

  /// Set a given bit in a data word on
  inline void
  setBit( RichDAQ::ShortType & data, 
          const RichDAQ::ShortType pos,
          const RichDAQ::ShortType value = 1 ) const
  {
    data |= value<<pos;
  }

  /** Dump the raw header and data block to message stream
   *
   *  @param os Stream to print to
   */
  virtual void dumpAllBits( MsgStream & os ) const;

  /** Print data bank to message stream
   *
   *  @param os Stream to print to
   */
  virtual void fillMsgStream( MsgStream & os ) const;

private:

  /// turn a number into a hex string
  inline std::string asHex( const RichDAQ::LongType word ) const
  {
    std::ostringstream AsHex;
    AsHex << std::hex << word;
    std::string tmpW = AsHex.str();
    tmpW.resize(10,' ');
    return tmpW;
  }

private: // data

  /// HPD header word(s)
  Header m_header;

  /// Pointer to the data block (excluding header)
  RichDAQ::LongType * m_data;

  /// HPD Footer word(s)
  Footer m_footer;

  /// Size of data block (excluding header and footer)
  RichDAQ::ShortType m_dataSize;

  /// Maximum data block size (excluding header, 32 LHCb mode, 256 ALICE mode)
  RichDAQ::ShortType m_maxDataSize;

  /// Flag to indicate if data is external or internal
  bool m_internalData;

};

#endif // RICHDAQ_RICHHPDDATABANK_H
