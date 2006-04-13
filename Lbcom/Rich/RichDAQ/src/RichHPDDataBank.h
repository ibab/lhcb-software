
//-----------------------------------------------------------------------------
/** @file RichHPDDataBank.h
 *
 *  Header file for RICH DAQ utility class : RichHPDDataBank
 *
 *  CVS Log :-
 *  $Id: RichHPDDataBank.h,v 1.12 2006-04-13 12:37:10 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHHPDDATABANK_H
#define RICHDAQ_RICHHPDDATABANK_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// Kernel
#include "RichDet/RichDAQDefinitions.h"

//-----------------------------------------------------------------------------
/** @class RichHPDDataBank RichHPDDataBank.h
 *
 *  Base class for all Rich HPD data bank implementations.
 *  Provides the interface for encoding and decoding.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */
//-----------------------------------------------------------------------------

class RichHPDDataBank 
{

public:

  /** Constructor with internal data representation

  *  @param header   Header word for this HPD data bank
  *  @param dataSize Initialisation size for data bank
  *  @param dataInit Initialisation value for each word in the data bank
  */
  RichHPDDataBank( const RichDAQ::LongType  header,
                   const RichDAQ::ShortType dataSize,
                   const RichDAQ::LongType  dataInit,
                   const RichDAQ::ShortType maxDataSize )
    : m_header       ( header                             ),
      m_data         ( new RichDAQ::LongType[maxDataSize] ),
      m_dataSize     ( dataSize                           ),
      m_maxDataSize  ( maxDataSize                        ),
      m_internalData ( true                               )
  {
    for ( RichDAQ::ShortType i = 0; i < dataSize; ++i ) m_data[i] = dataInit;
  }

  /** Constructor from external data (RawBuffer)
   *
   *  @param data     Pointer to start of data block (including header)
   *  @param dataSize Initialisation size for data bank
   */
  RichHPDDataBank( const RichDAQ::LongType * data,
                   const RichDAQ::ShortType dataSize,
                   const RichDAQ::ShortType maxDataSize )
    : m_header       ( *data                                     ),
      m_data         ( const_cast< RichDAQ::LongType * >(++data) ),
      m_dataSize     ( dataSize                                  ),
      m_maxDataSize  ( maxDataSize                               ),
      m_internalData ( false                                     ) { }

  /// Destructor
  virtual ~RichHPDDataBank()
  {
    cleanUp();
  }

  /// Read access to header
  inline RichDAQ::LongType header() const
  {
    return m_header;
  }

  /** Decode the data bank to a RichSmartID vector
   *
   *  @param ids     Vector of RichSmartIDs to fill
   *  @param hpdID   RichSmartID for the HPD
   */
  virtual void fillRichSmartIDs( LHCb::RichSmartID::Vector & ids,
                                 const LHCb::RichSmartID hpdID ) const = 0;

  /// Returns the L0ID
  virtual RichDAQ::Level0ID level0ID() const = 0;

  /// Returns the hit count for this HPD
  virtual RichDAQ::ShortType hitCount() const = 0;

  /** Fill a RAWBank with the data for this bank
   *
   *  @param rawData The raw data bank to fill
   */
  inline void fillRAWBank( RichDAQ::RAWBank & rawData ) const
  {
    // fill with header word
    rawData.push_back( header() );
    // ... then data words
    for ( RichDAQ::ShortType iData = 0; iData < dataSize(); ++iData )
    { 
      rawData.push_back( m_data[iData] ); 
    } 
  }

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

private: // methods

  /// Dis-allow Default constructor
  RichHPDDataBank() { }

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
                           "*RichHPDDataBank*", StatusCode::SUCCESS );
    }
    m_data[m_dataSize++] = data;
  }

  /// Read only access to data bank
  inline const RichDAQ::LongType * data() const
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
  setBit( RichDAQ::ShortType & data, const RichDAQ::ShortType pos )
  {
    data |= 1<<pos;
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
  virtual void fillMsgStream( MsgStream & os ) const = 0;

protected: // data

  /// HPD header word
  RichDAQ::LongType m_header;

  /// Pointer to the data block (not including header)
  RichDAQ::LongType * m_data;

  /// Size of data block (excluding header)
  RichDAQ::ShortType m_dataSize;

  /// Maximum data block size (excluding header, 32 LHCb mode, 256 ALICE mode)
  RichDAQ::ShortType m_maxDataSize;

  /// Flag to indicate if data is external or internal
  bool m_internalData;

};

#endif // RICHDAQ_RICHHPDDATABANK_H
