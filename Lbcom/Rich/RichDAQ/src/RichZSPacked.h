
/** @file RichZSPacked.h
 *
 *  Header file for RICH DAQ utility class : RichZSPacked
 *
 *  CVS Log :-
 *  $Id: RichZSPacked.h,v 1.2 2005-06-23 14:50:28 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

#ifndef RICHDAQ_RICHZSPACKED_H
#define RICHDAQ_RICHZSPACKED_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// Kernel
#include "RichKernel/RichDAQDefinitions.h"

/** @namespace RichZSPackedCode
 *
 *  Namespace for definitions related to RichZSPackedCode
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-12
 */
namespace RichZSPackedCode {

  /// Number of bits for each address
  static const RichDAQ::ShortType BitsAddress = 7;
  /// Number of bits for each bit field
  static const RichDAQ::ShortType BitsField   = 8;

  // shift registers
  static const RichDAQ::ShortType ShiftField0    = 0;
  static const RichDAQ::ShortType ShiftAddress0  = ShiftField0 + BitsField;
  static const RichDAQ::ShortType ShiftField1    = ShiftAddress0 + 1 + BitsAddress;
  static const RichDAQ::ShortType ShiftAddress1  = ShiftField1 + BitsField;

  // The masks
  static const RichDAQ::LongType  MaskField0   = ((1 << BitsField)-1)   << ShiftField0;
  static const RichDAQ::LongType  MaskAddress0 = ((1 << BitsAddress)-1) << ShiftAddress0;
  static const RichDAQ::LongType  MaskField1   = ((1 << BitsField)-1)   << ShiftField1;
  static const RichDAQ::LongType  MaskAddress1 = ((1 << BitsAddress)-1) << ShiftAddress1;

  // the max value of address and bit-field storable
  static const RichDAQ::ShortType MaxField     = ( 1 << BitsField   ) - 1;
  static const RichDAQ::ShortType MaxAddress   = ( 1 << BitsAddress ) - 1;

}

/** @class RichZSPacked RichZSPacked.h
 *
 *  Utility class representing 2 groups of address and 
 *  bit-field packed into a single word
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-12
 */

class RichZSPacked {

public: // methods

  /// Copy Constructor
  RichZSPacked( const RichZSPacked & word ) : m_data( word.data() ) {}

  /// Constructor from RichDAQ::LongType
  RichZSPacked( const RichDAQ::LongType data = 0 ) : m_data( data ) { }

  /// Constructor from address and field values
  RichZSPacked( const RichDAQ::ShortType address0 ,  ///< First address to store
                const RichDAQ::ShortType field0   ,  ///< First bitfield to store
                const RichDAQ::ShortType address1 ,  ///< Second address to store
                const RichDAQ::ShortType field1      ///< Second bitfield to store
                )
    : m_data ( 0 )
  {
    setAddress0  ( address0 );
    setBitField0 ( field0   );
    setAddress1  ( address1 );
    setBitField1 ( field1   );
  }

  /// Destructor
  ~RichZSPacked( ) { }

  /// Retrieve the full value
  inline RichDAQ::LongType data() const { return m_data; }

  /// operator to convert to RichDAQ::LongType
  inline operator RichDAQ::LongType() const { return data(); }

  /// Set the first address
  inline void setAddress0( const RichDAQ::ShortType address )
  {
    dataInRange( address, RichZSPackedCode::MaxAddress ); 
    set( address, RichZSPackedCode::ShiftAddress0, RichZSPackedCode::MaskAddress0 );
  }

  /// Set the second address
  inline void setAddress1( const RichDAQ::ShortType address )
  {
    dataInRange( address, RichZSPackedCode::MaxAddress ); 
    set( address, RichZSPackedCode::ShiftAddress1, RichZSPackedCode::MaskAddress1 );
  }

  /// Set the first bit-field
  inline void setBitField0( const RichDAQ::ShortType field )
  {
    dataInRange( field, RichZSPackedCode::MaxField ); 
    set( field, RichZSPackedCode::ShiftField0, RichZSPackedCode::MaskField0 );
  }

  /// Set the second bit-field
  inline void setBitField1( const RichDAQ::ShortType field )
  {
    dataInRange( field, RichZSPackedCode::MaxField ); 
    set( field, RichZSPackedCode::ShiftField1, RichZSPackedCode::MaskField1 );
  }

  /// Retrieve the first address
  inline RichDAQ::ShortType address0() const
  {
    return ( (data() & RichZSPackedCode::MaskAddress0) >> RichZSPackedCode::ShiftAddress0 );
  }

  /// Retrieve the first address
  inline RichDAQ::ShortType address1() const
  {
    return ( (data() & RichZSPackedCode::MaskAddress1) >> RichZSPackedCode::ShiftAddress1 );
  }

  /// Retrieve the first bit-field
  inline RichDAQ::ShortType bitField0() const
  {
    return ( (data() & RichZSPackedCode::MaskField0) >> RichZSPackedCode::ShiftField0 );
  }

  /// Retrieve the second bit-field
  inline RichDAQ::ShortType bitField1() const
  {
    return ( (data() & RichZSPackedCode::MaskField1) >> RichZSPackedCode::ShiftField1 );
  }

private: // methods

  /// Update the internal data
  inline void setData( const RichDAQ::LongType data ) { m_data = data; }

  /// Set the data value for a given mask and shift value
  inline void set( const RichDAQ::ShortType value,
                   const RichDAQ::ShortType shift,
                   const RichDAQ::LongType  mask )
  {
    setData( ((value << shift) & mask) | (data() & ~mask) );
  }

  /// tests whether a given value is in range for a given data field
  inline void dataInRange( const RichDAQ::ShortType value,
                           const RichDAQ::ShortType max ) const
  {
    if ( value > max ) throw GaudiException( "Data out of range", "*RichZSPacked*", StatusCode::FAILURE );
  }

private: // data

  /// The data word
  RichDAQ::LongType m_data;

};

#endif // RICHDAQ_RICHZSPACKED_H
