// $Id: RichMirrorSegPosition.h,v 1.1 2004-07-01 11:02:52 papanest Exp $
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

// Namespace for definitions related to RichTraceMode
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
  

  virtual ~RichMirrorSegPosition( ) {} ///< Destructor

  /// Retrieve the full value
  inline RichMirrorSegPositionNames::ShortType data() const { return m_data; }

  /// Update the internal data
  inline void setData( const RichMirrorSegPositionNames::ShortType data ) 
  { m_data = data; }

  /// Set the flag for top position
  inline bool setRow( const int row ) {
    set( row, 
         RichMirrorSegPositionNames::ShiftRow, 
         RichMirrorSegPositionNames::MaskRow );
    return true;
  }

  /// Retrieve the flag for top position
  inline int row() const {
    return ( (data() & RichMirrorSegPositionNames::MaskRow) 
                    >> RichMirrorSegPositionNames::ShiftRow  );
  }

  /// Set the flag for column position
  inline bool setColumn( const int column ) {
    set( column, 
         RichMirrorSegPositionNames::ShiftColumn, 
         RichMirrorSegPositionNames::MaskColumn );
    return true;
  }

  /// Retrieve the flag for column position
  inline int column() const {
    return ( (data() & RichMirrorSegPositionNames::MaskColumn) 
                    >> RichMirrorSegPositionNames::ShiftColumn  );
  }

protected:

private:

  RichMirrorSegPositionNames::ShortType m_data;
  
  inline bool set( const RichMirrorSegPositionNames::ShortType value,
                   const RichMirrorSegPositionNames::ShortType shift,
                   const RichMirrorSegPositionNames::ShortType mask ) {
    setData( ((value << shift) & mask) | (data() & ~mask) );
    return true;
  }

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
