// $Id: HLTBuffToRichDigitsAlg.cpp,v 1.1.1.1 2003-11-08 14:26:20 jonesc Exp $

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "HLTBuffToRichDigitsAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HLTBuffToRichDigitsAlg
//
// 2003-11-06 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<HLTBuffToRichDigitsAlg>          s_factory ;
const        IAlgFactory& HLTBuffToRichDigitsAlgFactory = s_factory ;

// Standard constructor, initializes variables
HLTBuffToRichDigitsAlg::HLTBuffToRichDigitsAlg( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : RichAlgBase ( name , pSvcLocator ),
    m_digits    ( 0                  ) {

  declareProperty( "HLTEventLocation",
                   m_hltEventLoc = HltEventLocation::Default );
  declareProperty( "RichDigitsLocation",
                   m_richDigitsLoc = RichDigitLocation::Default );

}

// Destructor
HLTBuffToRichDigitsAlg::~HLTBuffToRichDigitsAlg() {};

// Initialisation.
StatusCode HLTBuffToRichDigitsAlg::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Initialise" << endreq;

  // intialise base
  if ( !RichAlgBase::initialize() ) return StatusCode::FAILURE;

  msg << MSG::WARNING
      << "REMEMBER : Remove temporary method to create HltEvent" << endreq;

  return StatusCode::SUCCESS;
};


// Main execution
StatusCode HLTBuffToRichDigitsAlg::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Execute" << endreq;

  // temporary - Make the HltEvent
  if ( !createHltEvent() ) return StatusCode::FAILURE;

  // Retrieve the HLTEvent:
  SmartDataPtr<HltEvent> hltEvent( eventSvc(), m_hltEventLoc );
  if ( !hltEvent ) {
    msg << MSG::ERROR
        << "Unable to retrieve HLTEvent from " << m_hltEventLoc << endreq;
    return StatusCode::FAILURE;
  }

  // Get the bank for the RichDigits
  const RichHltBanks & richBanks = hltEvent->banks( RichDigit::classID() );
  if ( msgLevel(MSG::DEBUG) ) {
    msg << MSG::DEBUG
        << "Found " << richBanks.size() << " Rich banks in HltEvent"
        << endreq;
  }

  // Make new container for RichDigits
  m_digits = new RichDigits();

  // Loop over data banks
  for ( RichHltBanks::const_iterator iBank = richBanks.begin();
        iBank != richBanks.end(); ++iBank ) {

    // Get the bank header
    RichDAQHeaderPD bankHeader( (*iBank).data()[0] );

    // Is this a zero suppressed bank or not ?
    if ( bankHeader.zeroSuppressed() ) {
      decodeZeroSuppressedBank(*iBank);
    } else {
      decodeNonZeroSuppressedBank(*iBank);
    }

  }

  // Register new container of RichDigits to Gaudi data store
  if ( !eventSvc()->registerObject(m_richDigitsLoc,m_digits) ) {
    msg << MSG::ERROR << "Failed to register RichDigits at "
        << m_richDigitsLoc << endreq;
    return StatusCode::FAILURE;
  }
  if ( msgLevel(MSG::DEBUG) ) {
    msg << MSG::DEBUG << "Successfully registered " << m_digits->size()
        << " RichDigits at " << m_richDigitsLoc << endreq;
  }

  return StatusCode::SUCCESS;
};

unsigned int 
HLTBuffToRichDigitsAlg::decodeZeroSuppressedBank( const HltBank & bank ) {

  // Get the link identifier
  RichDAQLinkNumber linkN( bank.bankSourceID() );

  // Create a RichSmartID for the PD
  RichSmartID pdID( linkN.rich(), linkN.panel(), linkN.pdRow(), linkN.pdCol(), 0, 0 );

  // Get the bank header
  RichDAQHeaderPD bankHeader( bank.data()[0] );

  // How many digits do we expect to make
  RichDAQHeaderPD::ShortType digitCount = bankHeader.hitCount();

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
HLTBuffToRichDigitsAlg::decodeNonZeroSuppressedBank( const HltBank & bank ) {

  // Get the link identifier
  RichDAQLinkNumber linkN( bank.bankSourceID() );

  // Create a RichSmartID for the PD
  RichSmartID pdID( linkN.rich(), linkN.panel(), linkN.pdRow(), linkN.pdCol(), 0, 0 );

  // Get the bank header
  RichDAQHeaderPD bankHeader( bank.data()[0] );

  // How many digits do we expect to make
  RichDAQHeaderPD::ShortType digitCount = bankHeader.hitCount();

  if ( msgLevel(MSG::VERBOSE) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::VERBOSE
        << "Decoding " << digitCount << " non-zero suppressed hits for PD "
        << pdID << endreq;
    msg << MSG::VERBOSE << " Header : " << bankHeader << endreq;
  }

  // Create an empty block of non-zero suppressed data
  RichNonZeroSuppData nonZSdata;

  // Loop over data entries and set data
  {for ( unsigned int iData = 0; iData < nonZSdata.dataSize(); ++iData ) {
    (nonZSdata.data())[iData] = bank.data()[iData+1];
  }}

  return digitCount;
}

//  Finalize
StatusCode HLTBuffToRichDigitsAlg::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalise" << endreq;

  // finalise base
  return RichAlgBase::finalize();
}

//=============================================================================
// temporary whilst testing
StatusCode HLTBuffToRichDigitsAlg::createHltEvent() {

  // Retrieve the HLTBuffer
  SmartDataPtr<HltBuffer> hltBuffer( eventSvc(), HltBufferLocation::Default );

  if ( 0 == hltBuffer ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Unable to retrieve HltBuffer" << endmsg;
    return StatusCode::FAILURE;
  }

  HltEvent* hltEvent = new  HltEvent( hltBuffer );

  if ( 0 == hltEvent ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Unable to allocate memory to HltEvent" << endmsg;
    return StatusCode::FAILURE;
  }

  StatusCode sc = eventSvc()->registerObject( HltEventLocation::Default,
                                              hltEvent );

  if( sc.isFailure() ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Unable to register HltEvent in TES" << endmsg;
    delete hltEvent;
    return sc;
  }

  return StatusCode::SUCCESS;
};

