
/** @file RichMirrorSegPosition.h
 *
 *  Header file for utility class : Rich::MirrorSegPosition
 *
 *  CVS Log :-
 *  $Id: RichMirrorSegPosition.h,v 1.5 2007-02-01 16:41:13 jonrob Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-30
 */

#ifndef RICHDET_RICHMIRRORSEGPOSITION_H
#define RICHDET_RICHMIRRORSEGPOSITION_H 1

// std include
#include <iostream>

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  /** @namespace RichMirrorSegPositionNames
   *
   * Namespace for bit-packing parameters used by the utility class
   * RichMirrorSegPosition
   *
   * @author Antonis Papanestis a.papanestis@rl.ac.uk
   * @date   2004-06-30
   */
  namespace RichMirrorSegPositionNames
  {
    // Type for dataword
    typedef unsigned int ShortType;

    static const ShortType BitsRow    = 3; ///< number of bits for the row
    static const ShortType BitsColumn = 3; ///< number of bits for the column

    // Shifts
    static const ShortType ShiftRow    = 0;
    static const ShortType ShiftColumn = BitsColumn + ShiftRow;

    // Create the Masks
    static const ShortType MaskRow    = ((1 << BitsRow)-1) << ShiftRow;
    static const ShortType MaskColumn = ((1 << BitsColumn)-1) << ShiftColumn;
  }

  /** @class MirrorSegPosition RichMirrorSegPosition.h RichDet/RichMirrorSegPosition.h
   *
   *  Helper class to pack together row/column information for the mirror
   *   segment position.
   *  @author Antonis Papanestis a.papanestis@rl.ac.uk
   *  @date   2004-06-30
   */
  class MirrorSegPosition
  {

  public:

    /// Standard constructor
    MirrorSegPosition( ) : m_data(0) {}

    ~MirrorSegPosition( ) {} ///< Destructor

    /** Access to the full bit-packed data word
     *
     *  @return the bit-packed data word
     */
    inline RichMirrorSegPositionNames::ShortType data() const
    {
      return m_data;
    }

    /** Set the full bit-packed data word
     */
    inline void setData( const RichMirrorSegPositionNames::ShortType data )
    {
      m_data = data;
    }

    /** Set the row number
     */
    inline void setRow( const int row )
    {
      set( row,
           RichMirrorSegPositionNames::ShiftRow,
           RichMirrorSegPositionNames::MaskRow );
    }

    /** Retrieve the row number
     */
    inline int row() const
    {
      return ( (data() & RichMirrorSegPositionNames::MaskRow)
               >> RichMirrorSegPositionNames::ShiftRow  );
    }

    /** Set the column number
     */
    inline void setColumn( const int column )
    {
      set( column,
           RichMirrorSegPositionNames::ShiftColumn,
           RichMirrorSegPositionNames::MaskColumn );
    }

    /** Retrieve the column number
     */
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

    /// The bit-pack data word
    RichMirrorSegPositionNames::ShortType m_data;

  };

}

/// overloaded output to MsgStream
inline std::ostream& operator << ( std::ostream& os,
                                   const Rich::MirrorSegPosition& pos )
{
  return os << "Mirror Segment row:" << pos.row() << " column:" << pos.column();
}

/** backwards compatibility
 *  @todo remove this typedef */
typedef Rich::MirrorSegPosition RichMirrorSegPosition;

#endif // RICHDET_RICHMIRRORSEGPOSITION_H
