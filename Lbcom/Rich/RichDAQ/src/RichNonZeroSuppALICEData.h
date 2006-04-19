
//-----------------------------------------------------------------------------
/** @file RichNonZeroSuppALICEData.h
 *
 *  Header file for RICH DAQ utility class : RichNonZeroSuppALICEData
 *
 *  CVS Log :-
 *  $Id: RichNonZeroSuppALICEData.h,v 1.2 2006-04-19 17:05:04 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-07
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHNONZEROSUPPALICEDATA_H
#define RICHDAQ_RICHNONZEROSUPPALICEDATA_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// local
#include "RichHPDDataBank.h"
#include "RichDAQHPDIdentifier.h"
#include "RichDAQHeaderPD.h"

// RichKernel
#include "RichKernel/BoostMemPoolAlloc.h"

//===================================================================================

/** @namespace RichNonZeroSuppALICEDataV1
 *
 *  Namespace for version 1 of the RichNonZeroSuppALICEData object.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
namespace RichNonZeroSuppALICEDataV1
{

  /** @class RichNonZeroSuppALICEData RichNonZeroSuppALICEData.h
   *
   *  The RICH HPD non zero suppressed data format for ALICE mode.
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-07
   */
  class RichNonZeroSuppALICEData : public RichHPDDataBank,
                                   public Rich::BoostMemPoolAlloc<RichNonZeroSuppALICEDataV1::RichNonZeroSuppALICEData>
  {

  public: // Definitions

    /// Typedef for the header type for this data bank implementation
    typedef RichDAQHeaderV1::RichDAQHeaderPD Header;

  public:

    /// Default constructor
    RichNonZeroSuppALICEData() : 
      RichHPDDataBank( 0, RichDAQ::MaxDataSizeALICE, 0, RichDAQ::MaxDataSizeALICE ) { }

    /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
     *
     *  @param l0ID   L0 board hardware identifier
     *  @param digits Vector of RichSmartIDs listing the active channels in this HPD
     */
    explicit RichNonZeroSuppALICEData( const RichDAQ::Level0ID l0ID,
                                       const LHCb::RichSmartID::Vector & digits )
      : RichHPDDataBank ( Header( false, l0ID, digits.size() ),
                          RichDAQ::MaxDataSizeALICE, 0, RichDAQ::MaxDataSizeALICE )
    {
      for ( LHCb::RichSmartID::Vector::const_iterator iDig = digits.begin();
            iDig != digits.end(); ++ iDig )
      {
        const RichDAQ::ShortType row = 8*(*iDig).pixelRow() + (*iDig).pixelSubRow();
        setPixelActive( row, (*iDig).pixelCol() );
      }
    }

    /** Constructor from a block of raw data
     *
     *  @param data Pointer to the start of the data block
     */
    explicit RichNonZeroSuppALICEData( const RichDAQ::LongType * data )
      : RichHPDDataBank ( data, RichDAQ::MaxDataSizeALICE, RichDAQ::MaxDataSizeALICE ) { }

    /// Destructor
    ~RichNonZeroSuppALICEData() { }

    // Returns the L0ID
    virtual RichDAQ::Level0ID level0ID() const;

    // Returns the hit count for this HPD
    virtual RichDAQ::ShortType hitCount() const;

    // Fill a vector with RichSmartIDs for hit pixels
    virtual void fillRichSmartIDs( LHCb::RichSmartID::Vector & ids,
                                   const LHCb::RichSmartID hpdID ) const;

  protected: // methods

    // Print data bank to messager stream
    virtual void fillMsgStream( MsgStream & os ) const;

  private: // methods

    /// Set a pixel as active
    inline void setPixelActive( const RichDAQ::ShortType row,
                                const RichDAQ::ShortType col )
    {
      setBit( m_data[row], col );
    }

    /// Is a given pixel active ?
    inline bool isPixelActive( const RichDAQ::ShortType row,
                               const RichDAQ::ShortType col ) const
    {
      return isBitOn( m_data[row], col );
    }

  };

} // RichNonZeroSuppALICEDataV1 namespace

#endif // RICHDAQ_RICHNONZEROSUPPALICEDATA_H
