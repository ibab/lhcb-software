// $Id: RawBufferToRichDigitsAlg.cpp,v 1.5 2004-04-20 13:33:04 jonesc Exp $

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
  StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
};


// Main execution
StatusCode RawBufferToRichDigitsAlg::execute() {

  debug() << "Execute" << endreq;

  // Retrieve the RawEvent:
  RawEvent * rawEvent = getRawEvent();

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
  put( m_digits, m_richDigitsLoc );
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Successfully registered " << m_digits->size()
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
    verbose()  << "Decoding " << digitCount << " zero suppressed hits for PD "
               << pdID << endreq
               << " Header : " << bankHeader << endreq;
  }

  // Loop over data fields (Skip first header field)
  unsigned int nDigitsMade = 0;
  for ( int iEntry = 1; iEntry < bank.dataSize(); ++iEntry ) {

    // Get triplet data
    RichZSHitTriplet triplet( bank.data()[iEntry] );
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " Decoding triplet " << triplet << endreq;

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
    verbose() << "Decoding " << digitCount << " non-zero suppressed hits for PD "
              << pdID << endreq
              << " Header : " << bankHeader << endreq;
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
    if ( msgLevel(MSG::VERBOSE) ) verbose() << "  Hit " << *iID << endreq;
    RichDigit * newDigit = new RichDigit();
    m_digits->insert( newDigit, *iID );
  }

  return digitCount;
}

//  Finalize
StatusCode RawBufferToRichDigitsAlg::finalize()
{
  debug() << "Finalise" << endreq;

  // finalise base
  return RichAlgBase::finalize();
}

RawEvent * RawBufferToRichDigitsAlg::getRawEvent()
{

  // Try and load from TES. If it exists return, overwise try to create one
  SmartDataPtr<RawEvent> rawEventTES( eventSvc(), m_rawEventLoc );
  if ( rawEventTES ) { return rawEventTES; }
  else {
    debug() << "Creating RawEvent from RawBuffer" << endreq;

    // Retrieve the RawBuffer
    SmartDataPtr<RawBuffer> rawBuffer( eventSvc(), RawBufferLocation::Default );
    if ( !rawBuffer ) { Error("Unable to locate RawBuffer"); return 0; }

    // make new RawEvent and put into TES
    RawEvent * rawEvent = new RawEvent( rawBuffer );
    if ( !rawEvent ) { Error("Unable to allocate memory to RawEvent"); return 0; }
    put( rawEvent, RawEventLocation::Default );

    return rawEvent;
  }

  return 0;
};
