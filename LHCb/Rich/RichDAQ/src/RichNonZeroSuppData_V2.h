
//-----------------------------------------------------------------------------
/** @file RichNonZeroSuppData_V2.h
 *
 *  Header file for RICH DAQ utility class : RichNonZeroSuppData
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-07
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHNONZEROSUPPDATA_V2_H
#define RICHDAQ_RICHNONZEROSUPPDATA_V2_H 1

// local
#include "RichHPDDataBank.h"
#include "RichZSPacked_V2.h"

// RichKernel
#include "Kernel/MemPoolAlloc.h"

//===================================================================================

namespace Rich
{
  namespace DAQ
  {

    /** @namespace Rich::DAQ::RichNonZeroSuppDataV2
     *
     *  Namespace for version 2 of the RichNonZeroSuppData object.
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   2004-12-17
     */
    namespace RichNonZeroSuppDataV2
    {

      /** @class RichNonZeroSuppData RichNonZeroSuppData_V2.h
       *
       *  The RICH HPD non zero suppressed data format.
       *  Second iteration of the format. Identical to version 1
       *  apart from reversing the order the rows are encoded/decoded.
       *
       *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
       *  @date   2003-11-07
       *
       *  @todo Update 8 bit words + 1 value
       */
      template< class Version, class Header, class Footer >
      class RichNonZeroSuppData : public HPDDataBankImp<Version,Header,Footer>,
                                  public LHCb::MemPoolAlloc<RichNonZeroSuppDataV2::RichNonZeroSuppData<Version,Header,Footer> >
      {

      public:

        /// Default constructor
        RichNonZeroSuppData()
          : HPDDataBankImp<Version,Header,Footer>( MaxDataSize )
        { }

        /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
         *
         *  @param l0ID   L0 board hardware identifier
         *  @param digits Vector of RichSmartIDs listing the active channels in this HPD
         */
        explicit RichNonZeroSuppData( const Level0ID l0ID,
                                      const LHCb::RichSmartID::Vector & digits )
          : HPDDataBankImp<Version,Header,Footer> ( Header( false, // Not ZS
                                                            false, // Not ALICE mode
                                                            l0ID,
                                                            0 // filled by buildData call
                                                            ),
                                                    Footer(),
                                                    0, MaxDataSize, MaxDataSize )
        {
          buildData( digits );
        }

        /** Constructor from a block of raw data
         *
         *  @param data Pointer to the start of the data block
         */
        explicit RichNonZeroSuppData( const LongType * data )
          : HPDDataBankImp<Version,Header,Footer> ( data, // start of data
                                                    MaxDataSize // max data bloxk size
                                                    )
        { }

        /** Reset for a new block of raw data
         *  @param data     Pointer to the start of the data block
         */
        inline void reset( const LongType * data )
        {
          m_nHits = -1;
          HPDDataBankImp<Version,Header,Footer>::reset( data );
        }

        /// Destructor
        ~RichNonZeroSuppData() { }

        // Returns the hit count for this HPD
        virtual ShortType hitCount() const;

        // Fill a vector with RichSmartIDs for hit pixels
        virtual ShortType fillRichSmartIDs( LHCb::RichSmartID::Vector & ids,
                                            const LHCb::RichSmartID hpdID ) const;

      private: // methods

        /// Build data array from vector of RichSmartIDs
        void buildData( const LHCb::RichSmartID::Vector & digits );

        /// Calculates number of 8-bit words in the data
        ShortType calcEightBitword( const LHCb::RichSmartID::Vector & digits ) const;

        /// Set a pixel as active
        inline void setPixelActive( const ShortType row,
                                    const ShortType col )
        {
          this -> setBit( this->data()[this->maxDataSize()-(row+1)], col );
        }

        /// Is a given pixel active ?
        inline bool isPixelActive( const ShortType row,
                                   const ShortType col ) const
        {
          return this -> isBitOn( this->data()[this->maxDataSize()-(row+1)], col );
        }

      private: // data

        /// Decoded number of hits
        mutable int m_nHits = -1;

      };

    } // RichNonZeroSuppDataV2 namespace

  }
}

#endif // RICHDAQ_RICHNONZEROSUPPDATA_V2_H
