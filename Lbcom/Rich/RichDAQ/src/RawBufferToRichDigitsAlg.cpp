// $Id: RawBufferToRichDigitsAlg.cpp,v 1.2 2004-02-02 14:27:39 jonesc Exp $

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RawBufferToRichDigitsAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RawBufferToRichDigitsAlg
//
// 2003-11-06 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RawBufferToRichDigitsAlg>          s_factory ;
const        IAlgFactory& RawBufferToRichDigitsAlgFactory = s_factory ;

// Standard constructor, initializes variables
RawBufferToRichDigitsAlg::RawBufferToRichDigitsAlg( const std::string& name,
                                                    ISvcLocator* pSvcLocator )
  : RichAlgBase ( name , pSvcLocator ),
    m_digits    ( 0                  ) {

  declareProperty( "RawEventLocation",
                   m_rawEventLoc = RawEventLocation::Default );
  declareProperty( "RichDigitsLocation",
                   m_richDigitsLoc = RichDigitLocation::Default );

}

// Destructor
RawBufferToRichDigitsAlg::~RawBufferToRichDigitsAlg() {};

// Initialisation.
StatusCode RawBufferToRichDigitsAlg::initialize() {

  // intialise base
  if ( !RichAlgBase::initialize() ) return StatusCode::FAILURE;

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::DEBUG << "Initialise :-" << endreq
        << " Input  RawEvent location   = " << m_rawEventLoc << endreq
        << " Output RichDigits location = " << m_richDigitsLoc << endreq;
  }

  return StatusCode::SUCCESS;
};


// Main execution
StatusCode RawBufferToRichDigitsAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::DEBUG << "Execute" << endreq;
  }

  // Retrieve the RawEvent:
  SmartDataPtr<RawEvent> rawEvent( eventSvc(), m_rawEventLoc );
  if ( 0 == rawEvent ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR
        << "Unable to retrieve RawEvent from " << m_rawEventLoc << endreq;
    return StatusCode::FAILURE;
  }

  // Get the banks for the RichDigits
  const RichDAQ::RAWBanks & richBanks = rawEvent->banks( RawBuffer::Rich );

  // Make new container for RichDigits
  m_digits = new RichDigits();

  // Loop over data banks
  for ( RichDAQ::RAWBanks::const_iterator iBank = richBanks.begin();
        iBank != richBanks.end(); ++iBank ) {

    // Get the bank header
    RichDAQHeaderPD bankHeader( (*iBank).data()[0] );

    // Is this a zero suppressed bank or not ?
    if ( bankHeader.zeroSuppressed() ) {
      decodeZeroSuppressedBank(*iBank);
    } else {
      decodeNonZeroSuppressedBank(*iBank);
    }

  } // end loop over data banks

  // Register new container of RichDigits to Gaudi data store
  if ( !eventSvc()->registerObject(m_richDigitsLoc,m_digits) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR << "Failed to register RichDigits at "
        << m_richDigitsLoc << endreq;
    return StatusCode::FAILURE;
  }

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::DEBUG << "Successfully registered " << m_digits->size()
        << " RichDigits from " << richBanks.size() << " Raw banks at "
        << m_richDigitsLoc << endreq;
  }

  return StatusCode::SUCCESS;
};

unsigned int
RawBufferToRichDigitsAlg::decodeZeroSuppressedBank( const RawBank & bank ) const {

  // Get the link identifier
  RichDAQLinkNumber linkN( bank.bankSourceID() );

  // Create a RichSmartID for the PD
  RichSmartID pdID(linkN.rich(),linkN.panel(),linkN.pdRow(),linkN.pdCol(),0,0);

  // Get the bank header
  RichDAQHeaderPD bankHeader( bank.data()[0] );

  // How many digits do we expect to make
  RichDAQ::ShortType digitCount = bankHeader.hitCount();

  if ( msgLevel(MSG::VERBOSE) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::VERBOSE
        << "Decoding " << digitCount << " zero suppressed hits for PD "
        << pdID << endreq;
    msg << MSG::VERBOSE << " Header : " << bankHeader << endreq;
  }

  // Loop over data fields (Skip first header field)
  unsigned int nDigitsMade = 0;
  for ( int iEntry = 1; iEntry < bank.dataSize(); ++iEntry ) {

    // Get triplet data
    RichZSHitTriplet triplet( bank.data()[iEntry] );
    if ( msgLevel(MSG::VERBOSE) ) {
      MsgStream msg( msgSvc(), name() );
      msg << MSG::VERBOSE << " Decoding triplet " << triplet << endreq;
    }

    // Make first digit from triplet
    if ( nDigitsMade >= digitCount ) break;
    RichSmartID newID0( pdID.rich(), pdID.panel(),
                        pdID.PDRow(), pdID.PDCol(),
                        triplet.row0(), triplet.col0() );
    ++nDigitsMade;
    RichDigit * newDigit0 = new RichDigit();
    m_digits->insert( newDigit0, newID0 );

    // Make second digit from triplet
    if ( nDigitsMade >= digitCount ) break;
    RichSmartID newID1( pdID.rich(), pdID.panel(),
                        pdID.PDRow(), pdID.PDCol(),
                        triplet.row1(), triplet.col1() );
    ++nDigitsMade;
    RichDigit * newDigit1 = new RichDigit();
    m_digits->insert( newDigit1, newID1 );

    // Make third digit from triplet
    if ( nDigitsMade >= digitCount ) break;
    RichSmartID newID2( pdID.rich(), pdID.panel(),
                        pdID.PDRow(), pdID.PDCol(),
                        triplet.row2(), triplet.col2() );
    ++nDigitsMade;
    RichDigit * newDigit2 = new RichDigit();
    m_digits->insert( newDigit2, newID2 );

  }

  return nDigitsMade;
}

unsigned int
RawBufferToRichDigitsAlg::decodeNonZeroSuppressedBank( const RawBank & bank ) const {

  // Get the link identifier
  RichDAQLinkNumber linkN( bank.bankSourceID() );

  // Create a RichSmartID for the PD
  RichSmartID pdID(linkN.rich(),linkN.panel(),linkN.pdRow(),linkN.pdCol(),0,0);

  // Get the bank header
  RichDAQHeaderPD bankHeader( bank.data()[0] );

  // How many digits do we expect to make
  RichDAQ::ShortType digitCount = bankHeader.hitCount();

  if ( msgLevel(MSG::VERBOSE) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::VERBOSE
        << "Decoding " << digitCount << " non-zero suppressed hits for PD "
        << pdID << endreq;
    msg << MSG::VERBOSE << " Header : " << bankHeader << endreq;
  }

  // Create a block of non-zero suppressed data from RawBank
  RichNonZeroSuppData nonZSdata( bank );

  // Get new SmartIDs
  RichDAQ::SmartIDs IDs;
  nonZSdata.fillSmartIDs( linkN.rich(),linkN.panel(),
                          linkN.pdRow(),linkN.pdCol(),IDs );

  // Create RichDigits with new RichSmartIDs
  for ( RichDAQ::SmartIDs::const_iterator iID = IDs.begin();
        iID != IDs.end(); ++iID ) {
    if ( msgLevel(MSG::VERBOSE) ) {
      MsgStream  msg( msgSvc(), name() );
      msg << MSG::VERBOSE << "  Hit " << *iID << endreq;
    }
    RichDigit * newDigit = new RichDigit();
    m_digits->insert( newDigit, *iID );
  }

  return digitCount;
}

//  Finalize
StatusCode RawBufferToRichDigitsAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::DEBUG << "Finalise" << endreq;
  }

  // finalise base
  return RichAlgBase::finalize();
}

