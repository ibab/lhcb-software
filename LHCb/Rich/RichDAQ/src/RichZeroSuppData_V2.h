
//-----------------------------------------------------------------------------
/** @file RichZeroSuppData_V2.h
 *
 *  Header file for RICH DAQ utility class : RichZeroSuppData
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHZEROSUPPDATA_V2_H
#define RICHDAQ_RICHZEROSUPPDATA_V2_H 1

// local
#include "RichHPDDataBank.h"
#include "RichZSPacked_V1.h"

// RichKernel
#include "Kernel/MemPoolAlloc.h"

namespace Rich
{
  namespace DAQ
  {

    /** @namespace Rich::DAQ::RichZeroSuppDataV2
     *
     *  Namespace for version 2 of the RichZeroSuppData object.
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   2004-12-17
     */
    namespace RichZeroSuppDataV2
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
                               public LHCb::MemPoolAlloc<RichZeroSuppDataV2::RichZeroSuppData<Version,Header,Footer> >
      {

      public:

        /// Default constructor
        RichZeroSuppData()
          : HPDDataBankImp<Version,Header,Footer>( MaxDataSize )
        { }

        /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
         *
         *  @param l0ID   Level0 board Rich::DAQ::hardware identifier
         *  @param digits Vector of RichSmartIDs listing the active channels in this HPD
         */
        explicit RichZeroSuppData( const Level0ID l0ID,
                                   const LHCb::RichSmartID::Vector & digits )
          : HPDDataBankImp<Version,Header,Footer> ( Header ( true, l0ID, digits.size() ),
                                                    Footer ( ),
                                                    0, MaxDataSize )
        {
          buildData( digits );
        }

        /** Constructor from a block of raw data
         *
         *  @param data     Pointer to the start of the data block
         *  @param dataSize The size of the data block (excluding header HPD word)
         */
        explicit RichZeroSuppData( const LongType * data,
                                   const ShortType dataSize )
          : HPDDataBankImp<Version,Header,Footer> ( data,          // start of data
                                                    MaxDataSize,  // max data block size
                                                    dataSize )
        { }

         /** Reset for a new block of raw data
         *  @param data     Pointer to the start of the data block
         *  @param dataSize The size of the data block (excluding header HPD word)
         */
        inline void reset( const LongType * data, 
                           const ShortType dataSize )
        {
          m_tooBig = false;
          HPDDataBankImp<Version,Header,Footer>::reset( data, dataSize );
        }

        /// Destructor
        virtual ~RichZeroSuppData() { }

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

      private: // data

        /// Too big flag
        bool m_tooBig = false;

      };

    } // end V2 namespace

  }
}

#endif // RICHDAQ_RICHZEROSUPPDATA_V2_H
