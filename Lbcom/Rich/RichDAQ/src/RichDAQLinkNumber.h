
/** @file RichDAQLinkNumber.h
 *
 *  Header file for RICH DAQ utility class : RichDAQLinkNumber
 *
 *  CVS Log :-
 *  $Id: RichDAQLinkNumber.h,v 1.7 2004-07-27 13:46:06 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

#ifndef RICHDAQ_RICHDAQLINKNUMBER_H
#define RICHDAQ_RICHDAQLINKNUMBER_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// local
#include "RichDAQDefinitions.h"

/** @namespace RichZSHitTripletCode
 *
 *  Namespace for definitions related to RichDAQLinkNumber
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-07
 */
namespace RichDAQLinkNumberCode {

  // Define the number of bits for each field
  static const RichDAQ::ShortType  BitsRich  =  1; ///< Number of bits for RICH field
  static const RichDAQ::ShortType  BitsPanel =  1; ///< Number of bits for RICH panel field
  static const RichDAQ::ShortType  BitsPDRow =  5; ///< Number of bits for HPD row field
  static const RichDAQ::ShortType  BitsPDCol =  5; ///< Number of bits for HPD column field

  // Create the shift registers
  static const RichDAQ::ShortType  ShiftRich   = 0;
  static const RichDAQ::ShortType  ShiftPanel  = ShiftRich  + BitsRich;
  static const RichDAQ::ShortType  ShiftPDRow  = ShiftPanel + BitsPanel;
  static const RichDAQ::ShortType  ShiftPDCol  = ShiftPDRow + BitsPDRow;

  // Create the Masks
  static const RichDAQ::LongType  MaskRich  =   ((1 << BitsRich)-1) << ShiftRich;
  static const RichDAQ::LongType  MaskPanel =   ((1 << BitsPanel)-1) << ShiftPanel;
  static const RichDAQ::LongType  MaskPDRow =   ((1 << BitsPDRow)-1) << ShiftPDRow;
  static const RichDAQ::LongType  MaskPDCol =   ((1 << BitsPDCol)-1) << ShiftPDCol;

  // Create the max values that can be stored in each field
  static const RichDAQ::ShortType  MaxRich  =   ( 1 << BitsRich  ) - 1;
  static const RichDAQ::ShortType  MaxPanel =   ( 1 << BitsPanel ) - 1;
  static const RichDAQ::ShortType  MaxPDRow =   ( 1 << BitsPDRow ) - 1;
  static const RichDAQ::ShortType  MaxPDCol =   ( 1 << BitsPDCol ) - 1;

}

/** @class RichDAQLinkNumber RichDAQLinkNumber.h
 *
 *  Representation of the Rich DAQ link identifier
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-07
 */

class RichDAQLinkNumber {

public:

  /// Standard constructor
  RichDAQLinkNumber() : m_data(0) {}

  /// Copy constructor
  RichDAQLinkNumber( const RichDAQLinkNumber & link ) : m_data( link.data() ) {}

  /// Constructor from RichDAQ::LongType
  RichDAQLinkNumber( const RichDAQ::LongType data ) : m_data( data ) {}

  /// Constructor from full data
  RichDAQLinkNumber( const RichDAQ::ShortType rich,
                     const RichDAQ::ShortType panel,
                     const RichDAQ::ShortType pdRow,
                     const RichDAQ::ShortType pdCol ) : m_data(0)
  {
    setRich  ( rich  );
    setPanel ( panel );
    setPDRow ( pdRow );
    setPDCol ( pdCol );
  }

  /// Destructor
  ~RichDAQLinkNumber() {}

  /// Retrieve the full value
  inline RichDAQ::LongType data() const { return m_data; }

  /// Update the internal data
  inline void setData( const RichDAQ::LongType data ) { m_data = data; }

  /// operator to convert to RichDAQ::LongType
  inline operator RichDAQ::LongType() const { return data(); }

  /// Return the Rich number
  inline RichDAQ::ShortType rich() const
  {
    return ( (data() & RichDAQLinkNumberCode::MaskRich)
             >> RichDAQLinkNumberCode::ShiftRich );
  }

  /// Set the Rich number
  inline bool setRich( const RichDAQ::ShortType rich )
  {
    return ( dataInRange(rich,RichDAQLinkNumberCode::MaxRich) ?
             set( rich, RichDAQLinkNumberCode::ShiftRich,
                  RichDAQLinkNumberCode::MaskRich ) : false );
  }

  /// Return the panel number
  inline RichDAQ::ShortType panel() const
  {
    return ( (data() & RichDAQLinkNumberCode::MaskPanel)
             >> RichDAQLinkNumberCode::ShiftPanel );
  }

  /// Set the panel number
  inline bool setPanel( const RichDAQ::ShortType panel )
  {
    return ( dataInRange(panel,RichDAQLinkNumberCode::MaxPanel) ?
             set( panel, RichDAQLinkNumberCode::ShiftPanel,
                  RichDAQLinkNumberCode::MaskPanel ) : false );
  }

  /// Return the photon detector row number
  inline RichDAQ::ShortType pdRow() const
  {
    return ( (data() & RichDAQLinkNumberCode::MaskPDRow)
             >> RichDAQLinkNumberCode::ShiftPDRow );
  }

  /// Set the photon detector row number
  inline bool setPDRow( const RichDAQ::ShortType row )
  {
    return ( dataInRange(row,RichDAQLinkNumberCode::MaxPDRow) ?
             set( row, RichDAQLinkNumberCode::ShiftPDRow,
                  RichDAQLinkNumberCode::MaskPDRow ) : false );
  }

  /// Return the photon detector column number
  inline RichDAQ::ShortType pdCol() const
  {
    return ( (data() & RichDAQLinkNumberCode::MaskPDCol)
             >> RichDAQLinkNumberCode::ShiftPDCol );
  }

  /// Set the photon detector column number
  inline bool setPDCol( const RichDAQ::ShortType col )
  {
    return ( dataInRange(col,RichDAQLinkNumberCode::MaxPDCol) ?
             set( col, RichDAQLinkNumberCode::ShiftPDCol,
                  RichDAQLinkNumberCode::MaskPDCol ) : false );
  }

private: // methods

  /// Set the data value for a given mask and shift value
  inline bool set( const RichDAQ::ShortType value,
                   const RichDAQ::ShortType shift,
                   const RichDAQ::LongType  mask )
  {
    setData( ((value << shift) & mask) | (data() & ~mask) );
    return true;
  }

  /// tests whether a given value is in range for a given data field
  inline bool dataInRange( const RichDAQ::ShortType value,
                           const RichDAQ::ShortType max ) const
  {
    return ( value <= max );
  }

private: // data

  /// The data word
  RichDAQ::LongType m_data;

};

#endif // RICHDAQ_RICHDAQLINKNUMBER_H
