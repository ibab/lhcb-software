// $Id: RichZSHitTriplet.h,v 1.1.1.1 2003-11-08 14:26:20 jonesc Exp $
#ifndef RICHDAQ_RICHZSHITTRIPLET_H
#define RICHDAQ_RICHZSHITTRIPLET_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Event model
#include "Event/DAQTypes.h"
#include "Event/MCRichDigit.h"

/** @class RichZSHitTriplet RichZSHitTriplet.h
 *
 *  Utility class representing three hits packed into a single word
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
class RichZSHitTriplet {

public: // definitions

  typedef hlt_int        LongType;
  typedef unsigned int   ShortType;

public: // methods

  /// Standard constructor
  RichZSHitTriplet() : m_data(0) {}

  /// Copy Constructor
  RichZSHitTriplet( const RichZSHitTriplet & hits ) : m_data( hits.data() ) {}

  /// Constructor from LongType
  RichZSHitTriplet( const LongType data ) : m_data( data ) { }

  /// Constructor from three hits
  RichZSHitTriplet( const MCRichDigit * digOne,
                    const MCRichDigit * digTwo,
                    const MCRichDigit * digThree ) : m_data(0)
  {
    if ( digOne ) {
      setRow0 ( digOne->key().pixelRow() );
      setCol0 ( digOne->key().pixelCol() );
    }
    if ( digTwo ) {
      setRow1 ( digTwo->key().pixelRow() );
      setCol1 ( digTwo->key().pixelCol() );
    }
    if ( digThree ) {
      setRow2 ( digThree->key().pixelRow() );
      setCol2 ( digThree->key().pixelCol() );
    }
  }

  /// Destructor
  virtual ~RichZSHitTriplet( ) { }

  /// Retrieve the full value
  inline LongType data() const { return m_data; }

  /// Update the internal data
  inline void setData( LongType data ) { m_data = data;  }

  /// operator to convert to LongType
  inline operator LongType() const { return data(); }

  /// Set the first hit row number
  inline bool setRow0( ShortType row )
  {
    if ( !dataInRange(row,MaxRowCol) ) {
      std::cout << "RichZSHitTriplet ERROR : Row0 number "
                << row << " out of range" << std::endl;
      return false;
    }
    return set( row, ShiftRow0, MaskRow0 );
  }

  /// Set the first hit column number
  inline bool setCol0( ShortType col )
  {
    if ( !dataInRange(col,MaxRowCol) ) {
      std::cout << "RichZSHitTriplet ERROR : Col0 number "
                << col << " out of range" << std::endl;
      return false;
    }
    return set( col, ShiftCol0, MaskCol0 );
  }

  /// Set the second hit row number
  inline bool setRow1( ShortType row )
  {
    if ( !dataInRange(row,MaxRowCol) ) {
      std::cout << "RichZSHitTriplet ERROR : Row1 number "
                << row << " out of range" << std::endl;
      return false;
    }
    return set( row, ShiftRow1, MaskRow1 );
  }

  /// Set the second hit column number
  inline bool setCol1( ShortType col )
  {
    if ( !dataInRange(col,MaxRowCol) ) {
      std::cout << "RichZSHitTriplet ERROR : Col1 number "
                << col << " out of range" << std::endl;
      return false;
    }
    return set( col, ShiftCol1, MaskCol1 );
  }

  /// Set the third hit row number
  inline bool setRow2( ShortType row )
  {
    if ( !dataInRange(row,MaxRowCol) ) {
      std::cout << "RichZSHitTriplet ERROR : Row2 number "
                << row << " out of range" << std::endl;
      return false;
    }
    return set( row, ShiftRow2, MaskRow2 );
  }

  /// Set the third hit column number
  inline bool setCol2( ShortType col )
  {
    if ( !dataInRange(col,MaxRowCol) ) {
      std::cout << "RichZSHitTriplet ERROR : Col2 number "
                << col << " out of range" << std::endl;
      return false;
    }
    return set( col, ShiftCol2, MaskCol2 );
  }

  /// Retrieve the first hit row number
  inline ShortType row0() const
  {
    return ( (data() & MaskRow0) >> ShiftRow0 );
  }

  /// Retrieve the second hit row number
  inline ShortType row1() const
  {
    return ( (data() & MaskRow1) >> ShiftRow1 );
  }

  /// Retrieve the third hit row number
  inline ShortType row2() const
  {
    return ( (data() & MaskRow2) >> ShiftRow2 );
  }

  /// Retrieve the first hit column number
  inline ShortType col0() const
  {
    return ( (data() & MaskCol0) >> ShiftCol0 );
  }

  /// Retrieve the second hit column number
  inline ShortType col1() const
  {
    return ( (data() & MaskCol1) >> ShiftCol1 );
  }

  /// Retrieve the third hit column number
  inline ShortType col2() const
  {
    return ( (data() & MaskCol2) >> ShiftCol2 );
  }

private: // definitions

  // Define the number of bits for each field
  static const ShortType BitsHit   = 5;

  // shift registers
  static const ShortType ShiftRow0 = 0;
  static const ShortType ShiftCol0 = ShiftRow0 + BitsHit;
  static const ShortType ShiftRow1 = ShiftCol0 + BitsHit;
  static const ShortType ShiftCol1 = ShiftRow1 + BitsHit;
  static const ShortType ShiftRow2 = ShiftCol1 + BitsHit;
  static const ShortType ShiftCol2 = ShiftRow2 + BitsHit;

  // The masks
  static const LongType  MaskRow0  =  (((LongType)1 << BitsHit)-1) << ShiftRow0;
  static const LongType  MaskCol0  =  (((LongType)1 << BitsHit)-1) << ShiftCol0;
  static const LongType  MaskRow1  =  (((LongType)1 << BitsHit)-1) << ShiftRow1;
  static const LongType  MaskCol1  =  (((LongType)1 << BitsHit)-1) << ShiftCol1;
  static const LongType  MaskRow2  =  (((LongType)1 << BitsHit)-1) << ShiftRow2;
  static const LongType  MaskCol2  =  (((LongType)1 << BitsHit)-1) << ShiftCol2;

  // the max row/column values storable
  static const ShortType MaxRowCol = ( (ShortType)1 << BitsHit ) - 1;

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
                                 const RichZSHitTriplet & triplet )
{
  os << "pixels (r/c) : " 
     << triplet.row0() << "/" << triplet.col0() << " "
     << triplet.row1() << "/" << triplet.col1() << " "
     << triplet.row2() << "/" << triplet.col2() << " ";
  return os;
}

#endif // RICHDAQ_RICHZSHITTRIPLET_H
