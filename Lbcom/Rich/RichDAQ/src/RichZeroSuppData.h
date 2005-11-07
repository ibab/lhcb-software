
//-----------------------------------------------------------------------------
/** @file RichZeroSuppData.h
 *
 *  Header file for RICH DAQ utility class : RichZeroSuppData
 *
 *  CVS Log :-
 *  $Id: RichZeroSuppData.h,v 1.4 2005-11-07 11:48:35 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHZEROSUPPDATA_H
#define RICHDAQ_RICHZEROSUPPDATA_H 1

// Event Model
#include "Event/DAQTypes.h"

// local
#include "RichHPDDataBank.h"
#include "RichDAQHeaderPD.h"
#include "RichZSHitTriplet.h"

// RichKernel
#include "RichKernel/BoostMemPoolAlloc.h"

/** @namespace RichZeroSuppDataV0
 *
 *  Namespace for version 0 of the RichZeroSuppData object.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
namespace RichZeroSuppDataV0 
{

  /** @class RichZeroSuppData RichZeroSuppData.h
   *
   *  The RICH HPD zero suppressed data format.
   *  First version, compatible with DC04 data.
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-07
   */
  class RichZeroSuppData : public RichHPDDataBank,
                           public Rich::BoostMemPoolAlloc<RichZeroSuppDataV0::RichZeroSuppData>
  {

  public: // Definitions

    /// Typedef for the header type for this data bank implementation
    typedef RichDAQHeaderV0::RichDAQHeaderPD Header;

    /// Typedef for HPD identifier for this data bank implementation
    typedef RichDAQHPDIdentifierV0::RichDAQHPDIdentifier HPDID;

  public:

    /// Default constructor
    RichZeroSuppData() : RichHPDDataBank(0,0,0) { }

    /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
     *
     *  @param hpdID  RichSmartID identifying the HPD
     *  @param digits Collection of RichSmartIDs listing the active channels in this HPD
     */
    explicit RichZeroSuppData( const RichSmartID hpdID,
                               const RichSmartID::Collection & digits )
      : RichHPDDataBank ( Header( true, HPDID(hpdID), digits.size() ), 0, 0 )
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
      : RichHPDDataBank ( data, dataSize ) { }

    // Destructor
    virtual ~RichZeroSuppData() { }

    // Fill a vector with RichSmartIDs for hit pixels
    virtual void fillRichSmartIDs( RichSmartID::Collection & ids,
                                   const IRichHPDInfoTool * hpdTool ) const;

    // Print data bank to message stream
    virtual void fillMsgStream( MsgStream & os ) const;

  private: // methods

    /// Build data array from vector of RichSmartIDs
    void buildData( const RichSmartID::Collection & pdHits );

  };

} // end V0 namespace

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

  /** @class RichZeroSuppData RichZeroSuppData.h
   *
   *  The RICH HPD non zero suppressed data format.
   *  Second iteration of the format. Identical to version 0
   *  apart from a new header word format.
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-07
   */
  class RichZeroSuppData : public RichHPDDataBank,
                           public Rich::BoostMemPoolAlloc<RichZeroSuppDataV1::RichZeroSuppData>
  {

  public: // Definitions

    /// Typedef for the header type for this data bank implementation
    typedef RichDAQHeaderV1::RichDAQHeaderPD Header;

  public:

    /// Default constructor
    RichZeroSuppData() : RichHPDDataBank(0,0,0) { }

    /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
     *
     *  @param hpdID  Hardware identifier for the HPD
     *  @param digits Collection of RichSmartIDs listing the active channels in this HPD
     */
    explicit RichZeroSuppData( const RichDAQ::HPDHardwareID hpdID,
                               const RichSmartID::Collection & digits )
      : RichHPDDataBank ( Header( true, hpdID, digits.size() ), 0, 0 )
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
      : RichHPDDataBank ( data, dataSize ) { }

    // Destructor
    virtual ~RichZeroSuppData() { }

    // Fill a vector with RichSmartIDs for hit pixels
    virtual void fillRichSmartIDs( RichSmartID::Collection & ids,
                                   const IRichHPDInfoTool * hpdTool ) const;

    // Print data bank to message stream
    virtual void fillMsgStream( MsgStream & os ) const;

  private: // methods

    /// Build data array from vector of RichSmartIDs
    void buildData( const RichSmartID::Collection & pdHits );

  };

} // end V1 namespace

// =================================================================================

#include "RichZSPacked.h"

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
  class RichZeroSuppData : public RichHPDDataBank,
                           public Rich::BoostMemPoolAlloc<RichZeroSuppDataV2::RichZeroSuppData>
{

  public: // Definitions

    /// Typedef for the header type for this data bank implementation
    typedef RichDAQHeaderV1::RichDAQHeaderPD Header;

  public:

    /// Default constructor
    RichZeroSuppData() 
      : RichHPDDataBank ( 0, 0, 0 ), 
        m_tooBig        ( false   ) { }

    /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
     *
     *  @param hpdID  Hardware identifier for the HPD
     *  @param digits Collection of RichSmartIDs listing the active channels in this HPD
     */
    explicit RichZeroSuppData( const RichDAQ::HPDHardwareID hpdID,
                               const RichSmartID::Collection & digits )
      : RichHPDDataBank ( Header( true, hpdID, digits.size() ), 0, 0 ),
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
      : RichHPDDataBank ( data, dataSize ),
        m_tooBig        ( false          )
    { }

    // Destructor
    virtual ~RichZeroSuppData() { }

    // Fill a vector with RichSmartIDs for hit pixels
    virtual void fillRichSmartIDs( RichSmartID::Collection & ids,
                                   const IRichHPDInfoTool * hpdTool ) const;

    // Print data bank to message stream
    virtual void fillMsgStream( MsgStream & os ) const;

    /// Test if this bank would be too big ( i.e. greater than 32 words )
    inline bool tooBig() const
    {
      return m_tooBig;
    }

  private: // methods

    /// Build data array from vector of RichSmartIDs
    void buildData( const RichSmartID::Collection & pdHits );

    /// Get address from row and column information
    inline RichDAQ::ShortType addressFromRowCol( const RichDAQ::ShortType row,
                                                 const RichDAQ::ShortType col ) const
    {
      return ( row*4 + col/RichZSPackedCode::BitsField );
    }
    
    /// Get bit number from column information
    inline RichDAQ::ShortType bitFromCol( const RichDAQ::ShortType col ) const
    {
      return col%RichZSPackedCode::BitsField;
    }

    /// Get column information from address and bit number
    inline RichDAQ::ShortType colFromAddressAndBit( const RichDAQ::ShortType address,
                                                    const RichDAQ::ShortType bit ) const
    {
      return bit + RichZSPackedCode::BitsField*(address%4);
    }

    /// Get row information from address
    inline RichDAQ::ShortType rowFromAddress( const RichDAQ::ShortType address ) const
    {
      return address/4;
    }

  private: // data

    /// Too big flag
    bool m_tooBig;

  };

} // end V2 namespace

#endif // RICHDAQ_RICHZEROSUPPDATA_H
