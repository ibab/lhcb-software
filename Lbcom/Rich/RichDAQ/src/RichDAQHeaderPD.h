// $Id: RichDAQHeaderPD.h,v 1.4 2003-11-26 14:18:29 cattanem Exp $
#ifndef RICHDAQ_RICHDAQHEADERPD_H
#define RICHDAQ_RICHDAQHEADERPD_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// local
#include "RichDAQDefinitions.h"

// Include files

/** @class RichDAQHeaderPD RichDAQHeaderPD.h
 *
 *  Utility class representing the header word for HPD data
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
  // Define the number of bits for each field
  #define BitsHitCount    10
  #define BitsLinkNum     12
  #define BitsUnUsed1     7
  #define BitsZS          1
  #define BitsStartPD     1

  // Create the shift registers
  #define ShiftHitCount   0
  #define ShiftLinkNum    ShiftHitCount + BitsHitCount
  #define ShiftZS         ShiftLinkNum  + BitsUnUsed1 + BitsLinkNum
  #define ShiftStartPD    ShiftZS       + BitsZS

  // Create the Masks
  #define MaskHitCount  (((Rich::LongType)1 << BitsHitCount)-1) << ShiftHitCount
  #define MaskLinkNum   (((Rich::LongType)1 << BitsLinkNum)-1)  << ShiftLinkNum
  #define MaskZS        (((Rich::LongType)1 << BitsZS)-1)       << ShiftZS
  #define MaskStartPD   (((Rich::LongType)1 << BitsStartPD)-1)  << ShiftStartPD

  // Create the max values that can be stored in each field
  #define MaxHitCount    ( (Rich::ShortType)1 << BitsHitCount ) - 1
  #define MaxLinkNum     ( (Rich::ShortType)1 << BitsLinkNum )  - 1

class RichDAQHeaderPD {

public: // methods

  /// Standard constructor
  RichDAQHeaderPD() : m_data(0) { }

  /// Copy constructor
  RichDAQHeaderPD( const RichDAQHeaderPD & header ) : m_data( header.data() ) { }

  /// Constructor from Rich::LongType
  RichDAQHeaderPD( const Rich::LongType data ) : m_data( data ) { }

  /// Constructor from all data
  RichDAQHeaderPD( const bool zSupp,
                   const Rich::LongType linkN,
                   const Rich::ShortType hitCount,
                   const Rich::ShortType startPD = 1 ) : m_data(0)
  {
    setStartPD        ( startPD  );
    setZeroSuppressed ( zSupp    );
    setLinkNumber     ( linkN    );
    setHitCount       ( hitCount );
  }

  /// Destructor
  virtual ~RichDAQHeaderPD( ) {}

  /// Retrieve the full value
  inline Rich::LongType data() const { return m_data; }

  /// Update the internal data
  inline void setData( Rich::LongType data ) { m_data = data; }

  /// operator to convert to Rich::LongType
  inline operator Rich::LongType() const { return data(); }

  /// Set new PD bit
  inline void setStartPD( Rich::ShortType value)
  {
    set( value, ShiftStartPD, MaskStartPD );
  }

  /// Set the zero suppression info
  inline void setZeroSuppressed( bool zSupp )
  {
    Rich::ShortType i = ( zSupp ? 1 : 0 );
    set( i, ShiftZS, MaskZS );
  }

  /// Retrieve the zero suppressed information
  inline bool zeroSuppressed() const
  {
    return ( 0 != ( (data() & MaskZS) >> ShiftZS ) );
  }

  /// Set the link number info
  inline bool setLinkNumber( Rich::ShortType linkNum )
  {
    if ( !dataInRange(linkNum,MaxLinkNum) ) {
      std::cout << "RichDAQHeaderPD ERROR : Link number "
                << linkNum << " out of range" << std::endl;
      return false;
    }
    return set( linkNum, ShiftLinkNum, MaskLinkNum );
  }

  /// Retrieve the link number
  inline Rich::ShortType linkNumber() const
  {
    return ( (data() & MaskLinkNum) >> ShiftLinkNum );
  }

  /// Set the hit count info
  inline bool setHitCount( Rich::ShortType hitCount )
  {
    if ( !dataInRange(hitCount,MaxHitCount) ) {
      std::cout << "RichDAQHeaderPD ERROR : Hit count "
                << hitCount << " out of range" << std::endl;
      return false;
    }
    return set( hitCount, ShiftHitCount, MaskHitCount );
  }

  /// Retrieve the hit count number
  inline Rich::ShortType hitCount() const
  {
    return ( (data() & MaskHitCount) >> ShiftHitCount );
  }

private: // definitions


private: // methods

  inline bool set( const Rich::ShortType value,
                   const Rich::ShortType shift,
                   const Rich::LongType  mask )
  {
    setData( ((value << shift) & mask) | (data() & ~mask) );
    return true;
  }

  // tests whether a given value is in range for a given data field
  inline bool dataInRange( const Rich::ShortType value,
                           const Rich::ShortType max ) const
  {
    return ( value <= max );
  }

private: // data

  Rich::LongType m_data;

};

/// overloaded output to MsgStream
inline MsgStream & operator << ( MsgStream & os,
                                 const RichDAQHeaderPD & header )
{
  os << "ZeroSuppress = " << header.zeroSuppressed()
     << " LinkNumber = " << header.linkNumber()
     << " HitCount = " << header.hitCount();
  return os;
}

#endif // RICHDAQ_RICHDAQHEADERPD_H
