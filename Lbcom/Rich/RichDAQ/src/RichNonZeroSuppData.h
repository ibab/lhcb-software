// $Id: RichNonZeroSuppData.h,v 1.7 2003-11-10 14:59:59 jonrob Exp $
#ifndef RICHDAQ_RICHNONZEROSUPPDATA_H
#define RICHDAQ_RICHNONZEROSUPPDATA_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// local
#include "RichDAQDefinitions.h"

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
  RichNonZeroSuppData( const Rich::SmartIDs & digits )
  {
    initData();
    for ( Rich::SmartIDs::const_iterator iDig = digits.begin();
          iDig != digits.end(); ++ iDig ) {
      setPixelActive( (*iDig).pixelRow(), (*iDig).pixelCol() );
    }
  }

  /// Constructor from a vector of MCRichDigits
  RichNonZeroSuppData( const MCRichDigitVector & digits )
  {
    initData();
    for ( MCRichDigitVector::const_iterator iDig = digits.begin();
          iDig != digits.end(); ++ iDig ) {
      setPixelActive( (*iDig)->key().pixelRow(), (*iDig)->key().pixelCol() );
    }
  }

  /// Constructor from an HltBank
  RichNonZeroSuppData( const HltBank & bank )
  {
    initData();
    // Loop over data entries and set data.
    for ( Rich::ShortType iData = 0; iData < dataSize(); ++iData ) {
      m_data[iData] = bank.data()[iData+1];// NB: Skip 0th row since this is the header...
    }
  }

  /// Destructor
  virtual ~RichNonZeroSuppData() {}

  /// Set a pixel as active
  inline void setPixelActive( const Rich::ShortType row, 
                              const Rich::ShortType col )
  {
    setBit( m_data[row], col );
  }

  /// Is a given pixel active ?
  inline bool isPixelActive( const Rich::ShortType row, 
                             const Rich::ShortType col ) const
  {
    return isBitOn( m_data[row], col );
  }

  /// Return data size
  inline Rich::ShortType dataSize() const
  {
    return MaxBits;
  }

  /// Read only access to data
  inline const Rich::LongType * data() const
  {
    return &m_data[0];
  }

  /// Write access to data
  inline Rich::LongType * data()
  {
    return &m_data[0];
  }

  /// Fill a vector with RichSmartIDs for hit pixels
  inline void fillSmartIDs( const Rich::ShortType rich,
                            const Rich::ShortType panel,
                            const Rich::ShortType pdRow,
                            const Rich::ShortType pdCol,
                            Rich::SmartIDs & ids ) const
  {
    for ( Rich::ShortType iRow = 0; iRow < dataSize(); ++iRow ) {
      for ( Rich::ShortType iCol = 0; iCol < dataSize(); ++iCol ) {
        if ( isPixelActive(iRow,iCol) ) {
          ids.push_back( RichSmartID( rich,panel,pdRow,pdCol,iRow,iCol ) );
        }
      }
    }
  }

  /// Fill a vector with HLT data words
  inline void fillHLT( Rich::HLTBank & hltData ) const
  {
    for ( Rich::ShortType iData = 0; iData < dataSize(); ++iData ) {
      hltData.push_back( m_data[iData] );
    }
  }

private: // definitions

  static const Rich::ShortType MaxBits = 32;

private: // methods

  /// Reset all data to zero
  inline void initData()
  {
    for ( Rich::ShortType i = 0; i < dataSize(); ++i ) { m_data[i] = 0; }
  }

  /// Test if a given bit in a word is set on
  inline bool isBitOn( const Rich::LongType data, const Rich::ShortType pos ) const
  {
    return ( 0 != (data & (1<<pos)) );
  }

  /// Set a given bit in a data word on
  inline void setBit( Rich::LongType & data, Rich::ShortType pos )
  {
    data |= 1<<pos;
  }

private: //data

  Rich::LongType m_data[MaxBits];

};

/// overloaded output to MsgStream
inline MsgStream & operator << ( MsgStream & os,
                                 const RichNonZeroSuppData & data )
{
  for ( Rich::ShortType iRow = 0; iRow < data.dataSize(); ++iRow ) {
    os << "  ";
    for ( Rich::ShortType iCol = 0; iCol < data.dataSize(); ++iCol ) {
      os << (bool)( data.data()[iRow] & (1<<iCol) ) << " ";
    }
    os << endreq;
  }
  return os;
}

#endif // RICHDAQ_RICHNONZEROSUPPDATA_H
