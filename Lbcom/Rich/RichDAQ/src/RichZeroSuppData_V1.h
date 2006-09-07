
//-----------------------------------------------------------------------------
/** @file RichZeroSuppData_V1.h
 *
 *  Header file for RICH DAQ utility class : RichZeroSuppData
 *
 *  CVS Log :-
 *  $Id: RichZeroSuppData_V1.h,v 1.1 2006-09-07 17:14:11 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHZEROSUPPDATA_V1_H
#define RICHDAQ_RICHZEROSUPPDATA_V1_H 1

// local
#include "RichHPDDataBank.h"
#include "RichZSHitTriplet.h"

// RichKernel
#include "RichKernel/BoostMemPoolAlloc.h"

// =================================================================================

/** @namespace RichZeroSuppDataV1
 *
 *  Namespace for version 1 of the RichZeroSuppData object.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
namespace RichZeroSuppDataV1
{

  /** @class RichZeroSuppData RichZeroSuppData_V1.h
   *
   *  The RICH HPD non zero suppressed data format.
   *  Second iteration of the format. Identical to version 0
   *  apart from a new header word format.
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-07
   */
  template< class Version, class Header, class Footer >
  class RichZeroSuppData : public RichHPDDataBankImp<Version,Header,Footer>,
                           public Rich::BoostMemPoolAlloc<RichZeroSuppDataV1::RichZeroSuppData<Version,Header,Footer> >
  {

  public:

    /// Default constructor
    RichZeroSuppData() : RichHPDDataBankImp<Version,Header,Footer>( 0,
                                                                    Header(),
                                                                    Footer(),
                                                                    RichDAQ::MaxDataSize ) { }

    /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
     *
     *  @param hpdID  Level0 board Hardware identifier
     *  @param digits Vector of RichSmartIDs listing the active channels in this HPD
     */
    explicit RichZeroSuppData( const RichDAQ::Level0ID l0ID,
                               const LHCb::RichSmartID::Vector & digits )
      : RichHPDDataBankImp<Version,Header,Footer> ( Header ( true, l0ID, digits.size() ),
                                                    Footer ( ),
                                                    0, RichDAQ::MaxDataSize )
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
      : RichHPDDataBankImp<Version,Header,Footer> ( data,      // start of data
                                                    Header(),  
                                                    Footer(), 
                                                    RichDAQ::MaxDataSize,  // max data block size
                                                    dataSize )
    { }

    /// Destructor
    virtual ~RichZeroSuppData() { }

    // Returns the hit count for this HPD
    virtual RichDAQ::ShortType hitCount() const;

    // Fill a vector with RichSmartIDs for hit pixels
    virtual RichDAQ::ShortType fillRichSmartIDs( LHCb::RichSmartID::Vector & ids,
                                                 const LHCb::RichSmartID hpdID ) const;

  private: // methods

    /// Build data array from vector of RichSmartIDs
    void buildData( const LHCb::RichSmartID::Vector & pdHits );

  };

} // end V1 namespace

// =================================================================================



#endif // RICHDAQ_RICHZEROSUPPDATA_V1_H
