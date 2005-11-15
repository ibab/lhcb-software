
/** @file RichDAQHeaderPD.h
 *
 *  Header file for RICH DAQ utility class : RichDAQHeaderPD
 *
 *  $Id: RichDAQHeaderPD.h,v 1.14 2005-11-15 12:57:47 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

#ifndef RICHDAQ_RICHDAQHEADERPD_H
#define RICHDAQ_RICHDAQHEADERPD_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Kernel
#include "RichKernel/RichDAQDefinitions.h"

// local
#include "RichDAQHPDIdentifier.h"

/** @namespace RichDAQHeaderV0
 *
 *  Namespace for version 0 of the RichDAQHeaderPD object.
 *  DC04 compatible version (Now obsolete).
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
namespace RichDAQHeaderV0 {

  /** @namespace RichDAQHeaderPDCode
   *
   *  Namespace for definitions related to RichDAQHeaderV0::RichDAQHeaderPD
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-06
   */
  namespace RichDAQHeaderPDCode {

    // Define the number of bits for each field
    static const RichDAQ::ShortType BitsHitCount  =  10; ///< Number of bits for number of hits
    static const RichDAQ::ShortType BitsHPDID     =  12; ///< Number of bits for HPD identifier
    static const RichDAQ::ShortType BitsUnUsed1   =  7;  ///< Unused bits
    static const RichDAQ::ShortType BitsZS        =  1;  ///< Bits for the zero suppression flag
    static const RichDAQ::ShortType BitsStartPD   =  1;  ///< New HPD flag

    // Create the shift registers
    static const RichDAQ::ShortType ShiftHitCount = 0;
    static const RichDAQ::ShortType ShiftHPDID    = ShiftHitCount + BitsHitCount;
    static const RichDAQ::ShortType ShiftZS       = ShiftHPDID  + BitsUnUsed1 + BitsHPDID;
    static const RichDAQ::ShortType ShiftStartPD  = ShiftZS       + BitsZS;

    // Create the Masks
    static const RichDAQ::LongType MaskHitCount  = ((1 << BitsHitCount)-1) << ShiftHitCount;
    static const RichDAQ::LongType MaskHPDID     = ((1 << BitsHPDID)-1)  << ShiftHPDID;
    static const RichDAQ::LongType MaskZS        = ((1 << BitsZS)-1)       << ShiftZS;
    static const RichDAQ::LongType MaskStartPD   = ((1 << BitsStartPD)-1)  << ShiftStartPD;

    // Create the max values that can be stored in each field
    static const RichDAQ::ShortType MaxHitCount  = ( 1 << BitsHitCount ) - 1; ///< Maximum number of hits
    static const RichDAQ::ShortType MaxHPDID     = ( 1 << BitsHPDID  ) - 1;   ///< Maximum HPD ID

  }

  /** @class RichDAQHeaderPD RichDAQHeaderPD.h
   *
   *  Utility class representing the header word for HPD data.
   *
   *  First iteration, compatible with DC04 data
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-06
   */
  class RichDAQHeaderPD {

  public: // methods

    /// Copy constructor
    RichDAQHeaderPD ( const RichDAQHeaderPD & header ) : m_data( header.data() ) { }

    /// Constructor from RichDAQ::LongType
    RichDAQHeaderPD ( const RichDAQ::LongType data = 0 ) : m_data( data ) { }

    /// Constructor from all data
    RichDAQHeaderPD ( const bool zSupp,                    ///< Flag indicating if the block is zero suppressed
                      const RichDAQHPDIdentifierV0::RichDAQHPDIdentifier hpdID,    ///< The HPD identifier
                      const RichDAQ::ShortType hitCount,   ///< The number of hits in this block
                      const RichDAQ::ShortType startPD = 1 ///< New HPD flag
                      )
      : m_data ( 0 )
    {
      setStartPD        ( startPD  );
      setZeroSuppressed ( zSupp    );
      setHPDID          ( hpdID    );
      setHitCount       ( hitCount );
    }

    /// Destructor
    virtual ~RichDAQHeaderPD( ) {}

    /// Retrieve the full value
    inline RichDAQ::LongType data() const { return m_data; }

    /// Update the internal data
    inline void setData( const RichDAQ::LongType data ) { m_data = data; }

    /// operator to convert to RichDAQ::LongType
    inline operator RichDAQ::LongType() const { return data(); }

    /// Set new PD bit
    inline void setStartPD( const RichDAQ::ShortType value)
    {
      set( value, RichDAQHeaderPDCode::ShiftStartPD,
           RichDAQHeaderPDCode::MaskStartPD );
    }

    /// Is new PD bit set ?
    inline bool startPD() const
    {
      return ( 0 != ( (data() & RichDAQHeaderPDCode::MaskStartPD)
                      >> RichDAQHeaderPDCode::ShiftStartPD ) );
    }

    /// Set the zero suppression info
    inline void setZeroSuppressed( const bool zSupp )
    {
      RichDAQ::ShortType i = ( zSupp ? 1 : 0 );
      set( i, RichDAQHeaderPDCode::ShiftZS, RichDAQHeaderPDCode::MaskZS );
    }

    /// Retrieve the zero suppressed information
    inline bool zeroSuppressed() const
    {
      return ( 0 != ( (data() & RichDAQHeaderPDCode::MaskZS)
                      >> RichDAQHeaderPDCode::ShiftZS ) );
    }

    /// Set the HPD ID
    inline bool setHPDID( const RichDAQ::ShortType hpdid )
    {
      return ( dataInRange(hpdid,RichDAQHeaderPDCode::MaxHPDID) ?
               set( hpdid, RichDAQHeaderPDCode::ShiftHPDID,
                    RichDAQHeaderPDCode::MaskHPDID ) : false );
    }

    /// Retrieve the HPD ID
    inline RichDAQHPDIdentifierV0::RichDAQHPDIdentifier hpdID() const
    {
      return RichDAQHPDIdentifierV0::RichDAQHPDIdentifier
        ( (data() & RichDAQHeaderPDCode::MaskHPDID) >> RichDAQHeaderPDCode::ShiftHPDID );
    }

    /// Set the hit count info
    inline bool setHitCount( const RichDAQ::ShortType hitCount )
    {
      return ( dataInRange(hitCount,RichDAQHeaderPDCode::MaxHitCount) ?
               set( hitCount, RichDAQHeaderPDCode::ShiftHitCount,
                    RichDAQHeaderPDCode::MaskHitCount ) : false );
    }

    /// Retrieve the hit count number
    inline RichDAQ::ShortType hitCount() const
    {
      return ( (data() & RichDAQHeaderPDCode::MaskHitCount)
               >> RichDAQHeaderPDCode::ShiftHitCount );
    }

  private: // methods

    /// Set the data value using the given mask and shift values
    inline bool set( const RichDAQ::ShortType value,
                     const RichDAQ::ShortType shift,
                     const RichDAQ::LongType  mask )
    {
      setData( ((value << shift) & mask) | (data() & ~mask) );
      return true;
    }

    /// Tests whether a given value is in range for a given data field
    inline bool dataInRange( const RichDAQ::ShortType value,
                             const RichDAQ::ShortType max ) const
    {
      return ( value <= max );
    }

  private: // data

    RichDAQ::LongType m_data;

  };

} // RichDAQHeaderV0 namespace

/// overloaded output to std::ostream
inline std::ostream & operator << ( std::ostream & os,
                                    const RichDAQHeaderV0::RichDAQHeaderPD & header )
{
  os << " RichDAQHeaderPD V0 : HPD = " << header.hpdID()
     << " ZS = " << header.zeroSuppressed()
     << " HitCount = " << header.hitCount();
  return os;
}

//=============================================================================================

// Base class
#include "RichDAQHeaderPDBase.h"

/** @namespace RichDAQHeaderV1
 *
 *  Namespace for version 1 of the RichDAQHeaderPD object.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
namespace RichDAQHeaderV1
{

  /** @namespace RichDAQHeaderV1::RichDAQHeaderPDCode
   *
   *  Namespace for definitions related to RichDAQHeaderV1::RichDAQHeaderPD
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-06
   */
  namespace RichDAQHeaderPDCode
  {

    // Define the number of bits for each field
    static const RichDAQ::ShortType BitsHitCount  =  10; ///< Bits for number of hits
    static const RichDAQ::ShortType BitsL0ID      =  12; ///< Bits for L0 identifier
    static const RichDAQ::ShortType BitsUnUsed1   =  8;  ///< Unused bits

    // Create the shift registers
    static const RichDAQ::ShortType ShiftHitCount = 0;
    static const RichDAQ::ShortType ShiftL0ID     = ShiftHitCount + BitsHitCount;

    // Create the Masks
    static const RichDAQ::LongType MaskHitCount   = ((1 << BitsHitCount)-1) << ShiftHitCount;
    static const RichDAQ::LongType MaskL0ID       = ((1 << BitsL0ID)-1)  << ShiftL0ID;

    // Create the max values that can be stored in each field
    static const RichDAQ::ShortType MaxHitCount   = ( 1 << BitsHitCount ) - 1; ///< Max number of hits
    static const RichDAQ::ShortType MaxL0ID       = ( 1 << BitsL0ID  ) - 1;    ///< Max L0 ID

  }

  /** @class RichDAQHeaderPD RichDAQHeaderPD.h
   *
   *  Utility class representing the header word for a Level0/HPD data block.
   *
   *  Version 1 : Fixed with new HPD, zero-suppression and Alice mode bit locations
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-06
   */
  class RichDAQHeaderPD : public RichDAQHeaderPDBase
  {

  public: // methods

    /// Copy constructor
    RichDAQHeaderPD ( const RichDAQHeaderPD & header ) : RichDAQHeaderPDBase( header.data() ) { }

    /// Constructor from RichDAQ::LongType
    RichDAQHeaderPD ( const RichDAQ::LongType data = 0 ) : RichDAQHeaderPDBase( data ) { }

    /// Constructor from all data
    RichDAQHeaderPD ( const bool zSupp,                    ///< Flag indicating if the block is zero suppressed
                      const RichDAQ::Level0ID  l0ID,       ///< The Level 0 hardware identifier
                      const RichDAQ::ShortType hitCount,   ///< The number of hits in this block
                      const RichDAQ::ShortType startPD = 1 ///< New HPD flag
                      )
      : RichDAQHeaderPDBase ( 0 )
    {
      setStartPD        ( startPD  );
      setZeroSuppressed ( zSupp    );
      setL0ID           ( l0ID     );
      setHitCount       ( hitCount );
    }

    /// Destructor
    virtual ~RichDAQHeaderPD( ) {}

    /// Set the Level0 ID
    inline bool setL0ID( const RichDAQ::Level0ID l0id )
    {
      return ( dataInRange(l0id.dataValue(),RichDAQHeaderPDCode::MaxL0ID) ?
               set( l0id.dataValue(), RichDAQHeaderPDCode::ShiftL0ID,
                    RichDAQHeaderPDCode::MaskL0ID ) : false );
    }

    /// Retrieve the Level0 ID
    inline RichDAQ::Level0ID l0ID() const
    {
      return RichDAQ::Level0ID
        ( (data() & RichDAQHeaderPDCode::MaskL0ID) >> RichDAQHeaderPDCode::ShiftL0ID );
    }

    /// Set the number of hits
    inline bool setHitCount( const RichDAQ::ShortType hitCount )
    {
      return ( dataInRange(hitCount,RichDAQHeaderPDCode::MaxHitCount) ?
               set( hitCount, RichDAQHeaderPDCode::ShiftHitCount,
                    RichDAQHeaderPDCode::MaskHitCount ) : false );
    }

    /// Retrieve the number of hits
    inline RichDAQ::ShortType hitCount() const
    {
      return ( (data() & RichDAQHeaderPDCode::MaskHitCount)
               >> RichDAQHeaderPDCode::ShiftHitCount );
    }

  };

} // RichDAQHeaderV1 namespace

/// overloaded output to std::ostream
inline std::ostream & operator << ( std::ostream & os,
                                    const RichDAQHeaderV1::RichDAQHeaderPD & header )
{
  os << " RichDAQHeaderPD V1 : L0ID = " << header.l0ID()
     << " ZS = " << header.zeroSuppressed()
     << " HitCount = " << header.hitCount();
  return os;
}

#endif // RICHDAQ_RICHDAQHEADERPD_H
