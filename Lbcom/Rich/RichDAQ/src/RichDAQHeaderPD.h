
//=============================================================================================
/** @file RichDAQHeaderPD.h
 *
 *  Header file for RICH DAQ utility class : RichDAQHeaderPD
 *
 *  $Id: RichDAQHeaderPD.h,v 1.15 2005-12-16 15:11:34 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//=============================================================================================

#ifndef RICHDAQ_RICHDAQHEADERPD_H
#define RICHDAQ_RICHDAQHEADERPD_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Kernel
#include "RichKernel/RichDAQDefinitions.h"

// local
#include "RichDAQHPDIdentifier.h"

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
