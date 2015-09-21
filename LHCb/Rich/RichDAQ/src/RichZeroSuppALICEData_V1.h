
//-----------------------------------------------------------------------------
/** @file RichZeroSuppALICEData_V1.h
 *
 *  Header file for RICH DAQ utility class : RichZeroSuppALICEData
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHZEROSUPPALICEDATA_V1_H
#define RICHDAQ_RICHZEROSUPPALICEDATA_V1_H 1

// local
#include "RichHPDDataBank.h"
#include "RichZSPacked_V2.h"

// Kernel
#include "Kernel/MemPoolAlloc.h"

// Event Model
#include "Event/ODIN.h"

namespace Rich
{
  namespace DAQ
  {

    /** @namespace Rich::DAQ::RichZeroSuppALICEDataV1
     *
     *  Namespace for version 4 of the RichZeroSuppALICEData object.
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   2004-12-17
     */
    namespace RichZeroSuppALICEDataV1
    {

      /** @class RichZeroSuppALICEData RichZeroSuppALICEData.h
       *
       *  The RICH HPD zero suppressed data format.
       *  Third iteration of the format using a new packing scheme.
       *
       *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
       *  @date   2003-11-07
       */
      template< class Version, class Header, class Footer >
      class RichZeroSuppALICEData : public HPDDataBankImp<Version,Header,Footer>,
                                    public LHCb::MemPoolAlloc<RichZeroSuppALICEDataV1::RichZeroSuppALICEData<Version,Header,Footer> >
      {

      public:

        /// Default constructor
        RichZeroSuppALICEData()
          : HPDDataBankImp<Version,Header,Footer>( MaxDataSize ),
            m_tooBig ( false   ),
            m_nHits  ( -1      )
        { }

        /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
         *
         *  @param l0ID   Level0 board hardware identifier
         *  @param digits Vector of RichSmartIDs listing the active channels in this HPD
         */
        explicit RichZeroSuppALICEData( const Level0ID l0ID,
                                        const LHCb::RichSmartID::Vector & digits,
                                        const bool extendedFormat = false,
                                        const LHCb::ODIN * odin = NULL )
          : HPDDataBankImp<Version,Header,Footer> ( Header ( true,  // Is ZS
                                                             true,  // ALICE mode
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
         *  @param data Pointer to the start of the data block
         */
        explicit RichZeroSuppALICEData( const LongType * data )
          : HPDDataBankImp<Version,Header,Footer> ( data,        // start of data
                                                    MaxDataSize  // max data block size
                                                    ),
            m_tooBig ( false ),
            m_nHits  ( -1    )
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
        virtual ~RichZeroSuppALICEData() { }

        // Returns the hit count for this HPD
        virtual ShortType hitCount() const;

        // Fill a vector with RichSmartIDs for hit pixels
        virtual ShortType fillRichSmartIDs( LHCb::RichSmartID::Vector & ids,
                                            const LHCb::RichSmartID hpdID ) const;

        // Test if this bank would be too big ( i.e. greater than 32 words )
        inline bool tooBig() const { return m_tooBig; }

      private: // methods

        /// Build data array from vector of RichSmartIDs
        void buildData( const LHCb::RichSmartID::Vector & pdHits );

        /// update the address offset
        inline void updateOffset( const int address,
                                  const bool firstWord,
                                  int& last_address,
                                  int& last_offset ) const
        {
          if ( !firstWord )
          {
            if      ( 0 == address ) { ++last_offset; }
            else if ( 1 == address )
            {
              if ( 0 == last_address%2 ) { last_offset = 0; }
              else                       { ++last_offset;   }
            }
          }
          last_address = address;
        }

      private: // data

        /// Too big flag
        bool m_tooBig;

        /// Decoded number of hits
        mutable int m_nHits;

      };

    } // end V1 namespace

  }
}

#endif // RICHDAQ_RICHZEROSUPPALICEDATA_V1_H
