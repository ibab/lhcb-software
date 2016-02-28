
/** @file RichMirrorSegPosition.h
 *
 *  Header file for utility class : Rich::MirrorSegPosition
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-30
 */

#ifndef RICHDET_RICHMIRRORSEGPOSITION_H
#define RICHDET_RICHMIRRORSEGPOSITION_H 1

// std include
#include <iostream>

namespace Rich
{

  /** @class MirrorSegPosition RichDet/RichMirrorSegPosition.h
   *
   *  Helper class to pack together row/column information for the mirror
   *  segment position.
   *
   *  @author Antonis Papanestis a.papanestis@rl.ac.uk
   *  @date   2004-06-30
   */
  class MirrorSegPosition
  {

  public: // definitions

    /// Type for dataword
    typedef unsigned int ShortType;

    static constexpr ShortType BitsRow    = 3; ///< number of bits for the row
    static constexpr ShortType BitsColumn = 3; ///< number of bits for the column

    // Shifts
    static constexpr ShortType ShiftRow    = 0;
    static constexpr ShortType ShiftColumn = BitsColumn + ShiftRow;

    // Create the Masks
    static constexpr ShortType MaskRow    = ((1 << BitsRow)-1) << ShiftRow;
    static constexpr ShortType MaskColumn = ((1 << BitsColumn)-1) << ShiftColumn;

  public:

    /** Access to the full bit-packed data word
     *
     *  @return the bit-packed data word
     */
    inline ShortType data() const noexcept
    {
      return m_data;
    }

    /** Set the full bit-packed data word
     */
    inline void setData( const ShortType data ) noexcept
    {
      m_data = data;
    }

    /** Set the row number
     */
    inline void setRow( const int row ) noexcept
    {
      set( row, ShiftRow, MaskRow );
    }

    /** Retrieve the row number
     */
    inline int row() const noexcept
    {
      return ( (data() & MaskRow) >> ShiftRow );
    }

    /** Set the column number
     */
    inline void setColumn( const int column ) noexcept
    {
      set( column, ShiftColumn, MaskColumn );
    }

    /** Retrieve the column number
     */
    inline int column() const noexcept
    {
      return ( (data() & MaskColumn) >> ShiftColumn  );
    }

  private: // methods

    inline void set( const ShortType value,
                     const ShortType shift,
                     const ShortType mask ) noexcept
    {
      setData( ((value << shift) & mask) | (data() & ~mask) );
    }

  private: // data

    /// The bit-pack data word
    ShortType m_data{0};

  };

}

/// overloaded output to MsgStream
inline std::ostream& operator << ( std::ostream& os,
                                   const Rich::MirrorSegPosition& pos )
{
  return os << "Mirror Segment row:" << pos.row() << " column:" << pos.column();
}

#endif // RICHDET_RICHMIRRORSEGPOSITION_H
