
//-----------------------------------------------------------------------------
/** @file RichZeroSuppData.cpp
 *
 *  Implementation file for RICH DAQ helper class : RichZeroSuppData
 *
 *  CVS Log :-
 *  $Id: RichZeroSuppData.cpp,v 1.1 2005-01-07 12:35:59 jonrob Exp $
 *  $Log: not supported by cvs2svn $
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

  void RichZeroSuppData::fillRAWBank( RichDAQ::RAWBank & rawData ) const
  {
    // header word
    rawData.push_back( header() );
    // then data words
    for ( RichDAQ::ShortType iData = 0; iData < dataSize(); ++iData ) {
      rawData.push_back( data()[iData] );
    }
  }

  void RichZeroSuppData::fillRichSmartIDs ( RichSmartID::Collection & ids,
                                            const IRichHPDIDTool * /* hpdTool */ ) const
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
    os << RichZeroSuppDataV0::RichZeroSuppData::Header( header() ) << endreq;

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

  void RichZeroSuppData::fillRAWBank( RichDAQ::RAWBank & rawData ) const
  {
    // header word
    rawData.push_back( header() );
    // then data words
    for ( RichDAQ::ShortType iData = 0; iData < dataSize(); ++iData ) {
      rawData.push_back( data()[iData] );
    }
  }

  void RichZeroSuppData::fillRichSmartIDs ( RichSmartID::Collection & ids,
                                            const IRichHPDIDTool * hpdTool ) const
  {

    // How many digits do we expect to make
    const Header head ( header() );
    const RichDAQ::ShortType digitCount = head.hitCount();

    // Get HPD software ID for this HPD
    const RichSmartID sID = hpdTool->richSmartID( head.hpdID() );

    // Loop over data fields
    RichDAQ::ShortType nDigitsMade = 0;
    for ( RichDAQ::ShortType iData = 0; iData < dataSize(); ++iData ) {

      // Get triplet data
      const RichZSHitTriplet triplet( data()[iData] );

      // Make first smartid from triplet
      ids.push_back( RichSmartID( sID.rich(), sID.panel(),
                                  sID.PDRow(), sID.PDCol(),
                                  triplet.row0(), triplet.col0() ) );
      ++nDigitsMade;
      if ( nDigitsMade == digitCount ) break;

      // Make second smartid from triplet
      ids.push_back( RichSmartID( sID.rich(), sID.panel(),
                                  sID.PDRow(), sID.PDCol(),
                                  triplet.row1(), triplet.col1() ) );
      ++nDigitsMade;
      if ( nDigitsMade == digitCount ) break;

      // Make third smartid from triplet
      ids.push_back( RichSmartID( sID.rich(), sID.panel(),
                                  sID.PDRow(), sID.PDCol(),
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
    os << RichZeroSuppDataV1::RichZeroSuppData::Header( header() ) << endreq;

    // Raw print out of data block
    os << endreq;
    dumpAllBits( os );
    os << endreq;

    os << "======================================================================================================"
       << endreq;

  }

} // V1 namespace
