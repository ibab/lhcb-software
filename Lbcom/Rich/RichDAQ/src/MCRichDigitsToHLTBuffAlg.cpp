// $Id: MCRichDigitsToHLTBuffAlg.cpp,v 1.1.1.1 2003-11-08 14:26:19 jonesc Exp $
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
  : RichAlgBase ( name, pSvcLocator ) {

  declareProperty( "HLTBufferLocation",   m_hltBuffLoc = HltBufferLocation::Default );
  declareProperty( "MCRichDigitsLocation", m_digitsLoc = MCRichDigitLocation::Default );
  declareProperty( "ZeroSuppressHitCut", m_zeroSuppresCut = 50 );

}

// Destructor
MCRichDigitsToHLTBuffAlg::~MCRichDigitsToHLTBuffAlg() {};

// Initialisation.
StatusCode MCRichDigitsToHLTBuffAlg::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Initialise" << endreq;

  // intialise base
  if ( !RichAlgBase::initialize() ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode MCRichDigitsToHLTBuffAlg::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Execute" << endreq;

  // Retrieve the HLTBuffer
  SmartDataPtr<HltBuffer> hltBuffer( eventSvc(), m_hltBuffLoc );
  if ( !hltBuffer ) {
    msg << MSG::ERROR
        << "Unable to retrieve HLT buffer from " <<  m_hltBuffLoc << endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve RichDigits
  SmartDataPtr<MCRichDigits> digits( eventSvc(), m_digitsLoc );
  if ( !digits ) {
    msg << MSG::ERROR
        << "Unable to retrieve MCRichDigits from " << m_digitsLoc << endreq;
    return StatusCode::FAILURE;
  }

  RichPDMap PDDigits;

  // Loop overdigits and sort according to PD
  for ( MCRichDigits::const_iterator iDigit = digits->begin();
        iDigit != digits->end(); ++iDigit ) {
    PDDigits[(*iDigit)->key().pdID()].push_back( *iDigit );
  }

  // Loop over each photon detector and produce a digit bank for each
  for ( RichPDMap::const_iterator iPD = PDDigits.begin();
        iPD != PDDigits.end(); ++iPD ) {

    // Make a new data bank
    RichHLTBank dataBank;

    // Based on number of hits, decide whether to zero suppress or not
    if ( ((*iPD).second).size() < m_zeroSuppresCut ) {
      fillZeroSuppressed( (*iPD).first, dataBank, (*iPD).second );
    } else {
      fillNonZeroSuppressed( (*iPD).first, dataBank, (*iPD).second );
    }

    // Add this bank to the HLT buffer
    RichDAQHeaderPD header = dataBank[0];
    hltBuffer->addBank( header.linkNumber(), RichDigit::classID(), dataBank );

  } // end photon detector loop

  if ( msgLevel(MSG::DEBUG) ) {
    msg << MSG::DEBUG
        << "Created " << PDDigits.size() << " Rich banks in HltBuffer" << endreq;
  }

  return StatusCode::SUCCESS;
};

void MCRichDigitsToHLTBuffAlg::fillZeroSuppressed( RichSmartID pdID,
                                                   RichHLTBank & dataBank,
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

void MCRichDigitsToHLTBuffAlg::fillNonZeroSuppressed ( RichSmartID pdID,
                                                       RichHLTBank & dataBank,
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
  }

  // Create the non zero suppressed data block
  RichNonZeroSuppData nonZSdata( pdHits );

  // Loop over data entries and add to RichHLTBank
  for ( unsigned int iData = 0; iData < nonZSdata.dataSize(); ++iData ) {
    dataBank.push_back( (nonZSdata.data())[iData] );
  }

  // Printout of data
  if ( msgLevel(MSG::VERBOSE) ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::VERBOSE << nonZSdata << endreq;
  }

}


//  Finalize
StatusCode MCRichDigitsToHLTBuffAlg::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalise" << endreq;

  // finalise base
  return RichAlgBase::finalize();
}
