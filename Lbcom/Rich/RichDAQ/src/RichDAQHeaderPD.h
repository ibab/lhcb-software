
/** @file RichDAQHeaderPD.h
 *
 *  Header file for RICH DAQ utility class : RichDAQHeaderPD
 *
 *  CVS Log :-
 *  $Id: RichDAQHeaderPD.h,v 1.11 2005-01-13 13:09:12 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.10  2005/01/07 12:35:59  jonrob
 *  Complete rewrite
 *
 *  Revision 1.9  2004/10/13 09:16:27  jonrob
 *  Use Data on Demand Service + various speed improvements
 *
 *  Revision 1.8  2004/08/17 17:54:22  jonrob
 *  Update doxygen comments
 *
 *  Revision 1.7  2004/07/27 13:46:06  jonrob
 *  Add doxygen file documentation and CVS information
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

  /** @namespace RichDAQHeaderV0::RichDAQHeaderPDCode
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
    static const RichDAQ::ShortType MaxHitCount  = ( 1 << BitsHitCount ) - 1;
    static const RichDAQ::ShortType MaxHPDID     = ( 1 << BitsHPDID  ) - 1;

  }

  /** @class RichDAQHeaderV0::RichDAQHeaderPD RichDAQHeaderPD.h
   *
   *  Utility class representing the header word for HPD data
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

    // Is new PD bit set ?
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

    /// tests whether a given value is in range for a given data field
    inline bool dataInRange( const RichDAQ::ShortType value,
                             const RichDAQ::ShortType max ) const
    {
      return ( value <= max );
    }

  private: // data

    RichDAQ::LongType m_data;

  };

} // RichDAQHeaderV0 namespace

/// overloaded output to MsgStream
inline MsgStream & operator << ( MsgStream & os,
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
 *  Namespace for version 0 of the RichDAQHeaderPD object.
 *  DC04 compatible version (Now obsolete).
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
namespace RichDAQHeaderV1 {

  /** @namespace RichDAQHeaderV1::RichDAQHeaderPDCode
   *
   *  Namespace for definitions related to RichDAQHeaderV1::RichDAQHeaderPD
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-06
   */
  namespace RichDAQHeaderPDCode {

    // Define the number of bits for each field
    static const RichDAQ::ShortType BitsHitCount  =  10; ///< Bits for number of hits
    static const RichDAQ::ShortType BitsHPDID     =  12; ///< Bits for HPD identifier
    static const RichDAQ::ShortType BitsUnUsed1   =  8;  ///< Unused bits

    // Create the shift registers
    static const RichDAQ::ShortType ShiftHitCount = 0;
    static const RichDAQ::ShortType ShiftHPDID    = ShiftHitCount + BitsHitCount;

    // Create the Masks
    static const RichDAQ::LongType MaskHitCount   = ((1 << BitsHitCount)-1) << ShiftHitCount;
    static const RichDAQ::LongType MaskHPDID      = ((1 << BitsHPDID)-1)  << ShiftHPDID;

    // Create the max values that can be stored in each field
    static const RichDAQ::ShortType MaxHitCount   = ( 1 << BitsHitCount ) - 1;
    static const RichDAQ::ShortType MaxHPDID      = ( 1 << BitsHPDID  ) - 1;

  }

  /** @class RichDAQHeaderV1::RichDAQHeaderPD RichDAQHeaderPD.h
   *
   *  Utility class representing the header word for HPD data
   *  Version 1 : Fixed new HPD, zero-suppression and Alice mode bit locations
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-06
   */
  class RichDAQHeaderPD : public RichDAQHeaderPDBase {

  public: // methods

    /// Copy constructor
    RichDAQHeaderPD ( const RichDAQHeaderPD & header ) : RichDAQHeaderPDBase( header.data() ) { }

    /// Constructor from RichDAQ::LongType
    RichDAQHeaderPD ( const RichDAQ::LongType data = 0 ) : RichDAQHeaderPDBase( data ) { }

    /// Constructor from all data
    RichDAQHeaderPD ( const bool zSupp,                    ///< Flag indicating if the block is zero suppressed
                      const RichDAQ::HPDHardwareID hpdID,  ///< The HPD hardware identifier
                      const RichDAQ::ShortType hitCount,   ///< The number of hits in this block
                      const RichDAQ::ShortType startPD = 1 ///< New HPD flag
                      )
      : RichDAQHeaderPDBase ( 0 )
    {
      setStartPD        ( startPD  );
      setZeroSuppressed ( zSupp    );
      setHPDID          ( hpdID    );
      setHitCount       ( hitCount );
    }

    /// Destructor
    virtual ~RichDAQHeaderPD( ) {}

    /// Set the HPD ID
    inline bool setHPDID( const RichDAQ::ShortType hpdid )
    {
      return ( dataInRange(hpdid,RichDAQHeaderPDCode::MaxHPDID) ?
               set( hpdid, RichDAQHeaderPDCode::ShiftHPDID,
                    RichDAQHeaderPDCode::MaskHPDID ) : false );
    }

    /// Retrieve the HPD ID
    inline RichDAQ::HPDHardwareID hpdID() const
    {
      return RichDAQ::HPDHardwareID
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

  };

} // RichDAQHeaderV1 namespace

/// overloaded output to MsgStream
inline MsgStream & operator << ( MsgStream & os,
                                 const RichDAQHeaderV1::RichDAQHeaderPD & header )
{
  os << " RichDAQHeaderPD V1 : HPD = " << header.hpdID()
     << " ZS = " << header.zeroSuppressed()
     << " HitCount = " << header.hitCount();
  return os;
}

#endif // RICHDAQ_RICHDAQHEADERPD_H
