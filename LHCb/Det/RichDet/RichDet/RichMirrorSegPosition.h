// $Id: RichMirrorSegPosition.h,v 1.2 2004-07-09 17:36:30 jonrob Exp $
#ifndef RICHDET_RICHMIRRORSEGPOSITION_H 
#define RICHDET_RICHMIRRORSEGPOSITION_H 1

// Include files
#include "GaudiKernel/MsgStream.h"

/** @class RichMirrorSegPosition RichMirrorSegPosition.h RichDet/RichMirrorSegPosition.h
 *  
 *
 *  @author Antonis Papanestis
 *  @date   2004-06-30
 */

// Namespace for definitions related to RichMirrorSegPosition
namespace RichMirrorSegPositionNames
{
  // Type for dataword
  typedef unsigned int ShortType;
  
  static const ShortType BitsRow    = 3; ///< number of bits for this flag
  static const ShortType BitsColumn = 3; ///< number of bits for this flag

  // Shifts
  static const ShortType ShiftRow    = 0;
  static const ShortType ShiftColumn = BitsColumn + ShiftRow;

  // Create the Masks
  static const ShortType MaskRow    = ((1 << BitsRow)-1) << ShiftRow;
  static const ShortType MaskColumn = ((1 << BitsColumn)-1) << ShiftColumn;
}

class RichMirrorSegPosition {

public: 

  /// Standard constructor
  RichMirrorSegPosition( ): m_data(0) {}
  
  ~RichMirrorSegPosition( ) {} ///< Destructor

  /// Retrieve the full value
  inline RichMirrorSegPositionNames::ShortType data() const 
  { 
    return m_data; 
  }

  /// Update the internal data
  inline void setData( const RichMirrorSegPositionNames::ShortType data ) 
  { 
    m_data = data; 
  }

  /// Set the flag for top position
  inline void setRow( const int row ) 
  {
    set( row, 
         RichMirrorSegPositionNames::ShiftRow, 
         RichMirrorSegPositionNames::MaskRow );
  }
  
  /// Retrieve the flag for top position
  inline int row() const 
  {
    return ( (data() & RichMirrorSegPositionNames::MaskRow) 
             >> RichMirrorSegPositionNames::ShiftRow  );
  }

  /// Set the flag for column position
  inline void setColumn( const int column ) 
  {
    set( column, 
         RichMirrorSegPositionNames::ShiftColumn, 
         RichMirrorSegPositionNames::MaskColumn );
  }

  /// Retrieve the flag for column position
  inline int column() const 
  {
    return ( (data() & RichMirrorSegPositionNames::MaskColumn) 
             >> RichMirrorSegPositionNames::ShiftColumn  );
  }

private: // methods
  
  inline void set( const RichMirrorSegPositionNames::ShortType value,
                   const RichMirrorSegPositionNames::ShortType shift,
                   const RichMirrorSegPositionNames::ShortType mask ) 
  {
    setData( ((value << shift) & mask) | (data() & ~mask) );
  }

private: // data

  RichMirrorSegPositionNames::ShortType m_data;

};

/// overloaded output to MsgStream
inline MsgStream& operator << ( MsgStream& os,
                                const RichMirrorSegPosition& pos )
{
  os << "Mirror Segment row:" << pos.row()
     << " column:" << pos.column();
  return os;
    
}

#endif // RICHDET_RICHMIRRORSEGPOSITION_H
