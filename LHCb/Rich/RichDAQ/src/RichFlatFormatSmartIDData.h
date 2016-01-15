
//-----------------------------------------------------------------------------
/** @file RichFlatFormatSmartIDData.h
 *
 *  Header file for RICH DAQ utility class : RichFlatFormatSmartIDData
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-07
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RichFlatFormatSmartIDData_H
#define RICHDAQ_RichFlatFormatSmartIDData_H 1

// local
#include "RichHPDDataBank.h"
#include "RichZSPacked_V2.h"

// Kernel
#include "Kernel/MemPoolAlloc.h"

// Event Model
#include "Event/ODIN.h"

//===================================================================================

namespace Rich
{
  namespace DAQ
  {

    /** @class RichFlatFormatSmartIDData RichFlatFormatSmartIDData.h
     *
     *
     *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
     *  @date   2003-11-07
     */
    template< class Version, class Header, class Footer >
    class RichFlatFormatSmartIDData : public HPDDataBankImp<Version,Header,Footer>,
                                      public LHCb::MemPoolAlloc<RichFlatFormatSmartIDData<Version,Header,Footer> >
    {

    public:

      /// Default constructor
      RichFlatFormatSmartIDData()
        : HPDDataBankImp<Version,Header,Footer>( MaxDataSize )
      { }

      /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
       *
       *  @param l0ID   L0 board hardware identifier
       *  @param digits Vector of RichSmartIDs listing the active channels in this HPD
       *  @param extendedFormat
       *  @param odin   Pointer to the ODIN data object
       */
      explicit RichFlatFormatSmartIDData( const Level0ID l0ID,
                                          const LHCb::RichSmartID::Vector & digits,
                                          const bool extendedFormat = false,
                                          const LHCb::ODIN * odin = NULL )
        : HPDDataBankImp<Version,Header,Footer> ( Header( true,  // Not ZS
                                                          false, // Not ALICE mode
                                                          extendedFormat, // data format
                                                          false, // No GT inhibit
                                                          l0ID,  // The L0 ID
                                                          EventID( odin ? odin->eventNumber() : 0 ), // Event ID
                                                          0 // filled by buildData call below in main body
                                                          ),
                                                  Footer ( ),
                                                  0, MaxDataSize )
      {
        buildData( digits );
      }

      /** Constructor from a block of raw data
       *
       *  @param data Pointer to the start of the data block
       */
      explicit RichFlatFormatSmartIDData( const LongType * data )
        : HPDDataBankImp<Version,Header,Footer> ( data,       // start of data
                                                  MaxDataSize // max data bloxk size
                                                  )
      { }

        /** Reset for a new block of raw data
         *  @param data     Pointer to the start of the data block
         */
        inline void reset( const LongType * data )
        {
          HPDDataBankImp<Version,Header,Footer>::reset( data );
        }

      /// Destructor
      virtual ~RichFlatFormatSmartIDData() { }

      // Returns the hit count for this HPD
      virtual ShortType hitCount() const;

      // Fill a vector with RichSmartIDs for hit pixels
      virtual ShortType fillRichSmartIDs( LHCb::RichSmartID::Vector & ids,
                                          const LHCb::RichSmartID hpdID ) const;

    private: // methods

      /// Build data array from vector of RichSmartIDs
      void buildData( const LHCb::RichSmartID::Vector & digits );

    private: // data

      /// Decoded number of hits
      mutable int m_nHits = -1;

    };

  }
}

#endif // RICHDAQ_RichFlatFormatSmartIDData_H
