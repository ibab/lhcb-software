
/** @file RichHPDDataBank.h
 *
 *  Header file for RICH DAQ utility class : RichHPDDataBank
 *
 *  CVS Log :-
 *  $Id: RichHPDDataBank.h,v 1.1 2005-01-07 12:35:59 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */

#ifndef RICHDAQ_RICHHPDDATABANK_H
#define RICHDAQ_RICHHPDDATABANK_H 1

// Kernel
#include "RichKernel/RichDAQDefinitions.h"

// HPD Tool
#include "RichKernel/IRichHPDIDTool.h"

/** @class RichHPDDataBank RichHPDDataBank.h
 *
 *  Base class for all Rich HPD data bank implementations.
 *  Provides the interface for encoding and decoding.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */

class RichHPDDataBank {

public:

  /** Constructor with internal data representation

  *  @param header   Header word for this HPD data bank
  *  @param dataSize Initialisation size for data bank
  *  @param dataInit Initialisation value for words in data bank
  */
  RichHPDDataBank( const RichDAQ::LongType  header,
                   const RichDAQ::ShortType dataSize,
                   const RichDAQ::LongType  dataInit )
    : m_header       ( header   ),
      m_dataSize     ( dataSize ),
      m_internalData ( true     )
  {
    m_data = new RichDAQ::LongType[32];
    for ( RichDAQ::ShortType i = 0; i < dataSize; ++i ) m_data[i] = dataInit;
  }

  /** Constructor from external data (RawBuffer)
   *
   *  @param data     Pointer to start of data block (including header)
   *  @param dataSize Initialisation size for data bank
   */
  RichHPDDataBank( const RichDAQ::LongType * data,
                   const RichDAQ::ShortType dataSize )
    : m_header       ( *data    ),
      m_data         ( const_cast< RichDAQ::LongType * >(++data) ),
      m_dataSize     ( dataSize ),
      m_internalData ( false    ) { }

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
   *  @param hpdTool Pointer to the tool to convert hardware to software HPD IDs
   */
  virtual void fillRichSmartIDs( RichSmartID::Collection & ids,
                                 const IRichHPDIDTool * hpdTool ) const = 0;

  /** Fill a RAWBank with the data for this bank
   *
   *  @param rawData The raw data bank to fill
   */
  virtual void fillRAWBank( RichDAQ::RAWBank & rawData ) const = 0;

  /** Print data bank to Gaudi MsgStream
   *
   *  @param os Stream to print to
   */
  virtual void fillMsgStream( MsgStream & os ) const = 0;

private: // methods

  /// Ass-allow Default constructor
  RichHPDDataBank() { }

  /// Clean up data representation
  inline void cleanUp()
  {
    if ( m_internalData && m_data )
    {
      delete[] m_data; m_data = 0;
    }
  }

protected: // methods

  /// Add a data point
  inline void addData( const RichDAQ::LongType data )
  {
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

  /// Dump the raw header and data block to MsgStream
  void dumpAllBits( MsgStream & os ) const;

protected: // data

  /// HPD header word
  RichDAQ::LongType m_header;

  /// Pointer to the data block (not including header)
  RichDAQ::LongType * m_data;

  /// Size of data block (excluding header)
  RichDAQ::ShortType m_dataSize;

  /// Flag to indicate if data is external or internal
  bool m_internalData;

};

/// overloaded output to MsgStream
inline MsgStream & operator << ( MsgStream & os, const RichHPDDataBank & data )
{
  data.fillMsgStream(os);
  return os;
}

#endif // RICHDAQ_RICHHPDDATABANK_H
