
//=============================================================================================
/** @file RichDAQHeaderPD_V3.h
 *
 *  Header file for RICH DAQ class : RichDAQHeaderV3::RichDAQHeaderPD
 *
 *  $Id: RichDAQHeaderPD_V3.h,v 1.2 2006-09-20 13:07:12 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//=============================================================================================

#ifndef RICHDAQ_RICHDAQHEADERPD_V3_H
#define RICHDAQ_RICHDAQHEADERPD_V3_H 1

// Base class
#include "RichDAQHeaderPDBase.h"

// Gaudi
#include "GaudiKernel/GaudiException.h"

// numbering
#include "RichDet/RichDAQDefinitions.h"

/** @namespace RichDAQHeaderV3
 *
 *  Namespace for version 1 of the RichDAQHeaderPD object.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
namespace RichDAQHeaderV3
{

  /** @namespace RichDAQHeaderV3::RichDAQHeaderPDCode
   *
   *  Namespace for definitions related to RichDAQHeaderV3::RichDAQHeaderPD
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-06
   */
  namespace RichDAQHeaderPDCode
  {

    /// Number of words in this header
    static const RichDAQ::ShortType nHeaderWords = 3;

    // first header word
    //-----------------------------------------------------------------------------------------

    // Define the number of bits for each field
    static const RichDAQ::ShortType BitsNeightBitP1 = 11; ///< Bits for # 8-bit blocks + 1
    static const RichDAQ::ShortType BitsZS          = 1;  ///< Bits for the zero suppression flag
    static const RichDAQ::ShortType BitsAlice       = 1;  ///< Bits for the ALICE/LHCb mode flag
    static const RichDAQ::ShortType BitsMemID       = 3;  ///< Bits for the memory ID
    static const RichDAQ::ShortType BitsEventID     = 15; ///< Bits for Event ID

    // Create the shift registers
    static const RichDAQ::ShortType ShiftNeightBitP1 = 0;
    static const RichDAQ::ShortType ShiftZS          = ShiftNeightBitP1 + BitsNeightBitP1;
    static const RichDAQ::ShortType ShiftAlice       = ShiftZS + BitsZS;
    static const RichDAQ::ShortType ShiftMemID       = ShiftAlice + BitsAlice;
    static const RichDAQ::ShortType ShiftEventID     = ShiftMemID + BitsMemID;

    // Create the Masks
    static const RichDAQ::LongType MaskNeightBitP1  = ((1 << BitsNeightBitP1)-1) << ShiftNeightBitP1;
    static const RichDAQ::LongType MaskZS           = ((1 << BitsZS)-1)          << ShiftZS;
    static const RichDAQ::LongType MaskAlice        = ((1 << BitsAlice)-1)       << ShiftAlice;
    static const RichDAQ::LongType MaskMemID        = ((1 << BitsMemID )-1)      << ShiftMemID ;
    static const RichDAQ::LongType MaskEventID      = ((1 << BitsEventID)-1)     << ShiftEventID;

    //-----------------------------------------------------------------------------------------

    // second header word
    //-----------------------------------------------------------------------------------------
    // This word is ignored for the moment
    //-----------------------------------------------------------------------------------------

    // third header word
    //-----------------------------------------------------------------------------------------

    // Define the number of bits for each field
    static const RichDAQ::ShortType BitsL0ID        = 11;  ///< Bits for L0 identifier
    // rest is ignored

    // Create the shift registers
    static const RichDAQ::ShortType ShiftL0ID       = 0;

    // Create the Masks
    static const RichDAQ::LongType MaskL0ID        = ((1 << BitsL0ID)-1)     << ShiftL0ID;

    //-----------------------------------------------------------------------------------------

    // Create the max values that can be stored in each field
    static const RichDAQ::ShortType MaxNeightBitP1 = ( 1 << BitsNeightBitP1 ) - 1; ///< Max number of hits
    static const RichDAQ::ShortType MaxL0ID        = ( 1 << BitsL0ID  ) - 1;    ///< Max L0 ID

  }

  /** @class RichDAQHeaderPD RichDAQHeaderPD_V3.h
   *
   *  Utility class representing the header word for a Level0/HPD data block.
   *
   *  Version 3 : Multi word version used in September 2006 testbeam
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
      : RichDAQHeaderPDBase(RichDAQHeaderPDCode::nHeaderWords) // header has three words
    {
      headerWords()[0] = *data;
      headerWords()[1] = *(++data);
      headerWords()[2] = *(++data);
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
        throw GaudiException("Data out of range","*RichDAQHeaderPDV3*",StatusCode::FAILURE);
      }
    }

    /// Destructor
    ~RichDAQHeaderPD( ) {}

    /// Retrieve the Level0 ID
    inline RichDAQ::Level0ID l0ID() const
    {
      return RichDAQ::Level0ID
        ( (headerWords()[2] & RichDAQHeaderPDCode::MaskL0ID) >> RichDAQHeaderPDCode::ShiftL0ID );
    }

    /// Set the Level0 ID
    inline bool setL0ID( const RichDAQ::Level0ID l0id )
    {
      return ( dataInRange(l0id.data(),RichDAQHeaderPDCode::MaxL0ID) ?
               set( l0id.data(), RichDAQHeaderPDCode::ShiftL0ID,
                    RichDAQHeaderPDCode::MaskL0ID, 2 ) : false );
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
                    RichDAQHeaderPDCode::MaskNeightBitP1, 0 ) : false );
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
      return set( i, RichDAQHeaderPDCode::ShiftZS, RichDAQHeaderPDCode::MaskZS, 0 );
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
      return set( i, RichDAQHeaderPDCode::ShiftAlice, RichDAQHeaderPDCode::MaskAlice, 0 );
    }

    /** Returns the number of words in the data block associated to this header
     *  taking into account the bank type (zero-supressed or not)
     */
    inline unsigned int nDataWords() const
    {
      return ( zeroSuppressed() ? nEightBitBlocksPlusOne()/2 :
               ( aliceMode() ? RichDAQ::MaxDataSizeALICE : RichDAQ::MaxDataSize ) );
    }

   


    /// Retrieve the "memID" word
    inline unsigned int memID() const
    {
      return ( 0 != ( (headerWords()[0] & RichDAQHeaderPDCode::MaskMemID)
                      >> RichDAQHeaderPDCode::ShiftMemID ) );
    }

    /// Retrieve the "memID" word
    inline unsigned int eventID() const
    {
      return ( 0 != ( (headerWords()[0] & RichDAQHeaderPDCode::MaskEventID)
                      >> RichDAQHeaderPDCode::ShiftEventID ) );
    }

  };

} // RichDAQHeaderV3 namespace

/// overloaded output to std::ostream
inline std::ostream & operator << ( std::ostream & os,
                                    const RichDAQHeaderV3::RichDAQHeaderPD & header )
{
  os << "HPD header V3 : nHeadW = " << header.nHeaderWords()
     << " L0ID=" << header.l0ID()
     << " MemID=" << header.memID() 
     << " EventID=" << header.eventID() 
     << " ZS=" << header.zeroSuppressed()
     << " AliceMode=" << header.aliceMode()
     << " # 8-bit blocks+1=" << header.nEightBitBlocksPlusOne();
  return os;
}

#endif // RICHDAQ_RICHDAQHEADERPD_V3_H
