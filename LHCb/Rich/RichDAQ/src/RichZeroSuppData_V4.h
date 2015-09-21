
//-----------------------------------------------------------------------------
/** @file RichZeroSuppData_V4.h
 *
 *  Header file for RICH DAQ utility class : RichZeroSuppData
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHZEROSUPPDATA_V4_H
#define RICHDAQ_RICHZEROSUPPDATA_V4_H 1

// local
#include "RichHPDDataBank.h"
#include "RichZSPacked_V2.h"

// RichKernel
#include "Kernel/MemPoolAlloc.h"

// Event Model
#include "Event/ODIN.h"

namespace Rich
{
  namespace DAQ
  {

    /** @namespace Rich::DAQ::RichZeroSuppDataV4
     *
     *  Namespace for version 4 of the RichZeroSuppData object.
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   2004-12-17
     */
    namespace RichZeroSuppDataV4
    {

      /** @class RichZeroSuppData RichZeroSuppData.h
       *
       *  The RICH HPD zero suppressed data format.
       *  Third iteration of the format using a new packing scheme.
       *
       *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
       *  @date   2003-11-07
       */
      template< class Version, class Header, class Footer >
      class RichZeroSuppData : public HPDDataBankImp<Version,Header,Footer>,
                               public LHCb::MemPoolAlloc<RichZeroSuppDataV4::RichZeroSuppData<Version,Header,Footer> >
      {

      public:

        /// Default constructor
        RichZeroSuppData()
          : HPDDataBankImp<Version,Header,Footer>( MaxDataSize ),
            m_tooBig ( false   ),
            m_nHits  ( -1      )
        { }

        /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
         *
         *  @param l0ID   Level0  board hardware identifier
         *  @param digits Vector of RichSmartIDs listing the active channels in this HPD
         */
        explicit RichZeroSuppData( const Level0ID l0ID,
                                   const LHCb::RichSmartID::Vector & digits,
                                   const bool extendedFormat = false,
                                   const LHCb::ODIN * odin = NULL )
          : HPDDataBankImp<Version,Header,Footer> ( Header ( true,  // Is ZS
                                                             false, // Not ALICE mode
                                                             extendedFormat, // data format
                                                             false, // No GT inhibit
                                                             l0ID,  // L0 ID
                                                             EventID( odin ? odin->eventNumber() : 0 ), // Event ID
                                                             0      // Filled by buildData call below in main body
                                                             ),
                                                    Footer ( ),
                                                    0, MaxDataSize ),
            m_tooBig ( false   ),
            m_nHits  ( -1      )
        {
          buildData( digits );
        }

        /** Constructor from a block of raw data
         *
         *  @param data     Pointer to the start of the data block
         */
        explicit RichZeroSuppData( const LongType * data )
          : HPDDataBankImp<Version,Header,Footer> ( data,        // start of data
                                                    MaxDataSize  // max data block size
                                                    ),
            m_tooBig ( false   ),
            m_nHits  ( -1      )
        { }

        /** Reset for a new block of raw data
         *  @param data     Pointer to the start of the data block
         */
        inline void reset( const LongType * data )
        {
          m_tooBig = false;
          m_nHits  = -1;
          HPDDataBankImp<Version,Header,Footer>::reset( data );
        }

        /// Destructor
        virtual ~RichZeroSuppData() { }

        // Returns the hit count for this HPD
        virtual ShortType hitCount() const;

        // Fill a vector with RichSmartIDs for hit pixels
        virtual ShortType fillRichSmartIDs( LHCb::RichSmartID::Vector & ids,
                                            const LHCb::RichSmartID hpdID ) const;

        // Test if this bank would be too big ( i.e. greater than 32 words )
        inline bool tooBig() const
        {
          return m_tooBig;
        }

      private: // methods

        /// Build data array from vector of RichSmartIDs
        void buildData( const LHCb::RichSmartID::Vector & pdHits );

      private: // data

        /// Too big flag
        bool m_tooBig;

        /// Decoded number of hits
        mutable int m_nHits;

      };

    } // end V4 namespace

  }
}

#endif // RICHDAQ_RICHZEROSUPPDATA_V4_H
