
/** @file RichNonZeroSuppData.h
 *
 *  Header file for RICH DAQ utility class : RichNonZeroSuppData
 *
 *  CVS Log :-
 *  $Id: RichNonZeroSuppData.h,v 1.11 2004-07-27 13:46:07 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-07
 */

#ifndef RICHDAQ_RICHNONZEROSUPPDATA_H
#define RICHDAQ_RICHNONZEROSUPPDATA_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// local
#include "RichDAQDefinitions.h"

/** @namespace RichZSHitTripletCode
 *
 *  Namespace for definitions related to RichNonZeroSuppData
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
namespace RichNonZeroSuppDataCode {

  /// Maximum number of bits
  static const RichDAQ::ShortType MaxBits = 32;

}

/** @class RichNonZeroSuppData RichNonZeroSuppData.h
 *
 *  The non zero-suppressed data format
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-07
 */

class RichNonZeroSuppData {

public:

  /// Standard constructor
  RichNonZeroSuppData() { initData(); }

  /// Constructor from a vector of RichSmartIDs
  explicit RichNonZeroSuppData( const RichDAQ::SmartIDs & digits )
  {
    initData();
    for ( RichDAQ::SmartIDs::const_iterator iDig = digits.begin();
          iDig != digits.end(); ++ iDig ) {
      setPixelActive( (*iDig).pixelRow(), (*iDig).pixelCol() );
    }
  }

  /// Constructor from a vector of MCRichDigits
  explicit RichNonZeroSuppData( const MCRichDigitVector & digits )
  {
    initData();
    for ( MCRichDigitVector::const_iterator iDig = digits.begin();
          iDig != digits.end(); ++ iDig ) {
      setPixelActive( (*iDig)->key().pixelRow(), (*iDig)->key().pixelCol() );
    }
  }

  /// Constructor from a RawBank
  explicit RichNonZeroSuppData( const RawBank & bank )
  {
    initData();
    // Loop over data entries and set data.
    for ( RichDAQ::ShortType iData = 0; iData < dataSize(); ++iData ) {
      // NB: Skip 0th row since this is the header...
      m_data[iData] = bank.data()[iData+1];
    }
  }

  /// Destructor
  ~RichNonZeroSuppData() {}

  /// Set a pixel as active
  inline void setPixelActive( const RichDAQ::ShortType row,
                              const RichDAQ::ShortType col )
  {
    setBit( m_data[row], col );
  }

  /// Is a given pixel active ?
  inline bool isPixelActive( const RichDAQ::ShortType row,
                             const RichDAQ::ShortType col ) const
  {
    return isBitOn( m_data[row], col );
  }

  /// Return data size
  inline RichDAQ::ShortType dataSize() const
  {
    return RichNonZeroSuppDataCode::MaxBits;
  }

  /// Read only access to data
  inline const RichDAQ::LongType * data() const
  {
    return &m_data[0];
  }

  /// Write access to data
  inline RichDAQ::LongType * data()
  {
    return &m_data[0];
  }

  /// Fill a vector with RichSmartIDs for hit pixels
  inline void fillSmartIDs( const RichDAQ::ShortType rich,
                            const RichDAQ::ShortType panel,
                            const RichDAQ::ShortType pdRow,
                            const RichDAQ::ShortType pdCol,
                            RichDAQ::SmartIDs & ids ) const
  {
    for ( RichDAQ::ShortType iRow = 0; iRow < dataSize(); ++iRow ) {
      for ( RichDAQ::ShortType iCol = 0; iCol < dataSize(); ++iCol ) {
        if ( isPixelActive(iRow,iCol) ) {
          ids.push_back( RichSmartID( rich,panel,pdRow,pdCol,iRow,iCol ) );
        }
      }
    }
  }

  /// Fill a vector with Raw data words
  inline void fillRAW( RichDAQ::RAWBank & rawData ) const
  {
    for ( RichDAQ::ShortType iData = 0; iData < dataSize(); ++iData ) {
      rawData.push_back( m_data[iData] );
    }
  }

private: // methods

  /// Reset all data to zero
  inline void initData()
  {
    for ( RichDAQ::ShortType i = 0; i < dataSize(); ++i ) { m_data[i] = 0; }
  }

  /// Test if a given bit in a word is set on
  inline bool
  isBitOn( const RichDAQ::LongType data, const RichDAQ::ShortType pos ) const
  {
    return ( 0 != (data & (1<<pos)) );
  }

  /// Set a given bit in a data word on
  inline void setBit( RichDAQ::LongType & data, const RichDAQ::ShortType pos )
  {
    data |= 1<<pos;
  }

private: //data

  RichDAQ::LongType m_data[RichNonZeroSuppDataCode::MaxBits];

};

/// overloaded output to MsgStream
inline MsgStream & operator << ( MsgStream & os,
                                 const RichNonZeroSuppData & data )
{
  for ( RichDAQ::ShortType iRow = 0; iRow < data.dataSize(); ++iRow ) {
    os << "  ";
    for ( RichDAQ::ShortType iCol = 0; iCol < data.dataSize(); ++iCol ) {
      os << static_cast<bool>( data.data()[iRow] & (1<<iCol) ) << " ";
    }
    os << endreq;
  }
  return os;
}

#endif // RICHDAQ_RICHNONZEROSUPPDATA_H
