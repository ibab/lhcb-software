// $Id: RichNonZeroSuppData.h,v 1.5 2003-11-08 16:00:45 jonrob Exp $
#ifndef RICHDAQ_RICHNONZEROSUPPDATA_H 
#define RICHDAQ_RICHNONZEROSUPPDATA_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Include files
#include "Event/MCRichDigit.h"
#include "RichKernel/RichSmartID.h"

// Event
#include "Event/DAQTypes.h"
#include "Event/HltEvent.h"

/** @class RichNonZeroSuppData RichNonZeroSuppData.h
 *  
 *
 *  @author Chris Jones
 *  @date   2003-11-07
 */

class RichNonZeroSuppData {

public: // definitions

  typedef hlt_int        LongType;
  typedef unsigned int   ShortType;

public:

  /// Standard constructor
  RichNonZeroSuppData() { initData(); }

  /// Constructor from a vector of RichSmartIDs
  RichNonZeroSuppData( const std::vector<RichSmartID> & digits ) 
  { 
    initData(); 
    for ( std::vector<RichSmartID>::const_iterator iDig = digits.begin();
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
    for ( unsigned int iData = 0; iData < dataSize(); ++iData ) {
      m_data[iData] = bank.data()[iData+1];// NB> Skip 0th row since this is the header...
    }
  }

  /// Destructor
  virtual ~RichNonZeroSuppData() {}

  /// Set a pixel as active
  inline void setPixelActive( const ShortType row, const ShortType col ) 
  {
    setBit( m_data[row], col ); 
  }

  /// Is a given pixel active ?
  inline bool isPixelActive( const ShortType row, const ShortType col ) const
  {
    return isBitOn( m_data[row], col ); 
  }

  /// Return data size
  inline ShortType dataSize() const
  {
    return MaxBits;
  }

  /// Read only access to data
  inline const LongType * data() const
  {
    return &m_data[0];
  }

  /// Write access to data
  inline LongType * data()
  {
    return &m_data[0];
  }

  /// Fill a vector with RichSmartIDs for hit pixels
  void fillSmartIDs( const ShortType rich,
                     const ShortType panel,
                     const ShortType pdRow,
                     const ShortType pdCol,
                     std::vector<RichSmartID> & ids )
  {
    for ( unsigned int iRow = 0; iRow < dataSize(); ++iRow ) {
      for ( unsigned int iCol = 0; iCol < dataSize(); ++iCol ) {
        if ( isPixelActive(iRow,iCol) ) {
          ids.push_back( RichSmartID( rich,panel,pdRow,pdCol,iRow,iCol ) );
        }
      }
    }
  }
  
private: // definitions
  
  static const ShortType MaxBits = 32;

private: // methods

  /// Reset all data to zero
  inline void initData()
  {
    for ( unsigned int i = 0; i<MaxBits; ++i ) { m_data[i] = 0; }
  }
  
  /// Test if a given bit in a word is set on
  inline bool isBitOn( const LongType data, const ShortType pos ) const
  {
    return 0 != ( data & (1<<pos) );
  }
  
  /// Set a given bit in a data word on
  inline void setBit( LongType & data, ShortType pos ) 
  {
    data |= 1<<pos;
  }
  
private: //data
  
  LongType m_data[MaxBits];
  
};

/// overloaded output to MsgStream
inline MsgStream & operator << ( MsgStream & os, 
                                 const RichNonZeroSuppData & data )
{
  for ( unsigned int iRow = 0; iRow < data.dataSize(); ++iRow ) {
    os << "  ";
    for ( unsigned int iCol = 0; iCol < data.dataSize(); ++iCol ) {
      os << (bool)( data.data()[iRow] & (1<<iCol) ) << " ";
    }
    os << endreq;
  }
  return os;
}

#endif // RICHDAQ_RICHNONZEROSUPPDATA_H
