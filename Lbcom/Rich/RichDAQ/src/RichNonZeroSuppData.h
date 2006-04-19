
//-----------------------------------------------------------------------------
/** @file RichNonZeroSuppData.h
 *
 *  Header file for RICH DAQ utility class : RichNonZeroSuppData
 *
 *  CVS Log :-
 *  $Id: RichNonZeroSuppData.h,v 1.21 2006-04-19 17:05:05 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-07
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHNONZEROSUPPDATA_H
#define RICHDAQ_RICHNONZEROSUPPDATA_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// local
#include "RichHPDDataBank.h"
#include "RichDAQHPDIdentifier.h"
#include "RichDAQHeaderPD.h"

// RichKernel
#include "RichKernel/BoostMemPoolAlloc.h"

//===================================================================================

/** @namespace RichNonZeroSuppDataV1
 *
 *  Namespace for version 1 of the RichNonZeroSuppData object.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
namespace RichNonZeroSuppDataV1
{

  /** @class RichNonZeroSuppData RichNonZeroSuppData.h
   *
   *  The RICH HPD non zero suppressed data format.
   *  Second iteration of the format. Identical to version 0
   *  apart from a new header word format.
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-07
   */
  class RichNonZeroSuppData : public RichHPDDataBank,
                              public Rich::BoostMemPoolAlloc<RichNonZeroSuppDataV1::RichNonZeroSuppData>
  {

  public: // Definitions

    /// Typedef for the header type for this data bank implementation
    typedef RichDAQHeaderV1::RichDAQHeaderPD Header;

  public:

    /// Default constructor
    RichNonZeroSuppData() 
      : RichHPDDataBank( 0, RichDAQ::MaxDataSize, 0, RichDAQ::MaxDataSize ) { }

    /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
     *
     *  @param l0ID   L0 board hardware identifier
     *  @param digits Vector of RichSmartIDs listing the active channels in this HPD
     */
    explicit RichNonZeroSuppData( const RichDAQ::Level0ID l0ID,
                                  const LHCb::RichSmartID::Vector & digits )
      : RichHPDDataBank ( Header( false, l0ID, digits.size() ),
                          RichDAQ::MaxDataSize, 0, RichDAQ::MaxDataSize )
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
      : RichHPDDataBank ( data, RichDAQ::MaxDataSize, RichDAQ::MaxDataSize ) { }

    /// Destructor
    ~RichNonZeroSuppData() { }

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

} // RichNonZeroSuppDataV1 namespace

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

  /** @class RichNonZeroSuppData RichNonZeroSuppData.h
   *
   *  The RICH HPD non zero suppressed data format.
   *  Second iteration of the format. Identical to version 1
   *  apart from reversing the order the rows are encoded/decoded.
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-07
   */
  class RichNonZeroSuppData : public RichHPDDataBank,
                              public Rich::BoostMemPoolAlloc<RichNonZeroSuppDataV2::RichNonZeroSuppData>
  {

  public: // Definitions

    /// Typedef for the header type for this data bank implementation
    typedef RichDAQHeaderV1::RichDAQHeaderPD Header;

  public:

    /// Default constructor
    RichNonZeroSuppData() 
      : RichHPDDataBank( 0, RichDAQ::MaxDataSize, 0, RichDAQ::MaxDataSize ) { }

    /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
     *
     *  @param l0ID   L0 board hardware identifier
     *  @param digits Vector of RichSmartIDs listing the active channels in this HPD
     */
    explicit RichNonZeroSuppData( const RichDAQ::Level0ID l0ID,
                                  const LHCb::RichSmartID::Vector & digits )
      : RichHPDDataBank ( Header( false, l0ID, digits.size() ),
                          RichDAQ::MaxDataSize, 0, RichDAQ::MaxDataSize )
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
      : RichHPDDataBank ( data, RichDAQ::MaxDataSize, RichDAQ::MaxDataSize ) { }

    /// Destructor
    ~RichNonZeroSuppData() { }

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

    // Dump the raw header and data block to message stream
    virtual void dumpAllBits( MsgStream & os ) const;

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

  };

} // RichNonZeroSuppDataV2 namespace

#endif // RICHDAQ_RICHNONZEROSUPPDATA_H
