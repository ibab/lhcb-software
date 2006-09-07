
//-----------------------------------------------------------------------------
/** @file RichNonZeroSuppData_V2.h
 *
 *  Header file for RICH DAQ utility class : RichNonZeroSuppData
 *
 *  CVS Log :-
 *  $Id: RichNonZeroSuppData_V2.h,v 1.1 2006-09-07 17:14:11 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-07
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHNONZEROSUPPDATA_V2_H
#define RICHDAQ_RICHNONZEROSUPPDATA_V2_H 1

// local
#include "RichHPDDataBank.h"

// RichKernel
#include "RichKernel/BoostMemPoolAlloc.h"

//===================================================================================

/** @namespace RichNonZeroSuppDataV2
 *
 *  Namespace for version 2 of the RichNonZeroSuppData object.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
namespace RichNonZeroSuppDataV2
{

  /** @class RichNonZeroSuppData RichNonZeroSuppData_v2.h
   *
   *  The RICH HPD non zero suppressed data format.
   *  Second iteration of the format. Identical to version 1
   *  apart from reversing the order the rows are encoded/decoded.
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-07
   */
  template< class Version, class Header, class Footer >
  class RichNonZeroSuppData : public RichHPDDataBankImp<Version,Header,Footer>,
                              public Rich::BoostMemPoolAlloc<RichNonZeroSuppDataV2::RichNonZeroSuppData<Version,Header,Footer> >
  {

  public:

    /// Default constructor
    RichNonZeroSuppData()
      : RichHPDDataBankImp<Version,Header,Footer>( 0,
                                                   Header(),
                                                   Footer(),
                                                   RichDAQ::MaxDataSize ),
        m_nHits ( -1 )
    { }

    /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
     *
     *  @param l0ID   L0 board hardware identifier
     *  @param digits Vector of RichSmartIDs listing the active channels in this HPD
     */
    explicit RichNonZeroSuppData( const RichDAQ::Level0ID l0ID,
                                  const LHCb::RichSmartID::Vector & digits )
      : RichHPDDataBankImp<Version,Header,Footer> ( Header( false, // Not ZS
                                                            false, // Not ALICE mode
                                                            l0ID, (RichDAQ::MaxDataSize*2)+1 // needs to be updated
                                                            ),
                                                    Footer(),
                                                    0, RichDAQ::MaxDataSize, RichDAQ::MaxDataSize ),
        m_nHits ( -1 )
    {
      for ( LHCb::RichSmartID::Vector::const_iterator iDig = digits.begin();
            iDig != digits.end(); ++ iDig )
      {
        setPixelActive( (*iDig).pixelRow(), (*iDig).pixelCol() );
      }
    }

    /** Constructor from a block of raw data
     *
     *  @param data Pointer to the start of the data block
     */
    explicit RichNonZeroSuppData( const RichDAQ::LongType * data )
      : RichHPDDataBankImp<Version,Header,Footer> ( data, // start of data
                                                    Header(),  
                                                    Footer(),  
                                                    RichDAQ::MaxDataSize // max data bloxk size
                                                    ),
        m_nHits ( -1 )
    { }

    /// Destructor
    ~RichNonZeroSuppData() { }

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
      setBit( m_data[maxDataSize()-(row+1)], col );
    }

    /// Is a given pixel active ?
    inline bool isPixelActive( const RichDAQ::ShortType row,
                               const RichDAQ::ShortType col ) const
    {
      return isBitOn( m_data[maxDataSize()-(row+1)], col );
    }

  private: // data

    /// Decoded number of hits
    mutable int m_nHits;

  };

} // RichNonZeroSuppDataV2 namespace

#endif // RICHDAQ_RICHNONZEROSUPPDATA_V2_H
