
/** @file RichZeroSuppData.h
 *
 *  Header file for RICH DAQ utility class : RichZeroSuppData
 *
 *  CVS Log :-
 *  $Id: RichZeroSuppData.h,v 1.1 2005-01-07 12:35:59 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 *
 *  @todo See if decoding can be speeded up by removing need for data copying
 */

#ifndef RICHDAQ_RICHZEROSUPPDATA_H
#define RICHDAQ_RICHZEROSUPPDATA_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Event Model
#include "Event/DAQTypes.h"

// local
#include "RichHPDDataBank.h"
#include "RichZSHitTriplet.h"
#include "RichDAQHeaderPD.h"

/** @namespace RichZeroSuppDataV0
 *
 *  Namespace for version 0 of the RichZeroSuppData object.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
namespace RichZeroSuppDataV0 {

  /** @namespace RichZeroSuppDataCode
   *
   *  Namespace for definitions related to RichZeroSuppData
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-06
   */
  namespace RichZeroSuppDataCode {

    /// Maximum number of bits
    static const RichDAQ::ShortType MaxBits = 32;

  }

  /** @class RichZeroSuppData RichZeroSuppData.h
   *
   *  The non zero-suppressed data format
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-07
   */
  class RichZeroSuppData : public RichHPDDataBank {

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
                                   const IRichHPDIDTool * hpdTool ) const;

    // Fill a vector with Raw data words
    virtual void fillRAWBank( RichDAQ::RAWBank & rawData ) const;

    // Print data bank to Gaudi MsgStream
    virtual void fillMsgStream( MsgStream & os ) const;

  private: // methods

    /// Test if a given bit in a word is set on
    inline bool
    isBitOn( const RichDAQ::LongType data, const RichDAQ::ShortType pos ) const
    {
      return ( 0 != (data & (1<<pos)) );
    }

    /// Set a given bit in a data word on
    inline void setBit( RichDAQ::LongType & data, const RichDAQ::ShortType pos )
    {
      data |= 1<<pos;
    }

    /// Build data array from vector of RichSmartIDs
    void buildData( const RichSmartID::Collection & pdHits );

  };

} // end V0 namespace

/// overloaded output to MsgStream
MsgStream & operator << ( MsgStream & os,
                          const RichZeroSuppDataV0::RichZeroSuppData & data );

// =================================================================================

/** @namespace RichZeroSuppDataV1
 *
 *  Namespace for version 1 of the RichZeroSuppData object.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
namespace RichZeroSuppDataV1 {

  /** @namespace RichZeroSuppDataCode
   *
   *  Namespace for definitions related to RichZeroSuppData
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-06
   */
  namespace RichZeroSuppDataCode {

    /// Maximum number of bits
    static const RichDAQ::ShortType MaxBits = 32;

  }

  /** @class RichZeroSuppData RichZeroSuppData.h
   *
   *  The non zero-suppressed data format
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2003-11-07
   */
  class RichZeroSuppData : public RichHPDDataBank {

  public: // Definitions
    
    /// Typedef for the header type for this data bank implementation
    typedef RichDAQHeaderV1::RichDAQHeaderPD Header;

    /// Typedef for HPD identifier for this data bank implementation
    //typedef RichDAQHPDIdentifierV1::RichDAQHPDIdentifier HPDID;

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
                                   const IRichHPDIDTool * hpdTool ) const;

    // Fill a vector with Raw data words
    virtual void fillRAWBank( RichDAQ::RAWBank & rawData ) const;

    // Print data bank to Gaudi MsgStream
    virtual void fillMsgStream( MsgStream & os ) const;

  private: // methods

    /// Test if a given bit in a word is set on
    inline bool
    isBitOn( const RichDAQ::LongType data, const RichDAQ::ShortType pos ) const
    {
      return ( 0 != (data & (1<<pos)) );
    }

    /// Set a given bit in a data word on
    inline void setBit( RichDAQ::LongType & data, const RichDAQ::ShortType pos )
    {
      data |= 1<<pos;
    }

    /// Build data array from vector of RichSmartIDs
    void buildData( const RichSmartID::Collection & pdHits );

  };

} // end V1 namespace

/// overloaded output to MsgStream
MsgStream & operator << ( MsgStream & os,
                          const RichZeroSuppDataV1::RichZeroSuppData & data );

#endif // RICHDAQ_RICHZEROSUPPDATA_H
