
//-----------------------------------------------------------------------------
/** @file RichRawBufferToSmartIDsTool.cpp
 *
 * Implementation file for class : RichRawBufferToSmartIDsTool
 *
 * CVS Log :-
 * $Id: RichRawBufferToSmartIDsTool.cpp,v 1.2 2004-10-30 21:45:57 jonrob Exp $
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2004/10/30 19:13:05  jonrob
 * Reworking RawBuffer decoding as a tool, to allow reconstruction to skip RichDigit creation
 *
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichRawBufferToSmartIDsTool.h"

// Declaration of the Tool Factory
static const  ToolFactory<RichRawBufferToSmartIDsTool>          s_factory ;
const        IToolFactory& RichRawBufferToSmartIDsToolFactory = s_factory ;

// Standard constructor
RichRawBufferToSmartIDsTool::RichRawBufferToSmartIDsTool( const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent )
  : RichToolBase       ( type, name, parent ),
    m_newEvent         ( true ),
    m_rawEvent         ( 0    )
{

  declareInterface<IRichRawBufferToSmartIDsTool>(this);

  declareProperty( "RawEventLocation",
                   m_rawEventLoc = RawEventLocation::Default );

}

StatusCode RichRawBufferToSmartIDsTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // Make sure we are ready for a new event
  InitNewEvent();

  return StatusCode::SUCCESS;
}

StatusCode RichRawBufferToSmartIDsTool::finalize()
{

  // Execute base class method
  return RichToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichRawBufferToSmartIDsTool::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
}

RawEvent * RichRawBufferToSmartIDsTool::rawEvent() const
{
  if ( !m_rawEvent ) {

    // Try and load from TES. If it exists return, overwise try to create one
    SmartDataPtr<RawEvent> rawEventTES( evtSvc(), m_rawEventLoc );
    if ( rawEventTES ) { m_rawEvent = rawEventTES; }
    else {
      debug() << "Creating RawEvent from RawBuffer" << endreq;

      // Retrieve the RawBuffer
      SmartDataPtr<RawBuffer> rawBuffer( evtSvc(), RawBufferLocation::Default );
      if ( !rawBuffer ) { m_rawEvent = 0; Exception("Unable to locate RawBuffer"); }

      // make new RawEvent and put into TES
      m_rawEvent = new RawEvent( rawBuffer );
      if ( !m_rawEvent ) {
        m_rawEvent = 0;
        Exception("Unable to allocate memory to RawEvent");
      }
      put( m_rawEvent, RawEventLocation::Default );
    }

  }

  return m_rawEvent;
}

const RichSmartID::Vector & RichRawBufferToSmartIDsTool::allRichSmartIDs() const
{
  if ( m_newEvent ) {
    m_newEvent = false;
    fillRichSmartIDs();
  }
  return m_smartIDs;
}

void RichRawBufferToSmartIDsTool::fillRichSmartIDs() const
{

  // Get the banks for the RichDigits
  const RichDAQ::RAWBanks & richBanks = rawEvent()->banks( RawBuffer::Rich );

  // Purge SmartID container
  m_smartIDs.clear();

  // make a guess at a reserved size
  m_smartIDs.reserve( richBanks.size() * 5 );

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

  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Decoded " << m_smartIDs.size()
            << " RichSmartIDs from " << richBanks.size() << " Raw banks" << endreq;
  }

}

void
RichRawBufferToSmartIDsTool::decodeZeroSuppressedBank( const RawBank & bank ) const
{

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

  if ( digitCount > 0 ) {

    // Loop over data fields (Skip first header field)
    RichDAQ::ShortType nDigitsMade = 0;
    for ( int iEntry = 1; iEntry < bank.dataSize(); ++iEntry ) {

      // Get triplet data
      const RichZSHitTriplet triplet( bank.data()[iEntry] );
      if ( msgLevel(MSG::VERBOSE) ) verbose() << " Decoding triplet " << triplet << endreq;

      // Make first smartid from triplet
      m_smartIDs.push_back( RichSmartID( linkN.rich(), linkN.panel(),
                                         linkN.pdRow(), linkN.pdCol(),
                                         triplet.row0(), triplet.col0() ) );
      ++nDigitsMade;
      if ( nDigitsMade == digitCount ) break;

      // Make second smartid from triplet
      m_smartIDs.push_back( RichSmartID( linkN.rich(), linkN.panel(),
                                         linkN.pdRow(), linkN.pdCol(),
                                         triplet.row1(), triplet.col1() ) );
      ++nDigitsMade;
      if ( nDigitsMade == digitCount ) break;

      // Make third smartid from triplet
      m_smartIDs.push_back( RichSmartID( linkN.rich(), linkN.panel(),
                                         linkN.pdRow(), linkN.pdCol(),
                                         triplet.row2(), triplet.col2() ) );
      ++nDigitsMade;
      if ( nDigitsMade == digitCount ) break;

    }

  }

}

void
RichRawBufferToSmartIDsTool::decodeNonZeroSuppressedBank( const RawBank & bank ) const
{

  // Get the link identifier
  const RichDAQLinkNumber linkN( bank.bankSourceID() );

  if ( msgLevel(MSG::VERBOSE) ) {

    // Get the bank header
    const RichDAQHeaderPD bankHeader( bank.data()[0] );

    const RichSmartID pdID(linkN.rich(),linkN.panel(),linkN.pdRow(),linkN.pdCol(),0,0);
    verbose() << "Decoding " << bankHeader.hitCount() << " non-zero suppressed hits for PD "
              << pdID << endreq
              << " Header : " << bankHeader << endreq;

  }

  // Create a block of non-zero suppressed data from RawBank
  const RichNonZeroSuppData nonZSdata( bank );

  // Put smart IDs into container
  nonZSdata.fillSmartIDs( linkN.rich(),linkN.panel(),
                          linkN.pdRow(),linkN.pdCol(), m_smartIDs );

}
