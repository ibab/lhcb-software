// $Id: RichDAQHeaderPD.h,v 1.6 2004-06-29 19:32:03 jonrob Exp $
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

// Namespace for definitions related to RichDAQHeaderPD
namespace RichDAQHeaderPDCode {

  // Define the number of bits for each field
  static const RichDAQ::ShortType BitsHitCount  =  10;
  static const RichDAQ::ShortType BitsLinkNum   =  12;
  static const RichDAQ::ShortType BitsUnUsed1   =  7;
  static const RichDAQ::ShortType BitsZS        =  1;
  static const RichDAQ::ShortType BitsStartPD   =  1;

  // Create the shift registers
  static const RichDAQ::ShortType ShiftHitCount  = 0;
  static const RichDAQ::ShortType ShiftLinkNum   = ShiftHitCount + BitsHitCount;
  static const RichDAQ::ShortType ShiftZS        = ShiftLinkNum  + BitsUnUsed1 + BitsLinkNum;
  static const RichDAQ::ShortType ShiftStartPD   = ShiftZS       + BitsZS;

  // Create the Masks
  static const RichDAQ::LongType MaskHitCount  = ((1 << BitsHitCount)-1) << ShiftHitCount;
  static const RichDAQ::LongType MaskLinkNum   = ((1 << BitsLinkNum)-1)  << ShiftLinkNum;
  static const RichDAQ::LongType MaskZS        = ((1 << BitsZS)-1)       << ShiftZS;
  static const RichDAQ::LongType MaskStartPD   = ((1 << BitsStartPD)-1)  << ShiftStartPD;

  // Create the max values that can be stored in each field
  static const RichDAQ::ShortType MaxHitCount    = ( 1 << BitsHitCount ) - 1;
  static const RichDAQ::ShortType MaxLinkNum     = ( 1 << BitsLinkNum  ) - 1;

}

class RichDAQHeaderPD {

public: // methods

  /// Standard constructor
  RichDAQHeaderPD() : m_data(0) { }

  /// Copy constructor
  RichDAQHeaderPD( const RichDAQHeaderPD & header ) : m_data( header.data() ) { }

  /// Constructor from RichDAQ::LongType
  RichDAQHeaderPD( const RichDAQ::LongType data ) : m_data( data ) { }

  /// Constructor from all data
  RichDAQHeaderPD( const bool zSupp,
                   const RichDAQ::LongType linkN,
                   const RichDAQ::ShortType hitCount,
                   const RichDAQ::ShortType startPD = 1 ) : m_data(0)
  {
    setStartPD        ( startPD  );
    setZeroSuppressed ( zSupp    );
    setLinkNumber     ( linkN    );
    setHitCount       ( hitCount );
  }

  /// Destructor
  virtual ~RichDAQHeaderPD( ) {}

  /// Retrieve the full value
  inline RichDAQ::LongType data() const { return m_data; }

  /// Update the internal data
  inline void setData( const RichDAQ::LongType data ) { m_data = data; }

  /// operator to convert to RichDAQ::LongType
  inline operator RichDAQ::LongType() const { return data(); }

  /// Set new PD bit
  inline void setStartPD( const RichDAQ::ShortType value)
  {
    set( value, RichDAQHeaderPDCode::ShiftStartPD, 
         RichDAQHeaderPDCode::MaskStartPD );
  }

  /// Set the zero suppression info
  inline void setZeroSuppressed( const bool zSupp )
  {
    RichDAQ::ShortType i = ( zSupp ? 1 : 0 );
    set( i, RichDAQHeaderPDCode::ShiftZS, RichDAQHeaderPDCode::MaskZS );
  }

  /// Retrieve the zero suppressed information
  inline bool zeroSuppressed() const
  {
    return ( 0 != ( (data() & RichDAQHeaderPDCode::MaskZS) 
                    >> RichDAQHeaderPDCode::ShiftZS ) );
  }

  /// Set the link number info
  inline bool setLinkNumber( const RichDAQ::ShortType linkNum )
  {
    return ( dataInRange(linkNum,RichDAQHeaderPDCode::MaxLinkNum) ?
             set( linkNum, RichDAQHeaderPDCode::ShiftLinkNum, 
                  RichDAQHeaderPDCode::MaskLinkNum ) : false );
  }

  /// Retrieve the link number
  inline RichDAQ::ShortType linkNumber() const
  {
    return ( (data() & RichDAQHeaderPDCode::MaskLinkNum) 
             >> RichDAQHeaderPDCode::ShiftLinkNum );
  }

  /// Set the hit count info
  inline bool setHitCount( const RichDAQ::ShortType hitCount )
  {
    return ( dataInRange(hitCount,RichDAQHeaderPDCode::MaxHitCount) ?
             set( hitCount, RichDAQHeaderPDCode::ShiftHitCount, 
                  RichDAQHeaderPDCode::MaskHitCount ) : false );
  }

  /// Retrieve the hit count number
  inline RichDAQ::ShortType hitCount() const
  {
    return ( (data() & RichDAQHeaderPDCode::MaskHitCount) 
             >> RichDAQHeaderPDCode::ShiftHitCount );
  }

private: // methods

  inline bool set( const RichDAQ::ShortType value,
                   const RichDAQ::ShortType shift,
                   const RichDAQ::LongType  mask )
  {
    setData( ((value << shift) & mask) | (data() & ~mask) );
    return true;
  }

  // tests whether a given value is in range for a given data field
  inline bool dataInRange( const RichDAQ::ShortType value,
                           const RichDAQ::ShortType max ) const
  {
    return ( value <= max );
  }

private: // data

  RichDAQ::LongType m_data;

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
