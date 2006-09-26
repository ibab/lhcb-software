
//-----------------------------------------------------------------------------
/** @file RichZeroSuppData_V2.h
 *
 *  Header file for RICH DAQ utility class : RichZeroSuppData
 *
 *  CVS Log :-
 *  $Id: RichZeroSuppData_V2.h,v 1.2 2006-09-26 10:35:00 jonrob Exp $
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
#include "RichKernel/BoostMemPoolAlloc.h"

/** @namespace RichZeroSuppDataV2
 *
 *  Namespace for version 1 of the RichZeroSuppData object.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
namespace RichZeroSuppDataV2
{

  /** @class RichZeroSuppData RichZeroSuppData.h
   *
   *  The RICH HPD non zero suppressed data format.
   *  Third iteration of the format using a new packing scheme.
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-07
   */
  template< class Version, class Header, class Footer >
  class RichZeroSuppData : public RichHPDDataBankImp<Version,Header,Footer>,
                           public Rich::BoostMemPoolAlloc<RichZeroSuppDataV2::RichZeroSuppData<Version,Header,Footer> >
  {

  public:

    /// Default constructor
    RichZeroSuppData()
      : RichHPDDataBankImp<Version,Header,Footer>( 0,
                                                   Header(),
                                                   Footer(),
                                                   RichDAQ::MaxDataSize ),
        m_tooBig        ( false   ) { }

    /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
     *
     *  @param hpdID  Level0 board hardware identifier
     *  @param digits Vector of RichSmartIDs listing the active channels in this HPD
     */
    explicit RichZeroSuppData( const RichDAQ::Level0ID l0ID,
                               const LHCb::RichSmartID::Vector & digits )
      : RichHPDDataBankImp<Version,Header,Footer> ( Header ( true, l0ID, digits.size() ),
                                                    Footer ( ),
                                                    0, RichDAQ::MaxDataSize ),
        m_tooBig        ( false   )
    {
      buildData( digits );
    }

    /** Constructor from a block of raw data
     *
     *  @param data     Pointer to the start of the data block
     *  @param dataSize The size of the data block (excluding header HPD word)
     */
    explicit RichZeroSuppData( const RichDAQ::LongType * data,
                               const RichDAQ::ShortType dataSize )
      : RichHPDDataBankImp<Version,Header,Footer> ( data,          // start of data
                                                    Header(),   
                                                    Footer(),  
                                                    RichDAQ::MaxDataSize,  // max data block size
                                                    dataSize ),
        m_tooBig        ( false          )
    { }

    /// Destructor
    virtual ~RichZeroSuppData() { }

    // Returns the hit count for this HPD
    virtual RichDAQ::ShortType hitCount() const;

    // Fill a vector with RichSmartIDs for hit pixels
    virtual RichDAQ::ShortType fillRichSmartIDs( LHCb::RichSmartID::Vector & ids,
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

  };

} // end V2 namespace

#endif // RICHDAQ_RICHZEROSUPPDATA_V2_H
