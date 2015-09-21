
//=============================================================================================
/** @file RichDAQHeaderPD_V1.h
 *
 *  Header file for RICH DAQ class : RichDAQHeaderV1::RichDAQHeaderPD
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//=============================================================================================

#ifndef RICHDAQ_RICHDAQHEADERPD_V1_H
#define RICHDAQ_RICHDAQHEADERPD_V1_H 1

// Base class
#include "RichKernel/RichDAQHeaderPDBase.h"

// Gaudi
#include "GaudiKernel/GaudiException.h"

// numbering
#include "RichKernel/RichDAQDefinitions.h"

namespace Rich
{
  namespace DAQ
  {

    /** @namespace Rich::DAQ::RichDAQHeaderV1
     *
     *  Namespace for version 1 of the RichDAQHeaderPD object.
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   2004-12-17
     */
    namespace RichDAQHeaderV1
    {

      /** @namespace Rich::DAQ::RichDAQHeaderV1::RichDAQHeaderPDCode
       *
       *  Namespace for definitions related to RichDAQHeaderV1::RichDAQHeaderPD
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       *  @date   2003-11-06
       */
      namespace RichDAQHeaderPDCode
      {

        /// Number of words in this header
        static const ShortType nHeaderWords    = 1;

        // Define the number of bits for each field
        static const ShortType BitsHitCount  = 10;  ///< Bits for number of hits
        static const ShortType BitsL0ID      = 12;  ///< Bits for L0 identifier
        static const ShortType BitsUnUsed1   = 8;   ///< Unused bits
        static const ShortType BitsZS        = 1;   ///< Bits for the zero suppression flag
        static const ShortType BitsStartPD   = 1;   ///< Bits for the new HPD flag

        // Create the shift registers
        static const ShortType ShiftHitCount = 0;
        static const ShortType ShiftL0ID     = ShiftHitCount + BitsHitCount;
        static const ShortType ShiftZS       = ShiftL0ID + BitsL0ID + BitsUnUsed1;
        static const ShortType ShiftStartPD  = ShiftZS + BitsZS;

        // Create the Masks
        static const LongType MaskHitCount   = (LongType) (((1 << BitsHitCount)-1) << ShiftHitCount);
        static const LongType MaskL0ID       = (LongType) (((1 << BitsL0ID)-1)     << ShiftL0ID);
        static const LongType MaskZS         = (LongType) (((1 << BitsZS)-1)       << ShiftZS);
        static const LongType MaskStartPD    = (LongType) (((1 << BitsStartPD)-1)  << ShiftStartPD);

        // Create the max values that can be stored in each field
        static const ShortType MaxHitCount   = (ShortType) ( (1 << BitsHitCount) - 1 ); ///< Max number of hits
        static const ShortType MaxL0ID       = (ShortType) ( (1 << BitsL0ID)     - 1 ); ///< Max L0 ID

      }

      /** @class RichDAQHeaderPD RichDAQHeaderPD_V1.h
       *
       *  Utility class representing the header word for a Level0/HPD data block.
       *
       *  Version 1 : Single word header compatible with DC06
       *
       *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
       *  @date   2003-11-06
       */
      class RichDAQHeaderPD : public HeaderPDBase,
                              public LHCb::MemPoolAlloc<RichDAQHeaderPD>
      {

      public: // methods

        /// Default Constructor
        explicit RichDAQHeaderPD () : HeaderPDBase(RichDAQHeaderPDCode::nHeaderWords) { }

        /// Copy constructor
        RichDAQHeaderPD ( const RichDAQHeaderPD & header )
          : HeaderPDBase(header.headerWords()) { }

        /// Constructor from a pointer to a data stream
        explicit RichDAQHeaderPD ( const LongType * data )
          : HeaderPDBase(RichDAQHeaderPDCode::nHeaderWords)
        {
          headerWords()[0] = *(data++);
        }

        /// Constructor from all data
        RichDAQHeaderPD ( const bool zSupp,           ///< Flag indicating if the block is zero suppressed
                          const Level0ID  l0ID,       ///< The Level 0 hardware identifier
                          const ShortType dSize,      ///< The data size word
                          const ShortType startPD = 1 ///< New HPD flag
                          )
          : HeaderPDBase(RichDAQHeaderPDCode::nHeaderWords)
        {
          if ( !setStartPD(startPD) || !setZeroSuppressed(zSupp)         ||
               !setL0ID(l0ID)       || !setNEightBitBlocksPlusOne(dSize) )
          {
            throw GaudiException("Data out of range","*RichDAQHeaderPD*",StatusCode::FAILURE);
          }
        }

        /// Destructor
        ~RichDAQHeaderPD( ) {}

      public:

        /// reset
        inline void reset( ) { headerWords()[0] = 0; }

        /// reset for a new data stream
        inline void reset( const LongType * data )
        {
          reset( );
          readFromDataStream( data );
        }

      public:

        /// Set the Level0 ID
        inline bool setL0ID( const Level0ID l0id )
        {
          return ( dataInRange(l0id.data(),RichDAQHeaderPDCode::MaxL0ID) ?
                   set( l0id.data(), RichDAQHeaderPDCode::ShiftL0ID,
                        RichDAQHeaderPDCode::MaskL0ID ) : false );
        }

        /// Retrieve the Level0 ID
        inline Level0ID l0ID() const
        {
          return Level0ID
            ( (headerWords()[0] & RichDAQHeaderPDCode::MaskL0ID) >> RichDAQHeaderPDCode::ShiftL0ID );
        }

        /// Retrieve the number of "8-bit data blocks plus one" with at least one hit
        inline ShortType nEightBitBlocksPlusOne() const
        {
          return ( (headerWords()[0] & RichDAQHeaderPDCode::MaskHitCount)
                   >> RichDAQHeaderPDCode::ShiftHitCount );
        }

        /// Set the number of "8-bit data blocks plus one" with at least one hit
        inline bool setNEightBitBlocksPlusOne( const ShortType value )
        {
          return ( dataInRange(value,RichDAQHeaderPDCode::MaxHitCount) ?
                   set( value, RichDAQHeaderPDCode::ShiftHitCount,
                        RichDAQHeaderPDCode::MaskHitCount ) : false );
        }

        /// Set new PD bit
        inline bool setStartPD( const ShortType value)
        {
          return set( value, RichDAQHeaderPDCode::ShiftStartPD,
                      RichDAQHeaderPDCode::MaskStartPD );
        }

        /// Is new PD bit set ?
        inline bool startPD() const
        {
          return ( 0 != ( (headerWords()[0] & RichDAQHeaderPDCode::MaskStartPD)
                          >> RichDAQHeaderPDCode::ShiftStartPD ) );
        }

        /// Set the zero suppression info
        inline bool setZeroSuppressed( const bool zSupp )
        {
          const ShortType i = ( zSupp ? 1 : 0 );
          return set( i, RichDAQHeaderPDCode::ShiftZS, RichDAQHeaderPDCode::MaskZS );
        }

        /// Retrieve the zero suppressed information
        inline bool zeroSuppressed() const
        {
          return ( 0 != ( (headerWords()[0] & RichDAQHeaderPDCode::MaskZS)
                          >> RichDAQHeaderPDCode::ShiftZS ) );
        }

        /// WARNING : This method is not implemented for this class version
        inline unsigned int nDataWords() const
        {
          throw GaudiException( "nDataWords() is not defined. Do Not Use","RichDAQHeaderV1",StatusCode::FAILURE);
          return 0;
        }

      public: // methods not properly implemented, but included for compatbility

        /// Returns if this header (and the associated footer) are in extended mode or not (compact)
        inline bool extendedFormat() const { return false; }

        /// Retrieve the event ID word
        inline EventID eventID() const { return EventID( 0, 0 ); }

        /// Is the data from this HPD suppressed by the L1
        inline bool l1Suppressed() const { return true; }

      };

    } // RichDAQHeaderV1 namespace

    /// overloaded output to std::ostream
    inline std::ostream & operator << ( std::ostream & os,
                                        const RichDAQHeaderV1::RichDAQHeaderPD & header )
    {
      os << "HPD header V1 : nHeadW = " << header.nHeaderWords()
         << " L0ID = " << header.l0ID()
         << " ZS = " << header.zeroSuppressed()
         << " # hits = " << header.nEightBitBlocksPlusOne();
      return os;
    }

  }
}

#endif // RICHDAQ_RICHDAQHEADERPD_V1_H
