
/** @file RichZSPacked_V1.h
 *
 *  Header file for RICH DAQ utility class : Rich::DAQ::RichZSPacked_V1::RichZSPacked
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

#ifndef RICHDAQ_RICHZSPACKED_V1_H
#define RICHDAQ_RICHZSPACKED_V1_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// numberings
#include "RichKernel/RichDAQDefinitions.h"

namespace Rich
{
  namespace DAQ
  {

    /** @namespace Rich::DAQ::RichZSPacked_V1
     *
     *  Namespace for first version of LHCb mode ZS compressed data words
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   2005-01-12
     */
    namespace RichZSPacked_V1
    {

      /** @namespace Rich::DAQ::RichZSPacked_V1::RichZSPackedCode
       *
       *  Namespace for definitions related to RichZSPacked_V1::RichZSPacked
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       *  @date   2005-01-12
       */
      namespace RichZSPackedCode
      {

        /// Number of bits for each address
        static const ShortType BitsAddress = 7;
        /// Number of bits for each bit field
        static const ShortType BitsField   = 8;

        // shift registers
        static const ShortType ShiftField0    = 0;
        static const ShortType ShiftAddress0  = ShiftField0 + BitsField;
        static const ShortType ShiftField1    = ShiftAddress0 + 1 + BitsAddress;
        static const ShortType ShiftAddress1  = ShiftField1 + BitsField;

        // The masks
        static const LongType  MaskField0   = (LongType) ((1 << BitsField)-1)   << ShiftField0;
        static const LongType  MaskAddress0 = (LongType) ((1 << BitsAddress)-1) << ShiftAddress0;
        static const LongType  MaskField1   = (LongType) ((1 << BitsField)-1)   << ShiftField1;
        static const LongType  MaskAddress1 = (LongType) ((1 << BitsAddress)-1) << ShiftAddress1;

        // the max value of address and bit-field storable
        static const ShortType MaxField     = ( 1 << BitsField   ) - 1;
        static const ShortType MaxAddress   = ( 1 << BitsAddress ) - 1;

      }

      /** @class RichZSPacked RichZSPacked_V1.h
       *
       *  Utility class representing 2 groups of address and
       *  bit-field packed into a single word for LHCb mode readout ONLY
       *
       *  First version for LHCb mode data with ZS compression
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   2005-01-12
       */

      class RichZSPacked 
      {

      public: // methods

        /// Copy Constructor
        RichZSPacked( const RichZSPacked & word ) : m_data( word.data() ) {}

        /// Constructor from Rich::DAQ::LongType
        RichZSPacked( const Rich::DAQ::LongType data = 0 ) : m_data( data ) { }

        /// Constructor from address and field values
        RichZSPacked( const Rich::DAQ::ShortType address0 ,  ///< First address to store
                      const Rich::DAQ::ShortType field0   ,  ///< First bitfield to store
                      const Rich::DAQ::ShortType address1 ,  ///< Second address to store
                      const Rich::DAQ::ShortType field1      ///< Second bitfield to store
                      )
          : m_data ( 0 )
        {
          setAddress0  ( address0 );
          setBitField0 ( field0   );
          setAddress1  ( address1 );
          setBitField1 ( field1   );
        }

        /// Destructor
        ~RichZSPacked( ) { }

        /// Retrieve the full value
        inline Rich::DAQ::LongType data() const { return m_data; }

        /// operator to convert to Rich::DAQ::LongType
        inline operator Rich::DAQ::LongType() const { return data(); }

        /// Set the first address
        inline void setAddress0( const Rich::DAQ::ShortType address )
        {
          dataInRange( address, RichZSPackedCode::MaxAddress );
          set( address, RichZSPackedCode::ShiftAddress0, RichZSPackedCode::MaskAddress0 );
        }

        /// Set the second address
        inline void setAddress1( const Rich::DAQ::ShortType address )
        {
          dataInRange( address, RichZSPackedCode::MaxAddress );
          set( address, RichZSPackedCode::ShiftAddress1, RichZSPackedCode::MaskAddress1 );
        }

        /// Set the first bit-field
        inline void setBitField0( const Rich::DAQ::ShortType field )
        {
          dataInRange( field, RichZSPackedCode::MaxField );
          set( field, RichZSPackedCode::ShiftField0, RichZSPackedCode::MaskField0 );
        }

        /// Set the second bit-field
        inline void setBitField1( const Rich::DAQ::ShortType field )
        {
          dataInRange( field, RichZSPackedCode::MaxField );
          set( field, RichZSPackedCode::ShiftField1, RichZSPackedCode::MaskField1 );
        }

        /// Retrieve the first address
        inline Rich::DAQ::ShortType address0() const
        {
          return ( (data() & RichZSPackedCode::MaskAddress0) >> RichZSPackedCode::ShiftAddress0 );
        }

        /// Retrieve the first address
        inline Rich::DAQ::ShortType address1() const
        {
          return ( (data() & RichZSPackedCode::MaskAddress1) >> RichZSPackedCode::ShiftAddress1 );
        }

        /// Retrieve the first bit-field
        inline Rich::DAQ::ShortType bitField0() const
        {
          return ( (data() & RichZSPackedCode::MaskField0) >> RichZSPackedCode::ShiftField0 );
        }

        /// Retrieve the second bit-field
        inline Rich::DAQ::ShortType bitField1() const
        {
          return ( (data() & RichZSPackedCode::MaskField1) >> RichZSPackedCode::ShiftField1 );
        }

      public:

        /// Get address from row and column information
        inline static Rich::DAQ::ShortType addressFromRowCol( const Rich::DAQ::ShortType row,
                                                              const Rich::DAQ::ShortType col )
        {
          return ( row*4 + col/RichZSPackedCode::BitsField );
        }

        /// Get bit number from column information
        inline static Rich::DAQ::ShortType bitFromCol( const Rich::DAQ::ShortType col )
        {
          return col%RichZSPackedCode::BitsField;
        }

        /// Get column information from address and bit number
        inline static Rich::DAQ::ShortType colFromAddressAndBit( const Rich::DAQ::ShortType address,
                                                                 const Rich::DAQ::ShortType bit )
        {
          return bit + RichZSPackedCode::BitsField*(address%4);
        }

        /// Get row information from address
        inline static Rich::DAQ::ShortType rowFromAddress( const Rich::DAQ::ShortType address )
        {
          return address/4;
        }

      private: // methods

        /// Update the internal data
        inline void setData( const Rich::DAQ::LongType data ) { m_data = data; }

        /// Set the data value for a given mask and shift value
        inline void set( const Rich::DAQ::ShortType value,
                         const Rich::DAQ::ShortType shift,
                         const Rich::DAQ::LongType  mask )
        {
          setData( ((value << shift) & mask) | (data() & ~mask) );
        }

        /// tests whether a given value is in range for a given data field
        inline void dataInRange( const Rich::DAQ::ShortType value,
                                 const Rich::DAQ::ShortType max ) const
        {
          if ( value > max ) throw GaudiException( "Data out of range", "*RichZSPacked*", StatusCode::FAILURE );
        }

      private: // data

        /// The data word
        Rich::DAQ::LongType m_data = 0;

      };

    }

  }
}

#endif // RICHDAQ_RICHZSPACKED_V1_H
