
/** @file RawBufferToRichDigitsAlg.cpp
 *
 *  Implementation file for RICH DAQ algorithm : RawBufferToRichDigitsAlg
 *
 *  CVS Log :-
 *  $Id: RawBufferToRichDigitsAlg.cpp,v 1.8 2004-10-13 09:16:27 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.7  2004/07/27 13:46:06  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RawBufferToRichDigitsAlg.h"

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
StatusCode RawBufferToRichDigitsAlg::initialize() 
{
  // intialise base
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Add customisation here

  return StatusCode::SUCCESS;
};


// Main execution
StatusCode RawBufferToRichDigitsAlg::execute() 
{
  debug() << "Execute" << endreq;

  // Retrieve the RawEvent:
  RawEvent * rawEvent = getRawEvent();

  // Get the banks for the RichDigits
  const RichDAQ::RAWBanks & richBanks = rawEvent->banks( RawBuffer::Rich );

  // Check RichDigits don't already exist ?
  SmartDataPtr<RichDigits> richDigitsTES( eventSvc(), m_richDigitsLoc );
  if ( richDigitsTES ) {
    return Warning( "RichDigits already exist - abort building", StatusCode::SUCCESS );
  }

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
  const RichDAQLinkNumber linkN( bank.bankSourceID() );

  // Get the bank header
  const RichDAQHeaderPD bankHeader( bank.data()[0] );

  // How many digits do we expect to make
  const RichDAQ::ShortType digitCount = bankHeader.hitCount();

  if ( msgLevel(MSG::VERBOSE) ) {
    const RichSmartID pdID(linkN.rich(),linkN.panel(),linkN.pdRow(),linkN.pdCol(),0,0);
    verbose()  << "Decoding " << digitCount << " zero suppressed hits for PD "
               << pdID << endreq
               << " Header : " << bankHeader << endreq;
  }

  // Loop over data fields (Skip first header field)
  unsigned int nDigitsMade = 0;
  for ( int iEntry = 1; iEntry < bank.dataSize(); ++iEntry ) {

    // Get triplet data
    const RichZSHitTriplet triplet( bank.data()[iEntry] );
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " Decoding triplet " << triplet << endreq;

    // Make first digit from triplet
    if ( nDigitsMade >= digitCount ) break;
    m_digits->insert( new RichDigit(),
                      RichSmartID( linkN.rich(), linkN.panel(),
                                   linkN.pdRow(), linkN.pdCol(),
                                   triplet.row0(), triplet.col0() ) );
    ++nDigitsMade;

    // Make second digit from triplet
    if ( nDigitsMade >= digitCount ) break;
    m_digits->insert( new RichDigit(),
                      RichSmartID( linkN.rich(), linkN.panel(),
                                   linkN.pdRow(), linkN.pdCol(),
                                   triplet.row1(), triplet.col1() ) );
    ++nDigitsMade;

    // Make third digit from triplet
    if ( nDigitsMade >= digitCount ) break;
    m_digits->insert( new RichDigit(),
                      RichSmartID( linkN.rich(), linkN.panel(),
                                   linkN.pdRow(), linkN.pdCol(),
                                   triplet.row2(), triplet.col2() ) );
    ++nDigitsMade;

  }

  return nDigitsMade;
}

unsigned int
RawBufferToRichDigitsAlg::decodeNonZeroSuppressedBank( const RawBank & bank ) const {

  // Get the link identifier
  const RichDAQLinkNumber linkN( bank.bankSourceID() );

  // Get the bank header
  const RichDAQHeaderPD bankHeader( bank.data()[0] );

  // How many digits do we expect to make
  const RichDAQ::ShortType digitCount = bankHeader.hitCount();

  if ( msgLevel(MSG::VERBOSE) ) {
    const RichSmartID pdID(linkN.rich(),linkN.panel(),linkN.pdRow(),linkN.pdCol(),0,0);
    verbose() << "Decoding " << digitCount << " non-zero suppressed hits for PD "
              << pdID << endreq
              << " Header : " << bankHeader << endreq;
  }

  // Create a block of non-zero suppressed data from RawBank
  const RichNonZeroSuppData nonZSdata( bank );

  // Get new SmartIDs
  RichDAQ::SmartIDs IDs;
  nonZSdata.fillSmartIDs( linkN.rich(),linkN.panel(),
                          linkN.pdRow(),linkN.pdCol(),IDs );

  // Create RichDigits with new RichSmartIDs
  for ( RichDAQ::SmartIDs::const_iterator iID = IDs.begin();
        iID != IDs.end(); ++iID ) {
    if ( msgLevel(MSG::VERBOSE) ) verbose() << "  Hit " << *iID << endreq;
    m_digits->insert( new RichDigit(), *iID );
  }

  return digitCount;
}

//  Finalize
StatusCode RawBufferToRichDigitsAlg::finalize()
{
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
