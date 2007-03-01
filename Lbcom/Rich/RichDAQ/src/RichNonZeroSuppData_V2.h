
//-----------------------------------------------------------------------------
/** @file RichNonZeroSuppData_V2.h
 *
 *  Header file for RICH DAQ utility class : RichNonZeroSuppData
 *
 *  CVS Log :-
 *  $Id: RichNonZeroSuppData_V2.h,v 1.7 2007-03-01 19:39:07 jonrob Exp $
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
#include "RichKernel/BoostMemPoolAlloc.h"

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
                                  public Rich::BoostMemPoolAlloc<RichNonZeroSuppDataV2::RichNonZeroSuppData<Version,Header,Footer> >
      {

      public:

        /// Default constructor
        RichNonZeroSuppData()
          : HPDDataBankImp<Version,Header,Footer>( 0,
                                                   Header(),
                                                   Footer(),
                                                   MaxDataSize ),
            m_nHits ( -1 )
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
                                                    0, MaxDataSize, MaxDataSize ),
            m_nHits ( -1 )
        {
          buildData( digits );
        }

        /** Constructor from a block of raw data
         *
         *  @param data Pointer to the start of the data block
         */
        explicit RichNonZeroSuppData( const LongType * data )
          : HPDDataBankImp<Version,Header,Footer> ( data, // start of data
                                                    Header(),
                                                    Footer(),
                                                    MaxDataSize // max data bloxk size
                                                    ),
            m_nHits ( -1 )
        { }

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
          setBit( this->data()[this->maxDataSize()-(row+1)], col );
        }

        /// Is a given pixel active ?
        inline bool isPixelActive( const ShortType row,
                                   const ShortType col ) const
        {
          return isBitOn( this->data()[this->maxDataSize()-(row+1)], col );
        }

      private: // data

        /// Decoded number of hits
        mutable int m_nHits;

      };

    } // RichNonZeroSuppDataV2 namespace

  }
}

#endif // RICHDAQ_RICHNONZEROSUPPDATA_V2_H
