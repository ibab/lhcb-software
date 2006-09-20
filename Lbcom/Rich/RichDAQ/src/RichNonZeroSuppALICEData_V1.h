
//-----------------------------------------------------------------------------
/** @file RichNonZeroSuppALICEData_V1.h
 *
 *  Header file for RICH DAQ utility class : RichNonZeroSuppALICEData
 *
 *  CVS Log :-
 *  $Id: RichNonZeroSuppALICEData_V1.h,v 1.3 2006-09-20 13:07:13 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-07
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RichNonZeroSuppALICEDataV1_H
#define RICHDAQ_RichNonZeroSuppALICEDataV1_H 1

// local
#include "RichHPDDataBank.h"

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

  /** @class RichNonZeroSuppALICEData RichNonZeroSuppALICEData_V1.h
   *
   *  The RICH HPD non zero suppressed data format for ALICE mode.
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-07
   */
  template< class Version, class Header, class Footer >
  class RichNonZeroSuppALICEData : public RichHPDDataBankImp<Version,Header,Footer>,
                                   public Rich::BoostMemPoolAlloc< RichNonZeroSuppALICEDataV1::RichNonZeroSuppALICEData<Version,Header,Footer> >
  {

  public:

    /// Default constructor
    RichNonZeroSuppALICEData() :
      RichHPDDataBankImp<Version,Header,Footer> ( 0,
                                                  Header(),
                                                  Footer(),
                                                  RichDAQ::MaxDataSizeALICE ),
      m_nHits(-1)
    { }

    /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
     *
     *  @param l0ID   L0 board hardware identifier
     *  @param digits Vector of RichSmartIDs listing the active channels in this HPD
     */
    explicit RichNonZeroSuppALICEData( const RichDAQ::Level0ID l0ID,
                                       const LHCb::RichSmartID::Vector & digits )
      : RichHPDDataBankImp<Version,Header,Footer> ( Header ( false, // Non-ZS
                                                             true,  // Is ALICE mode
                                                             l0ID,
                                                             (RichDAQ::MaxDataSizeALICE*2)+1 // Needs to be updated
                                                             ),
                                                    Footer(),
                                                    0, RichDAQ::MaxDataSizeALICE ),
        m_nHits ( -1 )
    {
      // Set data words
      for ( LHCb::RichSmartID::Vector::const_iterator iDig = digits.begin();
            iDig != digits.end(); ++ iDig )
      {
        const RichDAQ::ShortType row = 8*(*iDig).pixelRow() + (*iDig).pixelSubRow();
        setPixelActive( row, (*iDig).pixelCol() );
      }
      // set footer parity
      if ( this->footer().hasParityWord() )
      {
        Footer foot = this->footer();
        foot.setParityWord( this->createParityWord(digits) );
        this->setFooter(foot);
      }
    }

    /** Constructor from a block of raw data
     *
     *  @param data Pointer to the start of the data block
     */
    explicit RichNonZeroSuppALICEData( const RichDAQ::LongType * data )
      : RichHPDDataBankImp<Version,Header,Footer> ( data, // start of data
                                                    Header(),
                                                    Footer(),
                                                    RichDAQ::MaxDataSizeALICE,
                                                    RichDAQ::MaxDataSizeALICE ),
        m_nHits( -1 )
    { }

    /// Destructor
    ~RichNonZeroSuppALICEData() { }

    // Returns the hit count for this HPD
    virtual RichDAQ::ShortType hitCount() const;

    // Fill a vector with RichSmartIDs for hit pixels
    virtual RichDAQ::ShortType fillRichSmartIDs( LHCb::RichSmartID::Vector & ids,
                                                 const LHCb::RichSmartID hpdID ) const;

  private: // methods

    /// Set a pixel as active
    inline void setPixelActive( const RichDAQ::ShortType row,
                                const RichDAQ::ShortType col )
    {
      setBit( this->data()[this->maxDataSize()-(row+1)], col );
    }

    /// Is a given pixel active ?
    inline bool isPixelActive( const RichDAQ::ShortType row,
                               const RichDAQ::ShortType col ) const
    {
      return isBitOn( this->data()[this->maxDataSize()-(row+1)], col );
    }

  private: // data

    /// Decoded number of hits
    mutable int m_nHits;

  };

} // RichNonZeroSuppALICEDataV1 namespace

#endif // RICHDAQ_RichNonZeroSuppALICEDataV1_H
