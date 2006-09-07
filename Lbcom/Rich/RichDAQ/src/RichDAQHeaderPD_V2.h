
//=============================================================================================
/** @file RichDAQHeaderPD_V2.h
 *
 *  Header file for RICH DAQ class : RichDAQHeaderV2::RichDAQHeaderPD
 *
 *  $Id: RichDAQHeaderPD_V2.h,v 1.1 2006-09-07 17:14:10 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//=============================================================================================

#ifndef RICHDAQ_RICHDAQHEADERPD_V2_H
#define RICHDAQ_RICHDAQHEADERPD_V2_H 1

// Base class
#include "RichDAQHeaderPDBase.h"

// Gaudi
#include "GaudiKernel/GaudiException.h"

// numbering
#include "RichDet/RichDAQDefinitions.h"

/** @namespace RichDAQHeaderV2
 *
 *  Namespace for version 1 of the RichDAQHeaderPD object.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
namespace RichDAQHeaderV2
{

  /** @namespace RichDAQHeaderV2::RichDAQHeaderPDCode
   *
   *  Namespace for definitions related to RichDAQHeaderV2::RichDAQHeaderPD
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-06
   */
  namespace RichDAQHeaderPDCode
  {

    /// Number of words in this header
    static const RichDAQ::ShortType nHeaderWords    = 1;

    // Define the number of bits for each field
    static const RichDAQ::ShortType BitsNeightBitP1 = 10;  ///< Bits for # 8-bit blocks + 1
    static const RichDAQ::ShortType BitsL0ID        = 12;  ///< Bits for L0 identifier
    static const RichDAQ::ShortType BitsUnUsed1     = 7;   ///< Unused bits
    static const RichDAQ::ShortType BitsAlice       = 1;   ///< Bits for the ALICE/LHCb mode flag
    static const RichDAQ::ShortType BitsZS          = 1;   ///< Bits for the zero suppression flag

    // Create the shift registers
    static const RichDAQ::ShortType ShiftNeightBitP1 = 0;
    static const RichDAQ::ShortType ShiftL0ID        = ShiftNeightBitP1 + BitsNeightBitP1;
    static const RichDAQ::ShortType ShiftAlice       = ShiftL0ID + BitsL0ID + BitsUnUsed1;
    static const RichDAQ::ShortType ShiftZS          = ShiftAlice + BitsAlice;

    // Create the Masks
    static const RichDAQ::LongType MaskNeightBitP1 = ((1 << BitsNeightBitP1)-1) << ShiftNeightBitP1;
    static const RichDAQ::LongType MaskL0ID        = ((1 << BitsL0ID)-1)        << ShiftL0ID;
    static const RichDAQ::LongType MaskAlice       = ((1 << BitsAlice)-1)       << ShiftAlice;
    static const RichDAQ::LongType MaskZS          = ((1 << BitsZS)-1)          << ShiftZS;

    // Create the max values that can be stored in each field
    static const RichDAQ::ShortType MaxNeightBitP1 = ( 1 << BitsNeightBitP1 ) - 1; ///< Max number of hits
    static const RichDAQ::ShortType MaxL0ID        = ( 1 << BitsL0ID  ) - 1;    ///< Max L0 ID

  }

  /** @class RichDAQHeaderPD RichDAQHeaderPD_V2.h
   *
   *  Utility class representing the header word for a Level0/HPD data block.
   *
   *  Version 2 : Final single word version. Under development
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-06
   */
  class RichDAQHeaderPD : public RichDAQHeaderPDBase
  {

  public: // methods

    /// Default Constructor
    explicit RichDAQHeaderPD () : RichDAQHeaderPDBase(RichDAQHeaderPDCode::nHeaderWords) { }

    /// Copy constructor
    RichDAQHeaderPD ( const RichDAQHeaderPD & header )
      : RichDAQHeaderPDBase(header) { }

    /// Constructor from a pointer to a data stream
    explicit RichDAQHeaderPD ( const RichDAQ::LongType * data )
      : RichDAQHeaderPDBase(RichDAQHeaderPDCode::nHeaderWords) // header has one word only
    {
      headerWords()[0] = *data;
    }

    /// Constructor from all data
    RichDAQHeaderPD ( const bool zSupp,                   ///< Flag indicating if the block is zero suppressed
                      const bool aMode,                   ///< Flag indicating if data is ALICE mode
                      const RichDAQ::Level0ID  l0ID,      ///< The Level 0 hardware identifier
                      const RichDAQ::ShortType dSize      ///< The data size word
                      )
      : RichDAQHeaderPDBase(RichDAQHeaderPDCode::nHeaderWords) 
    {
      if ( !setZeroSuppressed(zSupp) || !setAliceMode(aMode) ||
           !setL0ID(l0ID)            || !setNEightBitBlocksPlusOne(dSize) )
      {
        throw GaudiException("Data out of range","*RichDAQHeaderPDV2*",StatusCode::FAILURE);
      }
    }

    /// Destructor
    ~RichDAQHeaderPD( ) {}

    /// Retrieve the Level0 ID
    inline RichDAQ::Level0ID l0ID() const
    {
      return RichDAQ::Level0ID
        ( (headerWords()[0] & RichDAQHeaderPDCode::MaskL0ID) >> RichDAQHeaderPDCode::ShiftL0ID );
    }

    /// Set the Level0 ID
    inline bool setL0ID( const RichDAQ::Level0ID l0id )
    {
      return ( dataInRange(l0id.data(),RichDAQHeaderPDCode::MaxL0ID) ?
               set( l0id.data(), RichDAQHeaderPDCode::ShiftL0ID,
                    RichDAQHeaderPDCode::MaskL0ID ) : false );
    }

    /// Retrieve the number of "8-bit data blocks plus one" with at least one hit
    inline RichDAQ::ShortType nEightBitBlocksPlusOne() const
    {
      return ( (headerWords()[0] & RichDAQHeaderPDCode::MaskNeightBitP1)
               >> RichDAQHeaderPDCode::ShiftNeightBitP1 );
    }

    /// Set the number of "8-bit data blocks plus one" with at least one hit
    inline bool setNEightBitBlocksPlusOne( const RichDAQ::ShortType value )
    {
      return ( dataInRange(value,RichDAQHeaderPDCode::MaxNeightBitP1) ?
               set( value, RichDAQHeaderPDCode::ShiftNeightBitP1,
                    RichDAQHeaderPDCode::MaskNeightBitP1 ) : false );
    }

    /// Retrieve the zero suppressed information
    inline bool zeroSuppressed() const
    {
      return ( 0 != ( (headerWords()[0] & RichDAQHeaderPDCode::MaskZS)
                      >> RichDAQHeaderPDCode::ShiftZS ) );
    }

    /// Set the zero suppression info
    inline bool setZeroSuppressed( const bool zSupp )
    {
      const RichDAQ::ShortType i = ( zSupp ? 1 : 0 );
      return set( i, RichDAQHeaderPDCode::ShiftZS, RichDAQHeaderPDCode::MaskZS );
    }

    /// Retrieve the flag to say if the data is in ALICE mode
    inline bool aliceMode() const
    {
      return ( 0 != ( (headerWords()[0] & RichDAQHeaderPDCode::MaskAlice)
                      >> RichDAQHeaderPDCode::ShiftAlice ) );
    }

    /// Set the flag to say if the data is in ALICE mode
    inline bool setAliceMode( const bool aMode )
    {
      const RichDAQ::ShortType i = ( aMode ? 1 : 0 );
      return set( i, RichDAQHeaderPDCode::ShiftAlice, RichDAQHeaderPDCode::MaskAlice );
    }

    /** Returns the number of words in the data block associated to this header
     *  taking into account the bank type (zero-supressed or not)
     */
    inline unsigned int nDataWords() const
    {
      return ( zeroSuppressed() ? nEightBitBlocksPlusOne()/2 :
               ( aliceMode() ? RichDAQ::MaxDataSizeALICE : RichDAQ::MaxDataSize ) );
    }

  };

} // RichDAQHeaderV2 namespace

/// overloaded output to std::ostream
inline std::ostream & operator << ( std::ostream & os,
                                    const RichDAQHeaderV2::RichDAQHeaderPD & header )
{
  os << "HPD header V2 : nHeadW = " << header.nHeaderWords() 
     << " L0ID = " << header.l0ID()
     << " ZS = " << header.zeroSuppressed()
     << " AliceMode = " << header.aliceMode()
     << " # 8-bit blocks + 1 = " << header.nEightBitBlocksPlusOne();
  return os;
}

#endif // RICHDAQ_RICHDAQHEADERPD_V2_H
