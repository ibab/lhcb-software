// $Id: RichDAQHeaderPD.h,v 1.1.1.1 2003-11-08 14:26:20 jonesc Exp $
#ifndef RICHDAQ_RICHDAQHEADERPD_H
#define RICHDAQ_RICHDAQHEADERPD_H 1

// Event model
#include "Event/DAQTypes.h"

// Include files

/** @class RichDAQHeaderPD RichDAQHeaderPD.h
 *
 *  Utility class representing the header word for HPD data
 *
 *  @author Chris Jones
 *  @date   2003-11-06
 */

class RichDAQHeaderPD {

public: // definitions

  typedef hlt_int        LongType;
  typedef unsigned int   ShortType;

public: // methods

  /// Standard constructor
  RichDAQHeaderPD() : m_data(0) { }

  /// Copy constructor
  RichDAQHeaderPD( const RichDAQHeaderPD & header ) : m_data( header.data() ) { }

  /// Constructor from LongType
  RichDAQHeaderPD( const LongType data ) : m_data( data ) { }

  /// Constructor from all data
  RichDAQHeaderPD( const bool zSupp,
                   const LongType linkN,
                   const ShortType hitCount,
                   const ShortType startPD = 1 ) : m_data(0)
  {
    setStartPD        ( startPD  );
    setZeroSuppressed ( zSupp    );
    setLinkNumber     ( linkN    );
    setHitCount       ( hitCount );
  }

  /// Destructor
  virtual ~RichDAQHeaderPD( ) {}

  /// Retrieve the full value
  inline LongType data() const { return m_data; }

  /// Update the internal data
  inline void setData( LongType data ) { m_data = data; }

  /// operator to convert to LongType
  inline operator LongType() const { return data(); }

  /// Set new PD bit
  inline void setStartPD( ShortType value)
  {
    set( value, ShiftStartPD, MaskStartPD );
  }

  /// Set the zero suppression info
  inline void setZeroSuppressed( bool zSupp )
  {
    ShortType i = ( zSupp ? 1 : 0 ); 
    set( i, ShiftZS, MaskZS );
  }

  /// Retrieve the zero suppressed information
  inline bool zeroSuppressed() const
  {
    return ( 0 != ( (data() & MaskZS) >> ShiftZS ) );
  }

  /// Set the link number info
  inline bool setLinkNumber( ShortType linkNum )
  {
    if ( !dataInRange(linkNum,MaxLinkNum) ) {
      std::cout << "RichDAQHeaderPD ERROR : Link number " 
                << linkNum << " out of range" << std::endl;
      return false;
    }
    return set( linkNum, ShiftLinkNum, MaskLinkNum );
  }

  /// Retrieve the link number
  inline ShortType linkNumber() const
  {
    return ( (data() & MaskLinkNum) >> ShiftLinkNum );
  }

  /// Set the hit count info
  inline bool setHitCount( ShortType hitCount )
  {
    if ( !dataInRange(hitCount,MaxHitCount) ) {
      std::cout << "RichDAQHeaderPD ERROR : Hit count " 
                << hitCount << " out of range" << std::endl;
      return false;
    }
    return set( hitCount, ShiftHitCount, MaskHitCount );
  }

  /// Retrieve the hit count number
  inline ShortType hitCount() const
  {
    return ( (data() & MaskHitCount) >> ShiftHitCount );
  }

private: // definitions

  // Define the number of bits for each field
  static const ShortType BitsHitCount   = 10;
  static const ShortType BitsLinkNum    = 12;
  static const ShortType BitsUnUsed1    = 7;
  static const ShortType BitsZS         = 1;
  static const ShortType BitsStartPD    = 1;

  // Create the shift registers
  static const ShortType ShiftHitCount  = 0;
  static const ShortType ShiftLinkNum   = ShiftHitCount + BitsHitCount;
  static const ShortType ShiftZS        = ShiftLinkNum  + BitsUnUsed1 + BitsLinkNum;
  static const ShortType ShiftStartPD   = ShiftZS       + BitsZS;

  // Create the Masks
  static const LongType MaskHitCount = (((LongType)1 << BitsHitCount)-1) << ShiftHitCount;
  static const LongType MaskLinkNum  = (((LongType)1 << BitsLinkNum)-1)  << ShiftLinkNum;
  static const LongType MaskZS       = (((LongType)1 << BitsZS)-1)       << ShiftZS;
  static const LongType MaskStartPD  = (((LongType)1 << BitsStartPD)-1)  << ShiftStartPD;

  // Create the max values that can be stored in each field
  static const ShortType MaxHitCount   = ( (ShortType)1 << BitsHitCount ) - 1;
  static const ShortType MaxLinkNum    = ( (ShortType)1 << BitsLinkNum )  - 1;

private: // methods

  inline bool set( const ShortType value,
                   const ShortType shift,
                   const LongType  mask )
  {
    setData( ((value << shift) & mask) | (data() & ~mask) );
    return true;
  }   

  // tests whether a given value is in range for a given data field
  inline bool dataInRange( const ShortType value,
                           const ShortType max ) const
  {
    return ( value <= max );
  }
  
private: // data

  LongType m_data;

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
