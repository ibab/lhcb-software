// $Id: MCRichDigitsToRawBufferAlg.cpp,v 1.2 2004-02-02 14:27:38 jonesc Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "MCRichDigitsToRawBufferAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCRichDigitsToRawBufferAlg
//
// 2003-11-06 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MCRichDigitsToRawBufferAlg>          s_factory ;
const        IAlgFactory& MCRichDigitsToRawBufferAlgFactory = s_factory ;

// Standard constructor
MCRichDigitsToRawBufferAlg::MCRichDigitsToRawBufferAlg( const std::string& name,
                                                        ISvcLocator* pSvcLocator )
  : RichAlgBase ( name, pSvcLocator )
{

  declareProperty( "RawBufferLocation",
                   m_rawBuffLoc = RawBufferLocation::Default );
  declareProperty( "MCRichDigitsLocation",
                   m_digitsLoc = MCRichDigitLocation::Default );
  declareProperty( "ZeroSuppressHitCut", m_zeroSuppresCut = 96 );

}

// Destructor
MCRichDigitsToRawBufferAlg::~MCRichDigitsToRawBufferAlg() {};

// Initialisation.
StatusCode MCRichDigitsToRawBufferAlg::initialize() {

  // intialise base
  if ( !RichAlgBase::initialize() ) return StatusCode::FAILURE;

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::DEBUG << "Initialise :-" << endreq
        << " MCRichDigit location                = " << m_digitsLoc << endreq
        << " RawBuffer location                  = " << m_rawBuffLoc << endreq
        << " Max hits for zero-suppressed data   = " << m_zeroSuppresCut
        << endreq;
  }

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode MCRichDigitsToRawBufferAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::DEBUG << "Execute" << endreq;
  }

  // Retrieve the RawBuffer
  SmartDataPtr<RawBuffer> rawBuffer( eventSvc(), m_rawBuffLoc );
  if ( 0 == rawBuffer ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR
        << "Unable to retrieve Raw buffer from " <<  m_rawBuffLoc << endreq;
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

  RichDAQ::PDMap PDDigits;

  // Loop over digits and sort according to PD
  for ( MCRichDigits::const_iterator iDigit = digits->begin();
        iDigit != digits->end(); ++iDigit ) {
    PDDigits[(*iDigit)->key().pdID()].push_back( *iDigit );
  }

  // Loop over each photon detector and produce a digit bank for each
  for ( RichDAQ::PDMap::const_iterator iPD = PDDigits.begin();
        iPD != PDDigits.end(); ++iPD ) {

    // Make a new data bank
    RichDAQ::RAWBank dataBank;

    // Based on number of hits, decide whether to zero suppress or not
    if ( ((*iPD).second).size() < m_zeroSuppresCut ) {
      fillZeroSuppressed( (*iPD).first, dataBank, (*iPD).second );
    } else {
      fillNonZeroSuppressed( (*iPD).first, dataBank, (*iPD).second );
    }

    // Add this bank to the Raw buffer
    RichDAQHeaderPD header ( dataBank[0] );
    rawBuffer->addBank( header.linkNumber(), RawBuffer::Rich, dataBank );

  } // end photon detector loop

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::DEBUG
        << "Created " << PDDigits.size() << " Rich banks for "
        << digits->size() << " MCRichDigits in RawBuffer" << endreq;
  }

  return StatusCode::SUCCESS;
};

void
MCRichDigitsToRawBufferAlg::fillZeroSuppressed( RichSmartID pdID,
                                                RichDAQ::RAWBank & dataBank,
                                                const MCRichDigitVector & pdHits ) const {

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

void
MCRichDigitsToRawBufferAlg::fillNonZeroSuppressed( RichSmartID pdID,
                                                   RichDAQ::RAWBank & dataBank,
                                                   const MCRichDigitVector & pdHits ) const {

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

  // Create the non zero suppressed data block and update RAWBank
  RichNonZeroSuppData nonZSdata( pdHits );
  nonZSdata.fillRAW( dataBank );

  // Printout of data array
  if ( msgLevel(MSG::VERBOSE) ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::VERBOSE << nonZSdata << endreq;
  }

}

//  Finalize
StatusCode MCRichDigitsToRawBufferAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::DEBUG << "Finalise" << endreq;
  }

  // finalise base
  return RichAlgBase::finalize();
}
