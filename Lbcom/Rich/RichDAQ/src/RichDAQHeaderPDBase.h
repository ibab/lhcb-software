
//=============================================================================================
/** @file RichDAQHeaderPDBase.h
 *
 *  Header file for RICH DAQ utility class : RichDAQHeaderPDBase
 *
 *  CVS Log :-
 *  $Id: RichDAQHeaderPDBase.h,v 1.3 2006-02-02 17:23:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//=============================================================================================

#ifndef RICHDAQ_RICHDAQHEADERPDBASE_H
#define RICHDAQ_RICHDAQHEADERPDBASE_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// numberings
#include "RichDet/RichDAQDefinitions.h"

/** @namespace RichDAQHeaderPDBase
 *
 *  Namespace for definitions related to RichDAQHeaderPDBase
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
namespace RichDAQHeaderPDBaseCode 
{

  // Define the number of bits for each field
  static const RichDAQ::ShortType Ignored       = 30; ///< Bits reserved for the derived classes
  static const RichDAQ::ShortType BitsZS        = 1;  ///< Bits for the zero suppression flag
  static const RichDAQ::ShortType BitsStartPD   = 1;  ///< Bits for the new HPD flag

  // Create the shift registers
  static const RichDAQ::ShortType ShiftZS       = Ignored;
  static const RichDAQ::ShortType ShiftStartPD  = ShiftZS + BitsZS;

  // Create the Masks
  static const RichDAQ::LongType MaskZS         = ((1 << BitsZS)-1)       << ShiftZS;
  static const RichDAQ::LongType MaskStartPD    = ((1 << BitsStartPD)-1)  << ShiftStartPD;

}

/** @class RichDAQHeaderPDBase RichDAQHeaderPD.h
 *
 *  Base class for RICH HPD headers. Implements basic common functions
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
class RichDAQHeaderPDBase 
{

public:

  /// Constructor from RichDAQ::LongType
  RichDAQHeaderPDBase ( const RichDAQ::LongType data = 0 ) : m_data( data ) { }

  /// Retrieve the full value
  inline RichDAQ::LongType data() const { return m_data; }

  /// Update the internal data
  inline void setData( const RichDAQ::LongType data ) { m_data = data; }

  /// operator to convert to RichDAQ::LongType
  inline operator RichDAQ::LongType() const { return data(); }

  /// Set new PD bit
  inline void setStartPD( const RichDAQ::ShortType value)
  {
    set( value, RichDAQHeaderPDBaseCode::ShiftStartPD,
         RichDAQHeaderPDBaseCode::MaskStartPD );
  }

  // Is new PD bit set ?
  inline bool startPD() const
  {
    return ( 0 != ( (data() & RichDAQHeaderPDBaseCode::MaskStartPD)
                    >> RichDAQHeaderPDBaseCode::ShiftStartPD ) );
  }

  /// Set the zero suppression info
  inline void setZeroSuppressed( const bool zSupp )
  {
    RichDAQ::ShortType i = ( zSupp ? 1 : 0 );
    set( i, RichDAQHeaderPDBaseCode::ShiftZS, RichDAQHeaderPDBaseCode::MaskZS );
  }

  /// Retrieve the zero suppressed information
  inline bool zeroSuppressed() const
  {
    return ( 0 != ( (data() & RichDAQHeaderPDBaseCode::MaskZS)
                    >> RichDAQHeaderPDBaseCode::ShiftZS ) );
  }

protected: // methods

  /// Set the data value using the given mask and shift values
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

protected: // data

  RichDAQ::LongType m_data;

};

#endif // RICHDAQ_RICHDAQHEADERPDBASE_H
