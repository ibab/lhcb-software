// $Id: RichDAQLinkNumber.h,v 1.2 2003-11-09 12:39:29 jonrob Exp $
#ifndef RICHDAQ_RICHDAQLINKNUMBER_H
#define RICHDAQ_RICHDAQLINKNUMBER_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// local
#include "RichDAQDefinitions.h"

/** @class RichDAQLinkNumber RichDAQLinkNumber.h
 *
 *  Representation of the Rich DAQ link identifier
 *
 *  @author Chris Jones
 *  @date   2003-11-07
 */

class RichDAQLinkNumber {

public:

  /// Standard constructor
  RichDAQLinkNumber() : m_data(0) {}

  /// Copy constructor
  RichDAQLinkNumber( const RichDAQLinkNumber & link ) : m_data( link.data() ) {}

  /// Constructor from Rich::LongType
  RichDAQLinkNumber( const Rich::LongType data ) : m_data( data ) {}

  /// Constructor from full data
  RichDAQLinkNumber( const Rich::ShortType rich,
                     const Rich::ShortType panel,
                     const Rich::ShortType pdRow,
                     const Rich::ShortType pdCol ) : m_data(0)
  {
    setRich  ( rich  );
    setPanel ( panel );
    setPDRow ( pdRow );
    setPDCol ( pdCol );
  }

  /// Destructor
  virtual ~RichDAQLinkNumber() {}

  /// Retrieve the full value
  inline Rich::LongType data() const { return m_data; }

  /// Update the internal data
  inline void setData( Rich::LongType data ) { m_data = data; }

  /// operator to convert to Rich::LongType
  inline operator Rich::LongType() const { return data(); }

  /// Return the Rich number
  inline Rich::ShortType rich() const
  {
    return ( (data() & MaskRich) >> ShiftRich );
  }

  /// Set the Rich number
  inline bool setRich( Rich::ShortType rich )
  {
    if ( !dataInRange(rich,MaxRich) ) {
      std::cout << "RichDAQLinkNumber ERROR : RICH number "
                << rich << " out of range" << std::endl;
      return false;
    }
    return set( rich, ShiftRich, MaskRich );
  }

  /// Return the panel number
  inline Rich::ShortType panel() const
  {
    return ( (data() & MaskPanel) >> ShiftPanel );
  }

  /// Set the panel number
  inline bool setPanel( Rich::ShortType panel )
  {
    if ( !dataInRange(panel,MaxPanel) ) {
      std::cout << "RichDAQLinkNumber ERROR : Panel number "
                << panel << " out of range" << std::endl;
      return false;
    }
    return set( panel, ShiftPanel, MaskPanel );
  }

  /// Return the photon detector row number
  inline Rich::ShortType pdRow() const
  {
    return ( (data() & MaskPDRow) >> ShiftPDRow );
  }

  /// Set the photon detector row number
  inline bool setPDRow( Rich::ShortType row )
  {
    if ( !dataInRange(row,MaxPDRow) ) {
      std::cout << "RichDAQLinkNumber ERROR : PD row number "
                << row << " out of range" << std::endl;
      return false;
    }
    return set( row, ShiftPDRow, MaskPDRow );
  }

  /// Return the photon detector column number
  inline Rich::ShortType pdCol() const
  {
    return ( (data() & MaskPDCol) >> ShiftPDCol );
  }

  /// Set the photon detector column number
  inline bool setPDCol( Rich::ShortType col )
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
  static const Rich::ShortType BitsRich   = 1;
  static const Rich::ShortType BitsPanel  = 1;
  static const Rich::ShortType BitsPDRow  = 5;
  static const Rich::ShortType BitsPDCol  = 5;

  // Create the shift registers
  static const Rich::ShortType ShiftRich  = 0;
  static const Rich::ShortType ShiftPanel = ShiftRich  + BitsRich;
  static const Rich::ShortType ShiftPDRow = ShiftPanel + BitsPanel;
  static const Rich::ShortType ShiftPDCol = ShiftPDRow + BitsPDRow;

  // Create the Masks
  static const Rich::LongType MaskRich    = (((Rich::LongType)1 << BitsRich)-1) << ShiftRich;
  static const Rich::LongType MaskPanel   = (((Rich::LongType)1 << BitsPanel)-1) << ShiftPanel;
  static const Rich::LongType MaskPDRow   = (((Rich::LongType)1 << BitsPDRow)-1) << ShiftPDRow;
  static const Rich::LongType MaskPDCol   = (((Rich::LongType)1 << BitsPDCol)-1) << ShiftPDCol;

  // Create the max values that can be stored in each field
  static const Rich::ShortType MaxRich    = ( (Rich::ShortType)1 << BitsRich ) - 1;
  static const Rich::ShortType MaxPanel   = ( (Rich::ShortType)1 << BitsPanel ) - 1;
  static const Rich::ShortType MaxPDRow   = ( (Rich::ShortType)1 << BitsPDRow ) - 1;
  static const Rich::ShortType MaxPDCol   = ( (Rich::ShortType)1 << BitsPDCol ) - 1;

private: // methods

  inline bool set( const Rich::ShortType value,
                   const Rich::ShortType shift,
                   const Rich::LongType  mask )
  {
    setData( ((value << shift) & mask) | (data() & ~mask) );
    return true;
  }

  // tests whether a given value is in range for a given data field
  inline bool dataInRange( const Rich::ShortType value,
                           const Rich::ShortType max ) const
  {
    return ( value <= max );
  }

private: // data

  Rich::LongType m_data;

};

#endif // RICHDAQ_RICHDAQLINKNUMBER_H
