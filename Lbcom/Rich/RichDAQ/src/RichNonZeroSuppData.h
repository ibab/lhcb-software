// $Id: RichNonZeroSuppData.h,v 1.1.1.1 2003-11-08 14:26:20 jonesc Exp $
#ifndef RICHDAQ_RICHNONZEROSUPPDATA_H 
#define RICHDAQ_RICHNONZEROSUPPDATA_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Include files
#include "Event/MCRichDigit.h"
#include "RichKernel/RichSmartID.h"

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
    initData(); // zero all hits first
    for ( std::vector<RichSmartID>::const_iterator iDig = digits.begin();
          iDig != digits.end(); ++ iDig ) {
      setPixelActive( (*iDig).pixelRow(), (*iDig).pixelCol() );
    }
  }

  /// Constructor from a vector of MCRichDigits
  RichNonZeroSuppData( const MCRichDigitVector & digits ) 
  { 
    initData(); // zero all hits first
    for ( MCRichDigitVector::const_iterator iDig = digits.begin();
          iDig != digits.end(); ++ iDig ) {
      setPixelActive( (*iDig)->key().pixelRow(), (*iDig)->key().pixelCol() );
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

  /// return data size
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
  
private: // definitions
  
  static const ShortType MaxBits = 32;

private: // methods

  inline void initData()
  {
    for ( unsigned int i = 0; i<MaxBits; ++i ) { m_data[i] = 0; }
  }
  
  inline bool isBitOn( const LongType data, const ShortType pos ) const
  {
    return 0 != ( data & (1<<pos) );
  }
  
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
    os << " ";
    for ( unsigned int iCol = 0; iCol < data.dataSize(); ++iCol ) {
      os << (bool)( data.data()[iRow] & (1<<iCol) ) << " ";
    }
    os << endreq;
  }
  return os;
}

#endif // RICHDAQ_RICHNONZEROSUPPDATA_H
