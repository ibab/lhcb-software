// $Id: RichDAQLinkNumber.h,v 1.1.1.1 2003-11-08 14:26:20 jonesc Exp $
#ifndef RICHDAQLINKNUMBER_H
#define RICHDAQLINKNUMBER_H 1

// Include files

/** @class RichDAQLinkNumber RichDAQLinkNumber.h
 *
 *
 *  @author Chris Jones
 *  @date   2003-11-07
 */

class RichDAQLinkNumber {

public: // definitions

  typedef hlt_int        LongType;
  typedef unsigned int   ShortType;

public:

  /// Standard constructor
  RichDAQLinkNumber() : m_data(0) {}

  /// Copy constructor
  RichDAQLinkNumber( const RichDAQLinkNumber & link ) : m_data( link.data() ) {}

  /// Constructor from LongType
  RichDAQLinkNumber( const LongType data ) : m_data( data ) {}

  /// Constructor from full data
  RichDAQLinkNumber( const ShortType rich,
                     const ShortType panel,
                     const ShortType pdRow,
                     const ShortType pdCol ) : m_data(0)
  {
    setRich  ( rich  );
    setPanel ( panel );
    setPDRow ( pdRow );
    setPDCol ( pdCol );
  }

  /// Destructor
  virtual ~RichDAQLinkNumber() {}

  /// Retrieve the full value
  inline LongType data() const { return m_data; }

  /// Update the internal data
  inline void setData( LongType data ) { m_data = data; }

  /// operator to convert to LongType
  inline operator LongType() const { return data(); }

  /// Return the Rich number
  inline ShortType rich() const
  {
    return ( (data() & MaskRich) >> ShiftRich );
  }

  /// Set the Rich number
  inline bool setRich( ShortType rich )
  {
    if ( !dataInRange(rich,MaxRich) ) {
      std::cout << "RichDAQLinkNumber ERROR : RICH number "
                << rich << " out of range" << std::endl;
      return false;
    }
    return set( rich, ShiftRich, MaskRich );
  }

  /// Return the panel number
  inline ShortType panel() const
  {
    return ( (data() & MaskPanel) >> ShiftPanel );
  }

  /// Set the panel number
  inline bool setPanel( ShortType panel )
  {
    if ( !dataInRange(panel,MaxPanel) ) {
      std::cout << "RichDAQLinkNumber ERROR : Panel number "
                << panel << " out of range" << std::endl;
      return false;
    }
    return set( panel, ShiftPanel, MaskPanel );
  }

  /// Return the photon detector row number
  inline ShortType pdRow() const
  {
    return ( (data() & MaskPDRow) >> ShiftPDRow );
  }

  /// Set the photon detector row number
  inline bool setPDRow( ShortType row )
  {
    if ( !dataInRange(row,MaxPDRow) ) {
      std::cout << "RichDAQLinkNumber ERROR : PD row number "
                << row << " out of range" << std::endl;
      return false;
    }
    return set( row, ShiftPDRow, MaskPDRow );
  }

  /// Return the photon detector column number
  inline ShortType pdCol() const
  {
    return ( (data() & MaskPDCol) >> ShiftPDCol );
  }

  /// Set the photon detector column number
  inline bool setPDCol( ShortType col )
  {
    if ( !dataInRange(col,MaxPDCol) ) {
      std::cout << "RichDAQLinkNumber ERROR : PD column number "
                << col << " out of range" << std::endl;
      return false;
    }
    return set( col, ShiftPDCol, MaskPDCol );
  }

private: // definitions

  // Define the number of bits for each field
  static const ShortType BitsRich   = 1;
  static const ShortType BitsPanel  = 1;
  static const ShortType BitsPDRow  = 5;
  static const ShortType BitsPDCol  = 5;

  // Create the shift registers
  static const ShortType ShiftRich  = 0;
  static const ShortType ShiftPanel = ShiftRich  + BitsRich;
  static const ShortType ShiftPDRow = ShiftPanel + BitsPanel;
  static const ShortType ShiftPDCol = ShiftPDRow + BitsPDRow;

  // Create the Masks
  static const LongType MaskRich    = (((LongType)1 << BitsRich)-1) << ShiftRich;
  static const LongType MaskPanel   = (((LongType)1 << BitsPanel)-1) << ShiftPanel;
  static const LongType MaskPDRow   = (((LongType)1 << BitsPDRow)-1) << ShiftPDRow;
  static const LongType MaskPDCol   = (((LongType)1 << BitsPDCol)-1) << ShiftPDCol;

  // Create the max values that can be stored in each field
  static const ShortType MaxRich    = ( (ShortType)1 << BitsRich ) - 1;
  static const ShortType MaxPanel   = ( (ShortType)1 << BitsPanel ) - 1;
  static const ShortType MaxPDRow   = ( (ShortType)1 << BitsPDRow ) - 1;
  static const ShortType MaxPDCol   = ( (ShortType)1 << BitsPDCol ) - 1;

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
#endif // RICHDAQLINKNUMBER_H
