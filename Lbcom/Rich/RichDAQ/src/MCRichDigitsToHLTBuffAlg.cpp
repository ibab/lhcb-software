// $Id: MCRichDigitsToHLTBuffAlg.cpp,v 1.7 2003-11-21 13:13:40 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "MCRichDigitsToHLTBuffAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCRichDigitsToHLTBuffAlg
//
// 2003-11-06 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MCRichDigitsToHLTBuffAlg>          s_factory ;
const        IAlgFactory& MCRichDigitsToHLTBuffAlgFactory = s_factory ;

// Standard constructor
MCRichDigitsToHLTBuffAlg::MCRichDigitsToHLTBuffAlg( const std::string& name,
                                                    ISvcLocator* pSvcLocator )
  : RichAlgBase ( name, pSvcLocator )
{

  declareProperty( "HLTBufferLocation",   m_hltBuffLoc = HltBufferLocation::Default );
  declareProperty( "MCRichDigitsLocation", m_digitsLoc = MCRichDigitLocation::Default );
  declareProperty( "ZeroSuppressHitCut", m_zeroSuppresCut = 96 );

}

// Destructor
MCRichDigitsToHLTBuffAlg::~MCRichDigitsToHLTBuffAlg() {};

// Initialisation.
StatusCode MCRichDigitsToHLTBuffAlg::initialize() {

  // intialise base
  if ( !RichAlgBase::initialize() ) return StatusCode::FAILURE;

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::DEBUG << "Initialise :-" << endreq
        << " MCRichDigit location                = " << m_digitsLoc << endreq
        << " HltBuffer location                  = " << m_hltBuffLoc << endreq
        << " Max hits for zero-suppressed data   = " << m_zeroSuppresCut << endreq;
  }

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode MCRichDigitsToHLTBuffAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::DEBUG << "Execute" << endreq;
  }

  // Retrieve the HLTBuffer
  SmartDataPtr<HltBuffer> hltBuffer( eventSvc(), m_hltBuffLoc );
  if ( !hltBuffer ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR
        << "Unable to retrieve HLT buffer from " <<  m_hltBuffLoc << endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve MCRichDigits
  SmartDataPtr<MCRichDigits> digits( eventSvc(), m_digitsLoc );
  if ( !digits ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR
        << "Unable to retrieve MCRichDigits from " << m_digitsLoc << endreq;
    return StatusCode::FAILURE;
  }

  Rich::PDMap PDDigits;

  // Loop over digits and sort according to PD
  for ( MCRichDigits::const_iterator iDigit = digits->begin();
        iDigit != digits->end(); ++iDigit ) {
    PDDigits[(*iDigit)->key().pdID()].push_back( *iDigit );
  }

  // Loop over each photon detector and produce a digit bank for each
  for ( Rich::PDMap::const_iterator iPD = PDDigits.begin();
        iPD != PDDigits.end(); ++iPD ) {

    // Make a new data bank
    Rich::HLTBank dataBank;

    // Based on number of hits, decide whether to zero suppress or not
    if ( ((*iPD).second).size() < m_zeroSuppresCut ) {
      fillZeroSuppressed( (*iPD).first, dataBank, (*iPD).second );
    } else {
      fillNonZeroSuppressed( (*iPD).first, dataBank, (*iPD).second );
    }

    // Add this bank to the HLT buffer
    RichDAQHeaderPD header ( dataBank[0] );
    hltBuffer->addBank( header.linkNumber(), HltBuffer::Rich, dataBank );

  } // end photon detector loop

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::DEBUG
        << "Created " << PDDigits.size() << " Rich banks for "
        << digits->size() << " MCRichDigits in HltBuffer" << endreq;
  }

  return StatusCode::SUCCESS;
};

void MCRichDigitsToHLTBuffAlg::fillZeroSuppressed( RichSmartID pdID,
                                                   Rich::HLTBank & dataBank,
                                                   const MCRichDigitVector & pdHits ) {

  // Make a new header word for this PD and add to data bank
  RichDAQLinkNumber linkNumber( pdID.rich(), pdID.panel(),
                                pdID.PDRow(), pdID.PDCol() );
  RichDAQHeaderPD pdHeader( true, linkNumber, pdHits.size() );
  dataBank.push_back( pdHeader );

  if ( msgLevel(MSG::VERBOSE) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::VERBOSE << "PD " << pdID << " : Creating " << pdHits.size()
        << " zero suppressed hits " << endreq;
    msg << MSG::VERBOSE << " Header : " << pdHeader << endreq;
  }

  // Loop over digits and form groups of three
  MCRichDigitVector::const_iterator iDigit = pdHits.begin();
  while ( iDigit != pdHits.end() ) {

    MCRichDigit * one   = (*iDigit);
    MCRichDigit * two   = NULL;
    MCRichDigit * three = NULL;
    ++iDigit;
    if ( iDigit != pdHits.end() ) {
      two = (*iDigit);
      ++iDigit;
      if ( iDigit != pdHits.end() ) {
        three = (*iDigit);
        ++iDigit;
      }
    }

    // make a new triplet object and add to data bank
    RichZSHitTriplet triplet( one, two, three );
    dataBank.push_back( triplet );
    if ( msgLevel(MSG::VERBOSE) ) {
      MsgStream  msg( msgSvc(), name() );
      msg << MSG::VERBOSE << " Created triplet " << triplet << endreq;
    }

  } // end while loop

}

void MCRichDigitsToHLTBuffAlg::fillNonZeroSuppressed( RichSmartID pdID,
                                                      Rich::HLTBank & dataBank,
                                                      const MCRichDigitVector & pdHits ) {

  // Make a new header word for this PD and add to data bank
  RichDAQLinkNumber linkNumber( pdID.rich(), pdID.panel(),
                                pdID.PDRow(), pdID.PDCol() );
  RichDAQHeaderPD pdHeader( false, linkNumber, pdHits.size() );
  dataBank.push_back( pdHeader );

  if ( msgLevel(MSG::VERBOSE) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::VERBOSE << "PD " << pdID << " : Creating " << pdHits.size()
        << " non-zero suppressed hits " << endreq;
    msg << MSG::VERBOSE << " Header : " << pdHeader << endreq;
    for ( MCRichDigitVector::const_iterator iDig = pdHits.begin();
          iDig != pdHits.end(); ++iDig ) {
      msg << MSG::VERBOSE << " Hit " << (*iDig)->key() << endreq;
    }
  }

  // Create the non zero suppressed data block and update HLTBank
  RichNonZeroSuppData nonZSdata( pdHits );
  nonZSdata.fillHLT( dataBank );

  // Printout of data array
  if ( msgLevel(MSG::VERBOSE) ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::VERBOSE << nonZSdata << endreq;
  }

}

//  Finalize
StatusCode MCRichDigitsToHLTBuffAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::DEBUG << "Finalise" << endreq;
  }

  // finalise base
  return RichAlgBase::finalize();
}
