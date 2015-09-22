
/** @file RichZSPacked_V2.h
 *
 *  Header file for RICH DAQ utility class : Rich::DAQ::RichZSPacked_V2::RichZSPacked
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

#ifndef RICHDAQ_RICHZSPACKED_V2_H
#define RICHDAQ_RICHZSPACKED_V2_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// numberings
#include "RichKernel/RichDAQDefinitions.h"

namespace Rich
{
  namespace DAQ
  {

    /** @namespace Rich::DAQ::RichZSPacked_V2
     *
     *  Namespace for second version of LHCb mode ZS compressed data words
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   2005-01-12
     */
    namespace RichZSPacked_V2
    {

      /** @class RichZSPacked RichZSPacked_V2.h
       *
       *  Utility class representing 2 groups of address and
       *  bit-field packed into a single word for LHCb mode readout ONLY
       *
       *  Second version. Inverted row number (0,1,2 -> 31,30,29)
       *  as compared to the first version. Used in 2006 September testbeam.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   2005-01-12
       */

      class RichZSPacked
      {

      public: // definitions

        /// Number of bits for each address
        static const ShortType BitsAddress = 8;
        /// Number of bits for each bit field
        static const ShortType BitsField   = 8;

      private: // definitions

        // shift registers
        static const ShortType ShiftField0    = 0;
        static const ShortType ShiftAddress0  = ShiftField0   + BitsField;
        static const ShortType ShiftField1    = ShiftAddress0 + BitsAddress;
        static const ShortType ShiftAddress1  = ShiftField1   + BitsField;

        // The masks
        static const LongType  MaskField0   = (LongType) ((1 << BitsField)-1)   << ShiftField0;
        static const LongType  MaskAddress0 = (LongType) ((1 << BitsAddress)-1) << ShiftAddress0;
        static const LongType  MaskField1   = (LongType) ((1 << BitsField)-1)   << ShiftField1;
        static const LongType  MaskAddress1 = (LongType) ((1 << BitsAddress)-1) << ShiftAddress1;

        // the max value of address and bit-field storable
        static const ShortType MaxField     = ( 1 << BitsField   ) - 1;
        static const ShortType MaxAddress   = ( 1 << BitsAddress ) - 1;

      public: // methods

        /// Copy Constructor
        RichZSPacked( const RichZSPacked & word )
          : m_data( word.data() ), m_aliceMode( word.aliceMode() ) {}

        /// Constructor from LongType
        RichZSPacked( const LongType data = 0,
                      const bool aliceMode = false ) : m_data( data ), m_aliceMode( aliceMode ) { }

        /// Constructor from address and field values
        RichZSPacked( const ShortType address0 ,  ///< First address to store
                      const ShortType field0   ,  ///< First bitfield to store
                      const ShortType address1 ,  ///< Second address to store
                      const ShortType field1   ,  ///< Second bitfield to store
                      const bool aliceMode = false
                      )
          : m_data ( 0 ), m_aliceMode( aliceMode )
        {
          /// &todo Setup ALICE mode encoding properly
          setAddress0  ( address0 );
          setBitField0 ( field0   );
          setAddress1  ( address1 );
          setBitField1 ( field1   );
        }

        /// Destructor
        ~RichZSPacked( ) { }

        /// Retrieve the full value
        inline LongType data() const { return m_data; }

        /// operator to convert to LongType
        inline operator LongType() const { return data(); }

        /// Set the first address
        inline void setAddress0( const ShortType address )
        {
          dataInRange( address, MaxAddress );
          set( address, ShiftAddress0, MaskAddress0 );
        }

        /// Set the second address
        inline void setAddress1( const ShortType address )
        {
          dataInRange( address, MaxAddress );
          set( address, ShiftAddress1, MaskAddress1 );
        }

        /// Set the first bit-field
        inline void setBitField0( const ShortType field )
        {
          dataInRange( field, MaxField );
          set( field, ShiftField0, MaskField0 );
        }

        /// Set the second bit-field
        inline void setBitField1( const ShortType field )
        {
          dataInRange( field, MaxField );
          set( field, ShiftField1, MaskField1 );
        }

        /// Retrieve the first address
        inline ShortType address0() const
        {
          return ( (data() & MaskAddress0) >> ShiftAddress0 );
        }

        /// Retrieve the first address
        inline ShortType address1() const
        {
          return ( (data() & MaskAddress1) >> ShiftAddress1 );
        }

        /// Retrieve the first bit-field
        inline ShortType bitField0() const
        {
          return ( (data() & MaskField0) >> ShiftField0 );
        }

        /// Retrieve the second bit-field
        inline ShortType bitField1() const
        {
          return ( (data() & MaskField1) >> ShiftField1 );
        }

        /// Is this in alice mode or not ?
        inline bool aliceMode() const { return m_aliceMode; }

      public:

        /** Get address from row and column information
         *  @param row LHCb pixel row number (0-31)
         *  @param col LHCb pixel col number (0-31)
         *  @param subPix ALICE sub-pixel number, within an LHCb pixel (0-7)
         *  @return The address for the 8-bit block for the given row, col and subPix
         */
        inline ShortType addressFromRowCol( const ShortType row,
                                            const ShortType col,
                                            const ShortType subPix = 0 ) const
        {
          return ( !aliceMode() ? 
                   ( (MaxDataSize-1-row)*4 + col/BitsField ) :
                   ( 4*(NumAlicePixelsPerLHCbPixel-1-subPix) +
                     4*(MaxDataSize-1-row)*NumAlicePixelsPerLHCbPixel +
                     col/BitsField ) % (1+MaxZSAddress)
                   );
        }

        /** Get bit number from column information
         *  @param col The LHCb pixel column number
         *  @return the bit (0-7) for the column
         */
        inline ShortType bitFromCol( const ShortType col ) const
        {
          return col%BitsField;
        }

        /** Get column information from address and bit number
         *  @param address The 8-bit address
         *  @param bit     The bit (0-7)
         *  @return The LHCb pixel column number
         */
        inline ShortType colFromAddressAndBit( const ShortType address,
                                               const ShortType bit ) const
        {
          return bit + BitsField*(address%4);
        }

        /** Get row information from address
         *  @param address The 8-bit address
         *  @param aliceModeOffset Offset for ALICE mode
         *  @return The LHCB pixel row number
         */
        inline ShortType rowFromAddress( const ShortType address,
                                         const ShortType aliceModeOffset = 0 ) const
        {
          return ( !aliceMode() ?
                   ( MaxDataSize - 1 - ( address/4) ) :
                   ( MaxDataSize - 1 - ( (address/(4*NumAlicePixelsPerLHCbPixel)) + aliceModeOffset*8 ) )
                   );
        }

        /** get the ALICE sub-pixel number (0-7) from address
         *  @param address The 8-bit address
         *  @return The ALICE sub-pixel number
         */
        inline ShortType subPixelFromAddress( const ShortType address ) const
        {
          return ( !aliceMode() ? 0 :
                   NumAlicePixelsPerLHCbPixel - 1 - ( address % (4*NumAlicePixelsPerLHCbPixel) ) / 4
                   );
        }

      private: // methods

        /// Update the internal data
        inline void setData( const LongType data ) { m_data = data; }

        /// Set the data value for a given mask and shift value
        inline void set( const ShortType value,
                         const ShortType shift,
                         const LongType  mask )
        {
          setData( ((value << shift) & mask) | (data() & ~mask) );
        }

        /// tests whether a given value is in range for a given data field
        inline void dataInRange( const ShortType value,
                                 const ShortType max ) const
        {
          if ( value > max ) 
          {
            throw GaudiException( "Data out of range", "*RichZSPacked*", StatusCode::FAILURE );
          }
        }

      private: // data

        /// The data word
        LongType m_data;

        /// Alice mode flag
        bool m_aliceMode;

      };

    }

  }
}

#endif // RICHDAQ_RICHZSPACKED_V2_H
