
//=============================================================================================
/** @file RichDAQHeaderPD_V4.h
 *
 *  Header file for RICH DAQ class : RichDAQHeaderV4::RichDAQHeaderPD
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//=============================================================================================

#ifndef RICHDAQ_RICHDAQHEADERPD_V4_H
#define RICHDAQ_RICHDAQHEADERPD_V4_H 1

// Base class
#include "RichKernel/RichDAQHeaderPDBase.h"

// Gaudi
#include "GaudiKernel/GaudiException.h"

// numbering
#include "RichKernel/RichDAQDefinitions.h"

// L0 header
#include "RichKernel/RichDAQL0Header.h"

// CRJ : Need to put these here for SLC3/gcc323 compat.
namespace
{
  /// Number of words in this header (normal format)
  static const unsigned int nHeaderWordsNormal   = 1;
  /// Number of words in this header (extended format)
  static const unsigned int nHeaderWordsExtended = 3;
}

namespace Rich
{
  namespace DAQ
  {

    /** @namespace Rich::DAQ::RichDAQHeaderV4
     *
     *  Namespace for version 4 of the RichDAQHeaderPD object.
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   2004-12-17
     */
    namespace RichDAQHeaderV4
    {

      /** @class RichDAQHeaderPD RichDAQHeaderPD_V4.h
       *
       *  Utility class representing the header word for a Level0/HPD data block.
       *
       *  Version 4 : Final (?) version for 2007
       *
       *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
       *  @date   2003-11-06
       */
      class RichDAQHeaderPD : public HeaderPDBase
      {

        // primary header word packing info
        //-----------------------------------------------------------------------------------------

      public:

        // Define the number of bits for each field
        static const ShortType BitsL0ID        = 11; ///< Bits for L0 identifier
        static const ShortType BitsEventID     = 5;  ///< Bits for Event ID
        static const ShortType BitsNeightBit   = 11; ///< Bits for # of 8-bit blocks in the data
        static const ShortType BitsZS          = 1;  ///< Bits for the zero suppression flag
        static const ShortType BitsAlice       = 1;  ///< Bits for the ALICE/LHCb mode flag
        static const ShortType BitsDataFormat  = 1;  ///< Bits for the data format (0=compact 1=extended)
        static const ShortType BitsGTInhibit   = 1;  ///< Bits for the GT inhibit flag

      private:

        // Create the shift registers
        static const ShortType ShiftL0ID        = 0;
        static const ShortType ShiftEventID     = ShiftL0ID + BitsL0ID;
        static const ShortType ShiftNeightBit   = ShiftEventID + BitsEventID;
        static const ShortType ShiftZS          = ShiftNeightBit + BitsNeightBit;
        static const ShortType ShiftAlice       = ShiftZS + BitsZS;
        static const ShortType ShiftDataFormat  = ShiftAlice + BitsAlice;
        static const ShortType ShiftGTInhibit   = ShiftDataFormat + BitsDataFormat;

        // Create the Masks
        static const LongType MaskL0ID       = (LongType) ((1 << BitsL0ID)-1)        << ShiftL0ID;
        static const LongType MaskEventID    = (LongType) ((1 << BitsEventID)-1)     << ShiftEventID;
        static const LongType MaskNeightBit  = (LongType) ((1 << BitsNeightBit)-1)   << ShiftNeightBit;
        static const LongType MaskZS         = (LongType) ((1 << BitsZS)-1)          << ShiftZS;
        static const LongType MaskAlice      = (LongType) ((1 << BitsAlice)-1)       << ShiftAlice;
        static const LongType MaskDataFormat = (LongType) ((1 << BitsDataFormat)-1)  << ShiftDataFormat;
        static const LongType MaskGTInhibit  = (LongType) ((1 << BitsGTInhibit)-1)   << ShiftGTInhibit;

        // Max values that can be stored
        static const ShortType MaxL0ID       = ( 1 << BitsL0ID  ) - 1;       ///< Max L0 ID
        static const ShortType MaxEventID    = ( 1 << BitsEventID  ) - 1;    ///< Max Event ID
        static const ShortType MaxNeightBit  = ( 1 << BitsNeightBit  ) - 1;  ///< Max '# 8 bit' word

        //-----------------------------------------------------------------------------------------

      public: // methods

        /// Default Constructor
        explicit RichDAQHeaderPD() : HeaderPDBase(nHeaderWordsNormal) { }

        /// Copy constructor
        RichDAQHeaderPD ( const RichDAQHeaderPD & header )
          : HeaderPDBase(header.headerWords()) { }

        /// Constructor from a pointer to a data stream
        explicit RichDAQHeaderPD ( const LongType * data )
          : HeaderPDBase(nHeaderWordsNormal) 
        {
          readFromDataStream(data);
        }

        /// Constructor from raw header word(s)
        explicit RichDAQHeaderPD( const HeaderPDBase::HeaderWords & words )
          : HeaderPDBase(words) { }

        /// Constructor from all data
        RichDAQHeaderPD ( const bool zSupp,          ///< Flag indicating if the block is zero suppressed
                          const bool aMode,          ///< Flag indicating if data is ALICE mode
                          const bool extFormat,      ///< Flag indicating the data format is extended
                          const bool gtInhib,        ///< The GT inhibit flag
                          const Level0ID  l0ID,      ///< The Level 0 hardware identifier
                          const EventID   evtID,     ///< The Event ID
                          const ShortType dSize      ///< The data size word
                          )
          : HeaderPDBase( extFormat ? nHeaderWordsExtended : nHeaderWordsNormal )
        {
          setExtendedFormat ( extFormat );
          setZeroSuppressed ( zSupp     );
          setAliceMode      ( aMode     );
          setEventID        ( evtID     );
          setInhibit        ( gtInhib   );
          if ( !setL0ID(l0ID) || !setNEightBitBlocks(dSize) )
          {
            throw GaudiException("Data out of range","*RichDAQHeaderPDV4*",StatusCode::FAILURE);
          }
        }

        /// Destructor
        virtual ~RichDAQHeaderPD( ) { }

      public:
        
        /// reset
        inline void reset( )
        {
          if ( nHeaderWords() > 1 ) 
          {
            headerWords() = HeaderPDBase::HeaderWords(nHeaderWordsNormal,0); 
          }
          headerWords()[0] = 0;
        }

        /// reset for a new data stream
        inline void reset( const LongType * data )
        {
          reset( );
          readFromDataStream( data );
        }
        
      public:

        /// Retrieve the Level0 ID
        inline Level0ID l0ID() const
        {
          return Level0ID( (headerWords()[0] & MaskL0ID) >> ShiftL0ID );
        }

        /// Set the Level0 ID
        inline bool setL0ID( const Level0ID l0id )
        {
          return ( dataInRange(l0id.data(),MaxL0ID) ?
                   set( l0id.data(), ShiftL0ID, MaskL0ID ) : false );
        }

        /// Retrieve the event ID word
        inline EventID eventID() const
        {
          return EventID( ((headerWords()[0] & MaskEventID) >> ShiftEventID), BitsEventID );
        }

        /// Set the Event ID
        inline bool setEventID( const EventID & evID )
        {
          // don't test the range here, as we know we are only taking the lowest bits
          //return ( dataInRange((LongType)evID.data(),MaxEventID) ?
          //         set( (LongType)evID.data(), ShiftEventID, MaskEventID ) : false );
          return set( (LongType)evID.data(), ShiftEventID, MaskEventID );
        }

        /// Retrieve the number of "8-bit data blocks" with at least one hit
        inline ShortType nEightBitBlocks() const
        {
          // only trust nEightBitBlocks word if not GT inhibit
          return ( inhibit() ? 0 :
                   ( (headerWords()[0] & MaskNeightBit) >> ShiftNeightBit ) );
        }

        /// Set the number of "8-bit data blocks" with at least one hit
        inline bool setNEightBitBlocks( const ShortType value )
        {
          return ( dataInRange(value,MaxNeightBit) ?
                   set( value, ShiftNeightBit, MaskNeightBit ) : false );
        }

        /// Retrieve the zero suppressed information
        inline bool zeroSuppressed() const
        {
          return ( 0 != ( (headerWords()[0] & MaskZS) >> ShiftZS ) );
        }

        /// Set the zero suppression info
        inline bool setZeroSuppressed( const bool zSupp )
        {
          const ShortType i = ( zSupp ? 1 : 0 );
          return set( i, ShiftZS, MaskZS );
        }

        /// Retrieve the flag to say if the data is in ALICE mode
        inline bool aliceMode() const
        {
          return ( 0 != ( (headerWords()[0] & MaskAlice) >> ShiftAlice ) );
        }

        /// Set the flag to say if the data is in ALICE mode
        inline bool setAliceMode( const bool aMode )
        {
          const ShortType i = ( aMode ? 1 : 0 );
          return set( i, ShiftAlice, MaskAlice );
        }

        /// Returns if this header (and the associated footer) are in extended mode or not (compact)
        inline bool extendedFormat() const
        {
          return ( 0 != ( (headerWords()[0] & MaskDataFormat) >> ShiftDataFormat ) );
        }

        /// Set the data format bit
        inline bool setExtendedFormat( const bool format )
        {
          if ( format ) makeExtended();
          const ShortType i = ( format ? 1 : 0 );
          return set( i, ShiftDataFormat, MaskDataFormat );
        }

        /// Returns the HPD data inhibit flag
        inline bool inhibit() const
        {
          return ( 0 != ( (headerWords()[0] & MaskGTInhibit) >> ShiftGTInhibit ) );
        }

        /// Set the GT inhibit word
        inline bool setInhibit( const bool inhibit )
        {
          const ShortType i = ( inhibit ? 1 : 0 );
          return set( i, ShiftGTInhibit, MaskGTInhibit );
        }

      public:

        /// Read correct number of data words from given stream
        /// Note, after this call data pointer is incremented to the next word after the header
        virtual void readFromDataStream( const LongType *& data );

        /// Retrieve the L0 headers
        inline L0Header l0Header() const
        {
          return ( extendedFormat() ?
                   L0Header ( L0Header::Word0(headerWords()[1]),
                              L0Header::Word1(headerWords()[2]) ) :
                   L0Header ( L0Header::Word0(0), L0Header::Word1(0) ) );
        }

        /// Set the L0 header words
        inline void setL0Headers( const L0Header & l0header )
        {
          makeExtended();
          headerWords()[1] = l0header.word0().data();
          headerWords()[2] = l0header.word1().data();
        }

        /// Check overall status of the header word
        inline bool l1Suppressed() const
        {
          return inhibit();
        }

      public:

        /** Returns the number of words in the data block associated to this header
         *  taking into account the bank type (zero-supressed or not)
         */
        inline unsigned int nDataWords() const
        {
          return ( inhibit() ? 0 :
                   ( zeroSuppressed() ? nEightBitBlocks()/2 + nEightBitBlocks()%2 :
                     ( aliceMode() ? MaxDataSizeALICE : MaxDataSize ) ) );
        }

        /// Make sure this header has the extended L0 data words available
        void makeExtended();

      };

    } // RichDAQHeaderV4 namespace

    /// overloaded output to std::ostream
    inline std::ostream & operator << ( std::ostream & os,
                                        const RichDAQHeaderV4::RichDAQHeaderPD & header )
    {
      os << "HPD Header V4 : nHeadW=" << header.nHeaderWords()
         << " L0ID="           << header.l0ID()
         << " EventID="        << header.eventID()
         << " ZS="             << header.zeroSuppressed()
         << " AliceMode="      << header.aliceMode()
         << " ExtendedFormat=" << header.extendedFormat()
         << " Inhibit="        << header.inhibit()
         << " # 8-bit blocks=" << header.nEightBitBlocks();
      return os;
    }

  }
}

#endif // RICHDAQ_RICHDAQHEADERPD_V4_H
