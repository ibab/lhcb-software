
/** @file RichDAQHPDIdentifier.h
 *
 *  Header file for RICH DAQ utility class : RichDAQHPDIdentifier
 *
 *  $Id: RichDAQHPDIdentifier.h,v 1.3 2005-11-15 12:57:47 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

#ifndef RICHDAQ_RICHDAQHPDIDENTIFIER_H
#define RICHDAQ_RICHDAQHPDIDENTIFIER_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Kernel
#include "RichKernel/RichDAQDefinitions.h"

/** @namespace RichDAQHPDIdentifierV0
 *
 *  Namespace for version 0 of the RichDAQHPDIdentifier object.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
namespace RichDAQHPDIdentifierV0 
{

  /** @namespace RichDAQHPDIdentifierCode
   *
   *  Namespace for definitions related to RichDAQHPDIdentifier
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-07
   */
  namespace RichDAQHPDIdentifierCode 
{

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

  /** @class RichDAQHPDIdentifier RichDAQHPDIdentifier.h
   *
   *  Representation of the Rich DAQ link identifier
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-07
   */

  class RichDAQHPDIdentifier {

  public:

    /// Standard constructor
    RichDAQHPDIdentifier() : m_data(0) {}

    /// Copy constructor
    RichDAQHPDIdentifier( const RichDAQHPDIdentifier & link ) : m_data( link.data() ) {}

    /// Constructor from RichDAQ::LongType
    RichDAQHPDIdentifier( const RichDAQ::LongType data ) : m_data( data ) {}

    /// Constructor from full data
    RichDAQHPDIdentifier( const RichDAQ::ShortType rich,
                          const RichDAQ::ShortType panel,
                          const RichDAQ::ShortType pdRow,
                          const RichDAQ::ShortType pdCol ) : m_data(0)
    {
      setRich  ( rich  );
      setPanel ( panel );
      setPDRow ( pdRow );
      setPDCol ( pdCol );
    }

    /// Constructor from a RichSmartID
    RichDAQHPDIdentifier( const RichSmartID smartID ) : m_data(0)
    {
      setRich  ( smartID.rich()  );
      setPanel ( smartID.panel() );
      setPDRow ( smartID.pdRow() );
      setPDCol ( smartID.pdCol() );
    }

    /// Destructor
    ~RichDAQHPDIdentifier() {}

    /// Retrieve the full value
    inline RichDAQ::LongType data() const { return m_data; }

    /// Update the internal data
    inline void setData( const RichDAQ::LongType data ) { m_data = data; }

    /// operator to convert to RichDAQ::LongType
    inline operator RichDAQ::LongType() const { return data(); }

    /// Return the Rich number
    inline RichDAQ::ShortType rich() const
    {
      return ( (data() & RichDAQHPDIdentifierCode::MaskRich)
               >> RichDAQHPDIdentifierCode::ShiftRich );
    }

    /// Set the Rich number
    inline bool setRich( const RichDAQ::ShortType rich )
    {
      return ( dataInRange(rich,RichDAQHPDIdentifierCode::MaxRich) ?
               set( rich, RichDAQHPDIdentifierCode::ShiftRich,
                    RichDAQHPDIdentifierCode::MaskRich ) : false );
    }

    /// Return the panel number
    inline RichDAQ::ShortType panel() const
    {
      return ( (data() & RichDAQHPDIdentifierCode::MaskPanel)
               >> RichDAQHPDIdentifierCode::ShiftPanel );
    }

    /// Set the panel number
    inline bool setPanel( const RichDAQ::ShortType panel )
    {
      return ( dataInRange(panel,RichDAQHPDIdentifierCode::MaxPanel) ?
               set( panel, RichDAQHPDIdentifierCode::ShiftPanel,
                    RichDAQHPDIdentifierCode::MaskPanel ) : false );
    }

    /// Return the photon detector row number
    inline RichDAQ::ShortType pdRow() const
    {
      return ( (data() & RichDAQHPDIdentifierCode::MaskPDRow)
               >> RichDAQHPDIdentifierCode::ShiftPDRow );
    }

    /// Set the photon detector row number
    inline bool setPDRow( const RichDAQ::ShortType row )
    {
      return ( dataInRange(row,RichDAQHPDIdentifierCode::MaxPDRow) ?
               set( row, RichDAQHPDIdentifierCode::ShiftPDRow,
                    RichDAQHPDIdentifierCode::MaskPDRow ) : false );
    }

    /// Return the photon detector column number
    inline RichDAQ::ShortType pdCol() const
    {
      return ( (data() & RichDAQHPDIdentifierCode::MaskPDCol)
               >> RichDAQHPDIdentifierCode::ShiftPDCol );
    }

    /// Set the photon detector column number
    inline bool setPDCol( const RichDAQ::ShortType col )
    {
      return ( dataInRange(col,RichDAQHPDIdentifierCode::MaxPDCol) ?
               set( col, RichDAQHPDIdentifierCode::ShiftPDCol,
                    RichDAQHPDIdentifierCode::MaskPDCol ) : false );
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

} // RichDAQHPDIdentifierV0 namespace

/// overloaded output to MsgStream
inline MsgStream & operator << ( MsgStream & os,
                                 const RichDAQHPDIdentifierV0::RichDAQHPDIdentifier & hpdid )
{
  os << "RICH = " << hpdid.rich()
     << " panel = " << hpdid.panel()
     << " HPD Row = " << hpdid.pdRow()
     << " HPD Col = " << hpdid.pdCol();
  return os;
}

#endif // RICHDAQ_RICHDAQHPDIDENTIFIER_H
