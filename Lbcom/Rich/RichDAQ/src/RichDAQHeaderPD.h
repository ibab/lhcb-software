// $Id: RichDAQHeaderPD.h,v 1.3 2003-11-10 14:59:59 jonrob Exp $
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

  // Define the number of bits for each field
  static const Rich::ShortType BitsHitCount   = 10;
  static const Rich::ShortType BitsLinkNum    = 12;
  static const Rich::ShortType BitsUnUsed1    = 7;
  static const Rich::ShortType BitsZS         = 1;
  static const Rich::ShortType BitsStartPD    = 1;

  // Create the shift registers
  static const Rich::ShortType ShiftHitCount  = 0;
  static const Rich::ShortType ShiftLinkNum   = ShiftHitCount + BitsHitCount;
  static const Rich::ShortType ShiftZS        = ShiftLinkNum  + BitsUnUsed1 + BitsLinkNum;
  static const Rich::ShortType ShiftStartPD   = ShiftZS       + BitsZS;

  // Create the Masks
  static const Rich::LongType MaskHitCount = (((Rich::LongType)1 << BitsHitCount)-1) << ShiftHitCount;
  static const Rich::LongType MaskLinkNum  = (((Rich::LongType)1 << BitsLinkNum)-1)  << ShiftLinkNum;
  static const Rich::LongType MaskZS       = (((Rich::LongType)1 << BitsZS)-1)       << ShiftZS;
  static const Rich::LongType MaskStartPD  = (((Rich::LongType)1 << BitsStartPD)-1)  << ShiftStartPD;

  // Create the max values that can be stored in each field
  static const Rich::ShortType MaxHitCount   = ( (Rich::ShortType)1 << BitsHitCount ) - 1;
  static const Rich::ShortType MaxLinkNum    = ( (Rich::ShortType)1 << BitsLinkNum )  - 1;

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
