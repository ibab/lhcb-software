
//-----------------------------------------------------------------------------
/** @file RichZeroSuppData.cpp
 *
 *  Implementation file for RICH DAQ helper class : RichZeroSuppData
 *
 *  $Id: RichZeroSuppData.cpp,v 1.5 2005-11-15 12:57:48 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

// local
#include "RichZeroSuppData.h"

//-----------------------------------------------------------------------------

namespace RichZeroSuppDataV0 {

  void RichZeroSuppData::buildData( const RichSmartID::Collection & pdHits )
  {

    // Loop over digits and form groups of three
    RichSmartID::Collection::const_iterator iDigit = pdHits.begin();
    while ( iDigit != pdHits.end() ) {

      const RichSmartID *one(&*iDigit), *two(NULL), *three(NULL);
      ++iDigit;
      if ( iDigit != pdHits.end() ) {
        two = &(*iDigit);
        ++iDigit;
        if ( iDigit != pdHits.end() ) {
          three = &(*iDigit);
          ++iDigit;
        }
      }

      // make a new triplet object and add to data bank
      addData( RichZSHitTriplet( one, two, three ) );

    } // end while loop

  }

  void RichZeroSuppData::fillRichSmartIDs ( RichSmartID::Collection & ids,
                                            const IRichDetNumberingTool * /* hpdTool */ ) const
  {

    // How many digits do we expect to make
    const Header head ( header() );
    const RichDAQ::ShortType digitCount = head.hitCount();

    // Loop over data fields
    RichDAQ::ShortType nDigitsMade = 0;
    for ( RichDAQ::ShortType iData = 0; iData < dataSize(); ++iData ) {

      // Get triplet data
      const RichZSHitTriplet triplet( data()[iData] );

      // Make first smartid from triplet
      ids.push_back( RichSmartID( head.hpdID().rich(), head.hpdID().panel(),
                                  head.hpdID().pdRow(), head.hpdID().pdCol(),
                                  triplet.row0(), triplet.col0() ) );
      ++nDigitsMade;
      if ( nDigitsMade == digitCount ) break;

      // Make second smartid from triplet
      ids.push_back( RichSmartID( head.hpdID().rich(), head.hpdID().panel(),
                                  head.hpdID().pdRow(), head.hpdID().pdCol(),
                                  triplet.row1(), triplet.col1() ) );
      ++nDigitsMade;
      if ( nDigitsMade == digitCount ) break;

      // Make third smartid from triplet
      ids.push_back( RichSmartID( head.hpdID().rich(), head.hpdID().panel(),
                                  head.hpdID().pdRow(), head.hpdID().pdCol(),
                                  triplet.row2(), triplet.col2() ) );
      ++nDigitsMade;
      if ( nDigitsMade == digitCount ) break;

    }

  }

  void RichZeroSuppData::fillMsgStream( MsgStream & os ) const
  {

    os << "======================================================================================================"
       << endreq;

    // HPD header
    os << Header( header() ) << endreq;

    // Raw print out of data block
    os << endreq;
    dumpAllBits( os );
    os << endreq;

    os << "======================================================================================================"
       << endreq;

  }

} // V0 namespace

// ===================================================================================================

namespace RichZeroSuppDataV1 {

  void RichZeroSuppData::buildData( const RichSmartID::Collection & pdHits )
  {

    // Loop over digits and form groups of three
    RichSmartID::Collection::const_iterator iDigit = pdHits.begin();
    while ( iDigit != pdHits.end() ) {

      const RichSmartID *one(&*iDigit), *two(NULL), *three(NULL);
      ++iDigit;
      if ( iDigit != pdHits.end() ) {
        two = &(*iDigit);
        ++iDigit;
        if ( iDigit != pdHits.end() ) {
          three = &(*iDigit);
          ++iDigit;
        }
      }

      // make a new triplet object and add to data bank
      addData( RichZSHitTriplet( one, two, three ) );

    } // end while loop

  }

  void RichZeroSuppData::fillRichSmartIDs ( RichSmartID::Collection & ids,
                                            const IRichDetNumberingTool * hpdTool ) const
  {

    // How many digits do we expect to make
    const Header head ( header() );
    const RichDAQ::ShortType digitCount = head.hitCount();

    // Get HPD software ID for this HPD
    const RichSmartID sID = hpdTool->richSmartID( head.l0ID() );

    // Loop over data fields
    RichDAQ::ShortType nDigitsMade = 0;
    for ( RichDAQ::ShortType iData = 0; iData < dataSize(); ++iData ) {

      // Get triplet data
      const RichZSHitTriplet triplet( data()[iData] );

      // Make first smartid from triplet
      ids.push_back( RichSmartID( sID.rich(), sID.panel(),
                                  sID.pdRow(), sID.pdCol(),
                                  triplet.row0(), triplet.col0() ) );
      ++nDigitsMade;
      if ( nDigitsMade == digitCount ) break;

      // Make second smartid from triplet
      ids.push_back( RichSmartID( sID.rich(), sID.panel(),
                                  sID.pdRow(), sID.pdCol(),
                                  triplet.row1(), triplet.col1() ) );
      ++nDigitsMade;
      if ( nDigitsMade == digitCount ) break;

      // Make third smartid from triplet
      ids.push_back( RichSmartID( sID.rich(), sID.panel(),
                                  sID.pdRow(), sID.pdCol(),
                                  triplet.row2(), triplet.col2() ) );
      ++nDigitsMade;
      if ( nDigitsMade == digitCount ) break;

    }

  }

  void RichZeroSuppData::fillMsgStream( MsgStream & os ) const
  {

    os << "======================================================================================================"
       << endreq;

    // HPD header
    os << Header( header() ) << endreq;

    // Raw print out of data block
    os << endreq;
    dumpAllBits( os );
    os << endreq;

    os << "======================================================================================================"
       << endreq;

  }

} // V1 namespace

// ===================================================================================================

namespace RichZeroSuppDataV2 {

  void RichZeroSuppData::buildData( const RichSmartID::Collection & pdHits )
  {

    // temporary data map
    typedef RichMap< RichDAQ::ShortType, RichDAQ::ShortType > DataMap;
    DataMap dataM;

    // Loop over digits and form groups of three
    for ( RichSmartID::Collection::const_iterator iID = pdHits.begin();
          iID != pdHits.end(); ++iID )
    {

      // Get bit and address
      const RichDAQ::ShortType bit     = bitFromCol( (*iID).pixelCol() );
      const RichDAQ::ShortType address = addressFromRowCol( (*iID).pixelRow(), (*iID).pixelCol() );

      // Set the correct bit on for this address
      setBit( dataM[address], bit );

    }

    // How many words ?
    const RichDAQ::ShortType nWords = dataM.size()/2 + dataM.size()%2;
    if ( nWords > 32 ) { m_tooBig = true; return; }

    // Loop over address / bitfield pairs and fill to data bank
    DataMap::const_iterator iD = dataM.begin();
    while ( iD != dataM.end() )
    {
      const RichDAQ::ShortType address0( (*iD).first ), field0( (*iD).second );
      RichDAQ::ShortType address1(0), field1(0);
      ++iD;
      if ( iD != dataM.end() )
      {
        address1 = (*iD).first;
        field1   = (*iD).second;
        ++iD;
      }

      // Add these to the data block
      addData( RichZSPacked( address0, field0, address1, field1 ) );

    }

  }

  void RichZeroSuppData::fillRichSmartIDs ( RichSmartID::Collection & ids,
                                            const IRichDetNumberingTool * hpdTool ) const
  {

    // How many digits do we expect to make
    const Header head ( header() );
    const RichDAQ::ShortType digitCount = head.hitCount();

    // Get HPD software ID for this HPD
    const RichSmartID sID = hpdTool->richSmartID( head.l0ID() );

    // Loop over data fields
    RichDAQ::ShortType nDigitsMade = 0;
    for ( RichDAQ::ShortType iData = 0; iData < dataSize(); ++iData ) {

      // Get packed data data
      const RichZSPacked zsData( data()[iData] );

      // Get first address and bit field
      {
        const RichDAQ::ShortType address = zsData.address0();
        const RichDAQ::ShortType bits    = zsData.bitField0();
        for ( RichDAQ::ShortType iB = 0; iB < RichZSPackedCode::BitsField; ++iB )
        {
          if ( isBitOn(bits,iB) )
          {
            ids.push_back( RichSmartID( sID.rich(), sID.panel(),
                                        sID.pdRow(), sID.pdCol(),
                                        rowFromAddress(address),
                                        colFromAddressAndBit(address,iB) ) );
            ++nDigitsMade;
            if ( nDigitsMade == digitCount ) break;
          }
        }
      }

      // Get second address and bit field
      {
        const RichDAQ::ShortType address = zsData.address1();
        const RichDAQ::ShortType bits    = zsData.bitField1();
        for ( RichDAQ::ShortType iB = 0; iB < RichZSPackedCode::BitsField; ++iB )
        {
          if ( isBitOn(bits,iB) )
          {
            ids.push_back( RichSmartID( sID.rich(), sID.panel(),
                                        sID.pdRow(), sID.pdCol(),
                                        rowFromAddress(address),
                                        colFromAddressAndBit(address,iB) ) );
            ++nDigitsMade;
            if ( nDigitsMade == digitCount ) break;
          }
        }
      }

    }

  }

  void RichZeroSuppData::fillMsgStream( MsgStream & os ) const
  {

    os << "======================================================================================================"
       << endreq;

    // HPD header
    os << Header( header() ) << endreq;

    // Raw print out of data block
    os << endreq;
    dumpAllBits( os );
    os << endreq;

    os << "======================================================================================================"
       << endreq;

  }

} // V2 namespace
