
/** @file RichZSHitTriplet.h
 *
 *  Header file for RICH DAQ utility class : RichZSHitTriplet
 *
 *  CVS Log :-
 *  $Id: RichZSHitTriplet.h,v 1.9 2005-01-13 17:31:47 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.8  2005/01/13 13:12:41  jonrob
 *  Various updates
 *
 *  Revision 1.7  2005/01/07 12:35:59  jonrob
 *  Complete rewrite
 *
 *  Revision 1.6  2004/07/27 13:46:07  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

#ifndef RICHDAQ_RICHZSHITTRIPLET_H
#define RICHDAQ_RICHZSHITTRIPLET_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// Kernel
#include "RichKernel/RichDAQDefinitions.h"

/** @namespace RichZSHitTripletCode
 *
 *  Namespace for definitions related to RichZSHitTripletCode
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
namespace RichZSHitTripletCode {

  /// Number of bits for each row or column number
  static const RichDAQ::ShortType BitsHit    = 5;

  // shift registers
  static const RichDAQ::ShortType ShiftRow0  = 0;
  static const RichDAQ::ShortType ShiftCol0  = ShiftRow0 + BitsHit;
  static const RichDAQ::ShortType ShiftRow1  = ShiftCol0 + BitsHit;
  static const RichDAQ::ShortType ShiftCol1  = ShiftRow1 + BitsHit;
  static const RichDAQ::ShortType ShiftRow2  = ShiftCol1 + BitsHit;
  static const RichDAQ::ShortType ShiftCol2  = ShiftRow2 + BitsHit;

  // The masks
  static const RichDAQ::LongType  MaskRow0   = ((1 << BitsHit)-1) << ShiftRow0;
  static const RichDAQ::LongType  MaskCol0   = ((1 << BitsHit)-1) << ShiftCol0;
  static const RichDAQ::LongType  MaskRow1   = ((1 << BitsHit)-1) << ShiftRow1;
  static const RichDAQ::LongType  MaskCol1   = ((1 << BitsHit)-1) << ShiftCol1;
  static const RichDAQ::LongType  MaskRow2   = ((1 << BitsHit)-1) << ShiftRow2;
  static const RichDAQ::LongType  MaskCol2   = ((1 << BitsHit)-1) << ShiftCol2;

  // the max row/column values storable
  static const RichDAQ::ShortType MaxRowCol  = ( 1 << BitsHit ) - 1;

}

/** @class RichZSHitTriplet RichZSHitTriplet.h
 *
 *  Utility class representing three hits packed into a single word
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

class RichZSHitTriplet {

public: // methods

  /// Copy Constructor
  RichZSHitTriplet( const RichZSHitTriplet & hits ) : m_data( hits.data() ) {}

  /// Constructor from RichDAQ::LongType
  RichZSHitTriplet( const RichDAQ::LongType data = 0 ) : m_data( data ) { }

  /// Constructor from three MCRichDigits
  RichZSHitTriplet( const RichSmartID * digOne   ,  ///< Pointer to first RichSmartID to store
                    const RichSmartID * digTwo   ,  ///< Pointer to second RichSmartID to store
                    const RichSmartID * digThree    ///< Pointer to third RichSmartID to store
                    )
    : m_data ( 0 )
  {
    if ( digOne ) {
      setRow0 ( digOne->pixelRow() );
      setCol0 ( digOne->pixelCol() );
    }
    if ( digTwo ) {
      setRow1 ( digTwo->pixelRow() );
      setCol1 ( digTwo->pixelCol() );
    }
    if ( digThree ) {
      setRow2 ( digThree->pixelRow() );
      setCol2 ( digThree->pixelCol() );
    }
  }

  /// Destructor
  ~RichZSHitTriplet( ) { }

  /// Retrieve the full value
  inline RichDAQ::LongType data() const { return m_data; }

  /// operator to convert to RichDAQ::LongType
  inline operator RichDAQ::LongType() const { return data(); }

  /// Set the first hit row number
  inline void setRow0( const RichDAQ::ShortType row )
  {
    dataInRange(row,RichZSHitTripletCode::MaxRowCol);
    set( row, RichZSHitTripletCode::ShiftRow0, RichZSHitTripletCode::MaskRow0 );
  }

  /// Set the first hit column number
  inline void setCol0( const RichDAQ::ShortType col )
  {
    dataInRange(col,RichZSHitTripletCode::MaxRowCol);
    set( col, RichZSHitTripletCode::ShiftCol0,RichZSHitTripletCode::MaskCol0 );
  }

  /// Set the second hit row number
  inline void setRow1( const RichDAQ::ShortType row )
  {
    dataInRange(row,RichZSHitTripletCode::MaxRowCol);
    set( row, RichZSHitTripletCode::ShiftRow1,RichZSHitTripletCode::MaskRow1 );
  }

  /// Set the second hit column number
  inline void setCol1( const RichDAQ::ShortType col )
  {
    dataInRange(col,RichZSHitTripletCode::MaxRowCol);
    set( col, RichZSHitTripletCode::ShiftCol1,RichZSHitTripletCode::MaskCol1 );
  }

  /// Set the third hit row number
  inline void setRow2( const RichDAQ::ShortType row )
  {
    dataInRange(row,RichZSHitTripletCode::MaxRowCol);
    set( row, RichZSHitTripletCode::ShiftRow2, RichZSHitTripletCode::MaskRow2 );
  }

  /// Set the third hit column number
  inline void setCol2( const RichDAQ::ShortType col )
  {
    dataInRange(col,RichZSHitTripletCode::MaxRowCol);
    set( col, RichZSHitTripletCode::ShiftCol2,RichZSHitTripletCode::MaskCol2 );
  }

  /// Retrieve the first hit row number
  inline RichDAQ::ShortType row0() const
  {
    return ( (data() & RichZSHitTripletCode::MaskRow0) >> RichZSHitTripletCode::ShiftRow0 );
  }

  /// Retrieve the second hit row number
  inline RichDAQ::ShortType row1() const
  {
    return ( (data() & RichZSHitTripletCode::MaskRow1) >> RichZSHitTripletCode::ShiftRow1 );
  }

  /// Retrieve the third hit row number
  inline RichDAQ::ShortType row2() const
  {
    return ( (data() & RichZSHitTripletCode::MaskRow2) >> RichZSHitTripletCode::ShiftRow2 );
  }

  /// Retrieve the first hit column number
  inline RichDAQ::ShortType col0() const
  {
    return ( (data() & RichZSHitTripletCode::MaskCol0) >> RichZSHitTripletCode::ShiftCol0 );
  }

  /// Retrieve the second hit column number
  inline RichDAQ::ShortType col1() const
  {
    return ( (data() & RichZSHitTripletCode::MaskCol1) >> RichZSHitTripletCode::ShiftCol1 );
  }

  /// Retrieve the third hit column number
  inline RichDAQ::ShortType col2() const
  {
    return ( (data() & RichZSHitTripletCode::MaskCol2) >> RichZSHitTripletCode::ShiftCol2 );
  }

private: // methods

  /// Update the internal data
  inline void setData( const RichDAQ::LongType data ) { m_data = data;  }

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
    if ( value > max ) throw GaudiException( "Data out of range", "*RichZSHitTriplet*", StatusCode::FAILURE );
  }

private: // data

  /// The data word
  RichDAQ::LongType m_data;

};

#endif // RICHDAQ_RICHZSHITTRIPLET_H
