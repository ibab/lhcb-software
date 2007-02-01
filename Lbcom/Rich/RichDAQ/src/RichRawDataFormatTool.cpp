
//-----------------------------------------------------------------------------
/** @file RichRawDataFormatTool.cpp
 *
 *  Implementation file for class : Rich::RawDataFormatTool
 *
 *  CVS Log :-
 *  $Id: RichRawDataFormatTool.cpp,v 1.42 2007-02-01 17:42:30 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 2004-12-18
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"

// local
#include "RichRawDataFormatTool.h"

// Rich DAQ software
using namespace Rich::DAQ;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( RawDataFormatTool );

// private namespace
namespace
{
  /// number of 32 bit words in the L1 header
  static const unsigned int nL1HeaderWords = 2;
}

// Standard constructor
RawDataFormatTool::RawDataFormatTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichToolBase    ( type, name , parent ),
    m_richSys       ( NULL                ),
    m_rawEvent      ( NULL                ),
    m_odin          ( NULL                ),
    m_timeTool      ( NULL                ),
    m_evtCount      ( 0                   ),
    m_hasBeenCalled ( false               ),
    m_maxHPDOc      ( 999999              )
{
  // interface
  declareInterface<IRawDataFormatTool>(this);

  // job opts
  declareProperty( "ZeroSuppressHitCut", m_zeroSuppresCut = 96 );
  declareProperty( "RawEventLocation",
                   m_rawEventLoc = LHCb::RawEventLocation::Default );
  declareProperty( "PrintSummary",       m_summary   = true );
  declareProperty( "MaxHPDOccupancy",    m_maxHPDOc );
  declareProperty( "DumpRawBanks",       m_dumpBanks = false );
  declareProperty( "UseZeroSuppression", m_zeroSupp  = true );
  declareProperty( "UseExtendedFormat",  m_extendedFormat = false );
  declareProperty( "DecodeUsingODIN",    m_decodeUseOdin = true );
}

// Destructor
RawDataFormatTool::~RawDataFormatTool() {}

StatusCode RawDataFormatTool::initialize()
{
  // Initialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocation::RichSystem );

  // create a dummy L1data object with an empty vector for each L1 board
  m_dummyMap.clear();
  for ( Level1IDs::const_iterator iID = m_richSys->level1IDs().begin();
        iID != m_richSys->level1IDs().end(); ++iID )
  {
    m_dummyMap[ *iID ];
  }
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created " << m_dummyMap.size() << " entries in empty L1 map : L1IDs = "
            << m_richSys->level1IDs() << endreq;
  }

  // if suppression is less than max possible number of (ALICE) hits, print a message.
  if ( m_maxHPDOc < BitsPerDataWord * MaxDataSizeALICE )
  {
    info() << "Will suppress HPDs with more than " << m_maxHPDOc << " digits" << endreq;
  }

  if ( m_extendedFormat ) info() << "Will encode RawEvent using extended HPD format" << endreq;

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  incSvc()->addListener( this, IncidentType::EndEvent   );

  return sc;
}

// Method that handles various Gaudi "software events"
void RawDataFormatTool::handle ( const Incident& incident )
{
  // Update prior to start of new event.
  if ( IncidentType::BeginEvent == incident.type() )
  {
    InitEvent();
  }
  // End of event
  else if ( IncidentType::EndEvent == incident.type() )
  {
    FinishEvent();
  }
}

StatusCode RawDataFormatTool::finalize()
{

  // Print out L1 information
  if ( m_summary )
  {
    printL1Stats( m_l1decodeSummary, "RICH Level 1 : Decoding Summary" );
    printL1Stats( m_l1encodeSummary, "RICH Level 1 : Encoding Summary" );
  }

  // base class finalize
  return RichToolBase::finalize();
}

void
RawDataFormatTool::printL1Stats( const L1TypeCount & count,
                                 const std::string & title ) const
{

  // See if any summary info has been collected
  if ( !count.empty() )
  {

    // Statistical calculators
    const StatDivFunctor occ1("%7.2f +-%5.2f"), occ2("%8.2f +-%5.2f");

    // Printout
    info() << "==========================================================================================================" << endreq
           << "                             " << title << " : " << m_evtCount << " events" << endreq;

    std::vector< unsigned long > totWordSize(Rich::NRiches,0), totBanks(Rich::NRiches,0), totHits(Rich::NRiches,0);
    Rich::DetectorType lastrich = Rich::InvalidDetector;
    for ( L1TypeCount::const_iterator iL1C = count.begin(); iL1C != count.end(); ++iL1C )
    {
      const Level1ID      L1ID   = (*iL1C).first.second;
      const BankVersion version  = (*iL1C).first.first;
      const Rich::DetectorType rich       = m_richSys->richDetector( L1ID );
      const unsigned long nBanks          = (*iL1C).second.first;
      totBanks[rich]                     += nBanks;
      const unsigned long words           = (*iL1C).second.second.first;
      totWordSize[rich]                  += words;
      const unsigned long hits            = (*iL1C).second.second.second;
      totHits[rich]                      += hits;

      if ( rich != lastrich )
      {
        lastrich = rich;
        info() << "----------------------------------------------------------------------------------------------------------" << endreq;
      }

      info() << " " << rich << " L1 " << format("%3i",L1ID.data()) << " V" << format("%3i",version) << " | L1 size ="
             << occ1(nBanks,m_evtCount) << " hpds :"
             << occ2(words,m_evtCount) << " words :"
             << occ2(hits,m_evtCount) << " hits / event" << endreq;
    }

    info() << "----------------------------------------------------------------------------------------------------------" << endreq;
    info() << " Rich1 Average     | L1 size =" << occ1(totBanks[Rich::Rich1],m_evtCount) << " hpds :"
           << occ2(totWordSize[Rich::Rich1],m_evtCount) << " words :"
           << occ2(totHits[Rich::Rich1],m_evtCount) << " hits / event" << endreq;
    info() << " Rich2 Average     | L1 size =" << occ1(totBanks[Rich::Rich2],m_evtCount) << " hpds :"
           << occ2(totWordSize[Rich::Rich2],m_evtCount) << " words :"
           << occ2(totHits[Rich::Rich2],m_evtCount) << " hits / event" << endreq;

    info() << "==========================================================================================================" << endreq;

  } // end stats available

}

// Create data bank of correct version
// This function knows which bank objects to create for each version number
const HPDDataBank *
RawDataFormatTool::createDataBank( const LHCb::RichSmartID::Vector & smartIDs,
                                   const BankVersion version,
                                   const LHCb::ODIN * odin ) const
{

  // flag the tool as having been used this event
  m_hasBeenCalled = true;

  // pointer to HPD data block
  HPDDataBank * dataBank = 0;

  // Check bank is not empty
  if ( smartIDs.empty() )
  {
    Warning( "RichSmartID vector is empty -> No data bank" );
    return dataBank;
  }

  // Level 0 ID
  const Level0ID l0ID = m_richSys->level0ID( smartIDs.front().hpdID() );

  if      ( LHCb5 == version )
  {
    // NB : Does not yet support creation of ALICE mode data blocks...  To Be Done.

    // First try the ZS format
    RichDAQ_LHCb5::ZeroSuppLHCb * zsData =
      new RichDAQ_LHCb5::ZeroSuppLHCb( l0ID, smartIDs, m_extendedFormat, odin );

    // If too big, use non ZS instead
    if ( !m_zeroSupp || zsData->tooBig() )
    {
      delete zsData;
      dataBank = (HPDDataBank*) new RichDAQ_LHCb5::NonZeroSuppLHCb( l0ID, smartIDs,
                                                                    m_extendedFormat, odin );
    }
    else
    {
      dataBank = (HPDDataBank*)zsData;
    }

  }
  else if ( LHCb4 == version )
  {
    // NB : Does not support creation of ALICE mode data blocks.

    // First try the ZS format
    RichDAQ_LHCb4::ZeroSuppLHCb * zsData = new RichDAQ_LHCb4::ZeroSuppLHCb( l0ID, smartIDs );

    // If too big, use non ZS instead
    if ( !m_zeroSupp || zsData->tooBig() )
    {
      delete zsData;
      dataBank = (HPDDataBank*) new RichDAQ_LHCb4::NonZeroSuppLHCb( l0ID, smartIDs );
    }
    else
    {
      dataBank = (HPDDataBank*)zsData;
    }

  }
  else if ( LHCb3 == version )
  {
    // NB : Does not support creation of ALICE mode data blocks.

    // First try the ZS format
    RichDAQ_LHCb3::ZeroSuppLHCb * zsData = new RichDAQ_LHCb3::ZeroSuppLHCb( l0ID, smartIDs );

    // If too big, use non ZS instead
    if ( !m_zeroSupp || zsData->tooBig() )
    {
      delete zsData;
      dataBank = (HPDDataBank*) new RichDAQ_LHCb3::NonZeroSuppLHCb( l0ID, smartIDs );
    }
    else
    {
      dataBank = (HPDDataBank*)zsData;
    }

  }
  else if ( LHCb2 == version )
  {
    // First try the ZS format
    RichDAQ_LHCb2::ZeroSuppLHCb * zsData = new RichDAQ_LHCb2::ZeroSuppLHCb( l0ID, smartIDs );

    // If too big, use non ZS instead
    if ( !m_zeroSupp || zsData->tooBig() )
    {
      delete zsData;
      dataBank = (HPDDataBank*) new RichDAQ_LHCb2::NonZeroSuppLHCb( l0ID, smartIDs );
    }
    else
    {
      dataBank = (HPDDataBank*)zsData;
    }

  }
  else if ( LHCb1 == version )
  {

    // Decide to zero suppress or not depending on number of hits
    if ( m_zeroSupp && smartIDs.size() < m_zeroSuppresCut )
    {
      dataBank = (HPDDataBank*) new RichDAQ_LHCb1::ZeroSuppLHCb( l0ID, smartIDs );
    }
    else
    {
      dataBank = (HPDDataBank*) new RichDAQ_LHCb1::NonZeroSuppLHCb( l0ID, smartIDs );
    }

  }
  else
  {
    Warning ( "Unknown RICH Raw Buffer version " +
              boost::lexical_cast<std::string>(version) + " -> No data bank" );
    return NULL;
  }

  if ( msgLevel(MSG::VERBOSE) )
  {
    // Print out SmartIDs to encode
    verbose() << "   Creating encoding HPD " << smartIDs.front().hpdID()
              << " bank from " << smartIDs.size() << " RichSmartIDs :-" << endreq;
    for ( LHCb::RichSmartID::Vector::const_iterator iID = smartIDs.begin();
          iID != smartIDs.end(); ++iID )
    {
      verbose() << "    " << *iID << endreq;
    }
    // Printout this bank
    verbose() << *dataBank << endreq;
  }

  return dataBank;
}

// Create data bank of correct version
// This function knows which bank objects to create for each version number
const HPDDataBank *
RawDataFormatTool::createDataBank( const LongType * dataStart,
                                   const unsigned int dataSize,
                                   const BankVersion version ) const
{
  HPDDataBank * dataBank = NULL;

  if ( LHCb5 == version )
  {

    // Header
    const RichDAQ_LHCb5::Header header( dataStart );

    // Decide to zero suppress or not depending on number of hits
    if ( header.zeroSuppressed() )
    {
      dataBank = (HPDDataBank*) new RichDAQ_LHCb5::ZeroSuppLHCb( dataStart );
    }
    else
    {
      if ( header.aliceMode() )
      {
        dataBank = (HPDDataBank*) new RichDAQ_LHCb5::NonZeroSuppAlice( dataStart );
      }
      else
      {
        dataBank = (HPDDataBank*) new RichDAQ_LHCb5::NonZeroSuppLHCb( dataStart );
      }
    }

  }
  else if ( LHCb4 == version )
  {

    // Header
    const RichDAQ_LHCb4::Header header( dataStart );

    // Decide to zero suppress or not depending on number of hits
    if ( header.zeroSuppressed() )
    {
      dataBank = (HPDDataBank*) new RichDAQ_LHCb4::ZeroSuppLHCb( dataStart );
    }
    else
    {
      if ( header.aliceMode() )
      {
        dataBank = (HPDDataBank*) new RichDAQ_LHCb4::NonZeroSuppAlice( dataStart );
      }
      else
      {
        dataBank = (HPDDataBank*) new RichDAQ_LHCb4::NonZeroSuppLHCb( dataStart );
      }
    }

  }
  else if ( LHCb3 == version )
  {

    // Header
    const RichDAQ_LHCb3::Header header( dataStart );

    // Decide to zero suppress or not depending on number of hits
    if ( header.zeroSuppressed() )
    {
      dataBank = (HPDDataBank*) new RichDAQ_LHCb3::ZeroSuppLHCb( dataStart );
    }
    else
    {
      if ( header.aliceMode() )
      {
        dataBank = (HPDDataBank*) new RichDAQ_LHCb3::NonZeroSuppAlice( dataStart );
      }
      else
      {
        dataBank = (HPDDataBank*) new RichDAQ_LHCb3::NonZeroSuppLHCb( dataStart );
      }
    }

  }
  else if ( LHCb2 == version )
  {

    // Header
    const RichDAQ_LHCb2::Header header( dataStart );

    // Decide to zero suppress or not depending on number of hits
    if ( header.zeroSuppressed() )
    {
      dataBank = (HPDDataBank*) new RichDAQ_LHCb2::ZeroSuppLHCb( dataStart, dataSize );
    }
    else
    {
      dataBank = (HPDDataBank*) new RichDAQ_LHCb2::NonZeroSuppLHCb( dataStart );
    }

  }
  else if ( LHCb1 == version )
  {

    // Header
    const RichDAQ_LHCb1::Header header( dataStart );

    // Decide to zero suppress or not depending on number of hits
    if ( header.zeroSuppressed() )
    {
      dataBank = (HPDDataBank*) new RichDAQ_LHCb1::ZeroSuppLHCb( dataStart, dataSize );
    }
    else
    {
      dataBank = (HPDDataBank*) new RichDAQ_LHCb1::NonZeroSuppLHCb( dataStart );
    }

  }
  else
  {
    Exception ( "Unknown RICH Raw Buffer version " +
                boost::lexical_cast<std::string>(version) + " -> No data bank" );
  }

  // Printout this bank
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Created HPD Data Bank for Decoding :-" << endreq;
    verbose() << *dataBank << endreq;
  }

  return dataBank;
}

void RawDataFormatTool::fillRawEvent( const LHCb::RichSmartID::Vector & smartIDs,
                                      const BankVersion version ) const
{

  // new rich data map
  L1Map L1Data = m_dummyMap;

  // Loop over digits and sort according to L1 and HPD
  for ( LHCb::RichSmartID::Vector::const_iterator iDigit = smartIDs.begin();
        iDigit != smartIDs.end(); ++iDigit )
  {

    // Get the HPD SmartID
    const LHCb::RichSmartID hpdID = (*iDigit).hpdID();

    // Get Level 1 board number and input
    const Level1ID    L1ID    = m_richSys->level1ID       ( *iDigit );
    const Level1Input L1Input = m_richSys->level1InputNum ( *iDigit );

    // Get reference to correct data group
    IngressMap & Ingress   = L1Data  [ L1ID                ];
    HPDHashMap & hpdMap    = Ingress [ L1Input.ingressID() ];

    // Finally, insert this pixel into correct place
    hpdMap[ HPDInfo(hpdID,L1Input) ].push_back( *iDigit );

  }

  // Get some ODIN info
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "ODIN : " << *odin() << endreq;
  }

  // Loop over Level1 board and make a RAWBank for each
  for ( L1Map::const_iterator iL1 = L1Data.begin();
        iL1 != L1Data.end(); ++iL1 )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Creating Bank for L1 board " << (*iL1).first << endreq;

    // Make a new data bank
    RAWBank dataBank;

    // local hit and HPD count
    ShortType nHits(0), nHPDs(0);

    // Loop over ingresses for this L1 board
    for ( IngressMap::const_iterator iIngress = (*iL1).second.begin();
          iIngress != (*iL1).second.end(); ++iIngress )
    {

      // If 2007 data format, add Ingress header word to the raw bank
      // Eventually, should be done by default for all formats
      // (once DC04/06 compibility is no longer needed)
      // In this case this if clause can be removed
      if ( version != LHCb0 && version != LHCb1 &&
           version != LHCb2 && version != LHCb3 && version != LHCb4 )
      {
        const L1IngressHeader ingressWord( (*iIngress).second, odin() );
        ingressWord.fillRAWBank(dataBank);
        if ( msgLevel(MSG::DEBUG) )
          debug() << " Ingress : " << ingressWord << endreq;
      }

      if ( msgLevel(MSG::DEBUG) )
        debug() << "  Found " << (*iIngress).second.size() << " HPD(s) with data" << endreq;

      // Loop over each active HPD for this Level1 board ingress and fill RAWBank
      for ( HPDHashMap::const_iterator iHPD = (*iIngress).second.begin();
            iHPD != (*iIngress).second.end(); ++iHPD )
      {
        // Get raw data bank for this HPD, and fill into RAWBank
        const HPDDataBank * hpdData = createDataBank( (*iHPD).second, version, odin() );
        if ( hpdData )
        {
          hpdData->fillRAWBank( dataBank );
          delete hpdData;
          nHits += (*iHPD).second.size();
          ++nHPDs;
        }
        else
        {
          Warning( "No RICH Data Bank Created" );
        }

      } // end photon detector loop

    } // ingress loop

    // Add this bank to the Raw buffer
    rawEvent()->addBank( (*iL1).first.data(), LHCb::RawBank::Rich, version, dataBank );

    if ( m_summary )
    {
      // Count the number of banks and size
      L1CountAndSize & cands = m_l1encodeSummary[ L1IDandV(version,(*iL1).first) ];
      // Increment bank size
      cands.second.first += nL1HeaderWords+dataBank.size(); // L1 headers + data words
      // Increment hit occupancy
      cands.second.second += nHits;
      // Count number of HPD banks
      cands.first += nHPDs;
    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Encoded " << format("%2i",(*iL1).second.size()) << " HPDs into Level1 Bank "
              << format("%2i",(*iL1).first.data()) << " : Size "
              << format("%4i",nL1HeaderWords+dataBank.size())
              << " words : Version " << version << endreq;
    }

  } // Level1 loop

}

void RawDataFormatTool::decodeToSmartIDs( const LHCb::RawBank & bank,
                                          PDMap & smartIDs ) const
{
  // Check this is a RICH bank
  if ( bank.type() != LHCb::RawBank::Rich )
  {
    Exception( "BankType is not RICH : type = " +
               boost::lexical_cast<std::string>(bank.type()) );
  }

  // if configured, dump raw event before decoding
  if      ( msgLevel(MSG::VERBOSE) ) { dumpRawBank( bank, verbose() ); }
  else if ( m_dumpBanks            ) { dumpRawBank( bank, info()    ); }

  // Get bank version
  const BankVersion version = bankVersion( bank );

  // Now, delegate the work to a version of the decoding appropriate to the version

  if ( version == LHCb5 ) // latest version
  {
    decodeToSmartIDs_2007(bank,smartIDs);
  }
  else if ( version == LHCb3 || version == LHCb4 ) // RICH 2006 Testbeam
  {
    decodeToSmartIDs_2006TB(bank,smartIDs);
  }
  else if ( version == LHCb0 ||
            version == LHCb1 ||
            version == LHCb2 )  // DC04 or DC06
  {
    decodeToSmartIDs_DC0406(bank,smartIDs);
  }
  else // Some problem ...
  {
    std::ostringstream mess;
    mess << "Unknown RICH L1 version number " << version;
    Exception( mess.str() );
  }

}

void RawDataFormatTool::decodeToSmartIDs_2007( const LHCb::RawBank & bank,
                                               Rich::DAQ::PDMap & smartIDs ) const
{
  // Get L1 ID
  const Level1ID L1ID ( bank.sourceID() );

  // HPD count
  unsigned int nHPDbanks(0), decodedHits(0);

  // Data bank size in words
  const int bankSize = bank.size() / 4;

  // Get bank version
  const BankVersion version = bankVersion( bank );

  if ( bankSize > 0 )
  {

    // ... otherwise, must have at least 2 entries
    if ( bankSize < 2 )
    {
      Exception( "Non-empty RICH Bank size is less than 2 !" );
    }

    // flag the tool as having been used this event
    m_hasBeenCalled = true;

    // Loop over bank, find headers and produce a data bank for each
    // Fill data into RichSmartIDs
    long lineC(0), ingressNum(0);
    while ( lineC < bankSize )
    {

      // First, the ingress header word (and increment to the next line)
      const L1IngressHeader ingressWord( bank.data()[lineC++] );
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Level1 ID = " << L1ID << " : Ingress " << ingressNum << " "
                << ingressWord << endreq;
      }

      // Compare Ingress header to the ODIN
      const bool odinOK = ( !m_decodeUseOdin ? true :
                            compareIDs( ingressWord.eventID(), EventID(odin()->eventNumber()) ) &&
                            compareIDs( ingressWord.bxID(),    BXID   (odin()->orbitNumber()) ) );
      if ( !odinOK )
      {
        std::ostringstream mess;
        mess << "ODIN Mismatch : L1ID " << L1ID
             << " : ODIN EvID=" << odin()->eventNumber() << " BxID=" << odin()->orbitNumber()
             << " : L1IngressHeader " << ingressWord
             << " EvID= " << ingressWord.eventID() << " BxID=" << ingressWord.bxID()
             << " -> Data Suppressed";
        Error( mess.str(), 999999 );
      }

      // How many active HPDs ?
      const ShortType nHPDs = ingressWord.numActiveHPDs();

      // Check the Ingress supression flag
      if ( ingressWord.hpdsSuppressed() )
      {
        std::ostringstream mess;
        mess << "L1 " << L1ID << " : Ingress " << ingressNum << " "
             << ingressWord << " is suppressed : " << nHPDs << " HPDs";
        Warning( mess.str() );
      }
      else
      {
        // Ingress is OK, so read HPD data

        // Loop over active HPDs
        for ( ShortType iHPD = 0; iHPD < nHPDs; ++iHPD )
        {

          // Create data bank and decode into RichSmartIDs
          std::auto_ptr<const HPDDataBank> hpdBank ( createDataBank( &bank.data()[lineC], // pointer to start of data
                                                                     0, // Not needed here (to be removed). Must be 0 though
                                                                     version ) );

          // Only try and decode this HPD if ODIN test was OK
          if ( odinOK && !hpdBank->suppressed() )
          {

            // get HPD RichSmartID
            const LHCb::RichSmartID hpdID = m_richSys->richSmartID( hpdBank->level0ID() );

            // local hit count
            unsigned int hpdHitCount(0);

            // Vector to decode into
            LHCb::RichSmartID::Vector & newids = smartIDs[hpdID];

            // Compare Event IDs for errors
            bool OK = ( hpdBank->suppressed() ? true :
                        compareIDs( ingressWord.eventID(), hpdBank->eventID() ) );
            if ( !OK )
            {
              std::ostringstream mess;
              mess << "EventID Mismatch : HPD " << hpdID << " L1IngressHeader = " << ingressWord.eventID()
                   << " HPDHeader = " << hpdBank->eventID();
              Error( mess.str() );
            }
            else
            {

              // decode to smartIDs
              hpdHitCount = hpdBank->fillRichSmartIDs( newids, hpdID );

              // Do data integrity checks
              OK = hpdBank->checkDataIntegrity(newids,warning());
              if ( !OK ) Warning( "HPD data block failed integrity check" );

              if ( msgLevel(MSG::VERBOSE) )
              {
                // printout decoded RichSmartIDs
                verbose() << " Decoded RichSmartIDs :-" << endreq;
                for ( LHCb::RichSmartID::Vector::const_iterator iID = newids.begin();
                      iID != newids.end(); ++iID )
                {
                  verbose() << "   " << *iID << endreq;
                }
              }

            } // event IDs OK

            // is data OK
            if ( OK )
            {

              // apply suppression of high occupancy HPDs
              if ( hpdHitCount < m_maxHPDOc )
              {
                ++nHPDbanks;
                decodedHits += hpdHitCount;
              }
              else
              {
                std::ostringstream hpd;
                hpd << hpdID.panelID();
                Warning( "Suppressed HPD in "+hpd.str(), StatusCode::SUCCESS, 0 );
                smartIDs[hpdID].clear();
              }

            }
            else
            {
              // decoding error ....
              error() << "Error in decoding -> Data is rejected for HPD " << hpdID << endreq;

              error() << " -> Dump of raw L1 data :-" << endreq;
              dumpRawBank( bank, error() );

              error() << " -> Badly decoded HPD :-" << endreq;
              error() << *hpdBank << endreq;
            }

          } // ODIN OK and not suppressed

          // Increment line number to next data block
          lineC += hpdBank->nTotalWords();

        } // active HPDs

      } // ingress not suppressed

      ++ingressNum; // count ingresses
    } // bank while loop

  } // data bank not empty

  if ( m_summary )
  {
    // Count the number of banks and size
    L1CountAndSize & cands = m_l1decodeSummary[ L1IDandV(version,L1ID) ];
    // Increment bank size
    cands.second.first += nL1HeaderWords + (bank.size()/4); // 2 L1 headers + data words
    // Increment hit occupancy
    cands.second.second += decodedHits;
    // Count number of HPD banks
    cands.first += nHPDbanks;
  }

  // debug printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Decoded " << format("%2i",nHPDbanks) << " HPDs from Level1 Bank "
            << format("%2i",L1ID.data())
            << " : Size " << format("%4i",nL1HeaderWords+(bank.size()/4)) << " words : Version "
            << version << endreq;
  }

}

void RawDataFormatTool::decodeToSmartIDs_2006TB( const LHCb::RawBank & bank,
                                                 Rich::DAQ::PDMap & smartIDs ) const
{

  // Get L1 ID
  const Level1ID L1ID ( bank.sourceID() );

  // HPD count
  unsigned int nHPDbanks(0), decodedHits(0);

  // Data bank size in words
  const int bankSize = bank.size() / 4;

  // Get bank version
  const BankVersion version = bankVersion( bank );

  if ( bankSize > 0 )
  {

    // ... otherwise, must have at least 2 entries
    if ( bankSize < 2 )
    {
      Exception( "Non-empty RICH Bank size is less than 2 !" );
    }

    // flag the tool as having been used this event
    m_hasBeenCalled = true;

    // Loop over bank, find headers and produce a data bank for each
    // Fill data into RichSmartIDs
    long lineC(0);
    while ( lineC < bankSize )
    {

      // Create data bank and decode into RichSmartIDs
      std::auto_ptr<const HPDDataBank> hpdBank ( createDataBank( &bank.data()[lineC], // pointer to start of data
                                                                 0, // Not needed here (to be removed). Must be 0 though
                                                                 version ) );

      // get HPD RichSmartID
      const LHCb::RichSmartID hpdID = m_richSys->richSmartID( hpdBank->level0ID() );

      // decode to smartIDs
      LHCb::RichSmartID::Vector & newids = smartIDs[hpdID];
      const unsigned int hpdHitCount = hpdBank->fillRichSmartIDs( newids, hpdID );

      // Do data integrity checks
      const bool OK = hpdBank->checkDataIntegrity(newids,warning());

      if ( !OK || msgLevel(MSG::VERBOSE) )
      {
        // printout decoded RichSmartIDs
        verbose() << " Decoded RichSmartIDs :-" << endreq;
        for ( LHCb::RichSmartID::Vector::const_iterator iID = smartIDs[hpdID].begin();
              iID != smartIDs[hpdID].end(); ++iID )
        {
          verbose() << "   " << *iID << endreq;
        }
      }

      // is data OK
      if ( OK )
      {

        // apply suppression of high occupancy HPDs
        if ( hpdHitCount < m_maxHPDOc )
        {
          ++nHPDbanks;
          decodedHits += hpdHitCount;
        }
        else
        {
          std::ostringstream hpd;
          hpd << hpdID.panelID();
          Warning( "Suppressed HPD in "+hpd.str(), StatusCode::SUCCESS, 0 );
          smartIDs[hpdID].clear();
        }

      }
      else
      {
        // decoding error ....
        error() << "Corruption in decoding -> Data is rejected for HPD " << hpdID << endreq;

        error() << " -> Dump of offending raw L1 data :-" << endreq;
        dumpRawBank( bank, error() );

        error() << " -> Badly decoded HPD :-" << endreq;
        error() << *hpdBank << endreq;
      }

      // Increment line number to next data block
      lineC += hpdBank->nTotalWords();

    } // bank while loop

  } // data bank not empty

  if ( m_summary )
  {
    // Count the number of banks and size
    L1CountAndSize & cands = m_l1decodeSummary[ L1IDandV(version,L1ID) ];
    // Increment bank size
    cands.second.first += nL1HeaderWords + (bank.size()/4); // 2 L1 headers + data words
    // Increment hit occupancy
    cands.second.second += decodedHits;
    // Count number of HPD banks
    cands.first += nHPDbanks;
  }

  // debug printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Decoded " << format("%2i",nHPDbanks) << " HPDs from Level1 Bank "
            << format("%2i",L1ID.data())
            << " : Size " << format("%4i",nL1HeaderWords+(bank.size()/4)) << " words : Version "
            << version << endreq;
  }

}

void RawDataFormatTool::decodeToSmartIDs_DC0406( const LHCb::RawBank & bank,
                                                 PDMap & smartIDs ) const
{

  // Get L1 ID
  const Level1ID L1ID ( bank.sourceID() );

  // Get max data size for LHCb mode
  const ShortType maxDataSize = MaxDataSize;

  // HPD count
  unsigned int nHPDbanks(0), decodedHits(0);

  // Data bank size in words
  const int bankSize = bank.size() / 4;

  // Get bank version
  const BankVersion version = bankVersion( bank );

  // Header type
  typedef RichDAQHeaderV1::RichDAQHeaderPD MyHeader;

  // Is this an empty bank ?
  if ( bankSize > 0 )
  {

    // ... otherwise, must have at least 2 entries
    if ( bankSize < 2 )
    {
      Exception( "Non-empty RICH Bank size is less than 2 !" );
    }

    // flag the tool as having been used this event
    m_hasBeenCalled = true;

    // Loop over bank, find headers and produce a data bank for each
    // Fill data into RichSmartIDs
    long lineC(0);
    while ( lineC < bankSize )
    {

      // Find HPD bank start
      const MyHeader header ( &bank.data()[lineC] );

      // Is this a true header
      if ( header.startPD() )
      {

        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " Found HPD header at line " << lineC << " of " << bankSize << endreq;

        // Store start line for header
        const long lineHeader = lineC;

        // Find last line of block
        long lineLast = lineC;
        if ( header.zeroSuppressed() )
        {
          // For ZS blocks, have to search for the hext header to define the block length

          bool cont = true;
          while ( cont && lineC < bankSize )
          {
            ++lineC;
            // Test if this is the last line of data or a new header word
            if ( lineC == bankSize || MyHeader(&bank.data()[lineC]).startPD() )
            {
              lineLast = lineC-1;
              cont = false;
            }
          }

          if ( msgLevel(MSG::VERBOSE) )
            verbose() << "  -> Bank is zero surpressed : ends at " << lineLast << endreq;

        }
        else
        {
          // non-ZS blocks have fixed length, so skip straight to the end

          lineC   += 1 + maxDataSize; // data block + header
          lineLast = lineC - 1;

          if ( msgLevel(MSG::VERBOSE) )
            verbose() << "  -> Bank is non zero surpressed : ends at " << lineLast << endreq;

        }

        // Check data size
        const unsigned int dataSize = lineLast-lineHeader;
        if ( dataSize < 1 )
        {
          Exception( "Invalid HPD data block size : " + boost::lexical_cast<std::string>(dataSize) );
        }

        // Create data bank and decode into RichSmartIDs
        const HPDDataBank * hpdBank ( createDataBank( &bank.data()[lineHeader],
                                                      dataSize,
                                                      version ) );
        // get hit count
        const unsigned int hpdHitCount = hpdBank->hitCount();
        // get HPD RichSmartID
        const LHCb::RichSmartID hpdID = m_richSys->richSmartID( hpdBank->level0ID() );
        // apply suppression of high occupancy HPDs
        if ( hpdHitCount < m_maxHPDOc )
        {
          hpdBank->fillRichSmartIDs( smartIDs[hpdID], hpdID );
          ++nHPDbanks;
          decodedHits += hpdHitCount;
        }
        else
        {
          std::ostringstream hpd;
          hpd << hpdID.panelID();
          Warning( "Suppressed HPD in "+hpd.str(), StatusCode::SUCCESS, 0 );
        }
        delete hpdBank;

      }
      else // Not a data header line
      {
        ++lineC;
      }

    } // bank while loop

  } // data bank not empty

  if ( m_summary )
  {
    // Count the number of banks and size
    L1CountAndSize & cands = m_l1decodeSummary[ L1IDandV(version,L1ID) ];
    // Increment bank size
    cands.second.first += nL1HeaderWords + (bank.size()/4); // 2 L1 headers + data words
    // Increment hit occupancy
    cands.second.second += decodedHits;
    // Count number of HPD banks
    cands.first += nHPDbanks;
  }

  // debug printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Decoded " << format("%2i",nHPDbanks) << " HPDs from Level1 Bank "
            << format("%2i",L1ID.data())
            << " : Size " << format("%4i",nL1HeaderWords+(bank.size()/4)) << " words : Version "
            << version << endreq;
  }

}

void
RawDataFormatTool::decodeToSmartIDs( PDMap & smartIDs ) const
{

  // Get the banks for the Rich
  const LHCb::RawBank::Vector & richBanks = rawEvent()->banks( LHCb::RawBank::Rich );

  // Purge SmartID container
  smartIDs.clear();

  // Loop over data banks
  for ( LHCb::RawBank::Vector::const_iterator iBank = richBanks.begin();
        iBank != richBanks.end(); ++iBank )
  {
    // get bank
    LHCb::RawBank * bank = *iBank;
    // test bank is OK
    if ( !bank ) Exception( "Retrieved null pointer to RawBank" );
    // Decode this bank to RichSmartIDs
    try
    {
      decodeToSmartIDs( *bank, smartIDs );
    }
    catch ( const GaudiException & expt )
    {
      // Print error message
      std::ostringstream mess;
      mess << "Error decoding bank ID=" << bank->sourceID() << " version=" << bankVersion(*bank)
           << " '" << expt.message() << "'";
      Error( mess.str() );
      // dump the full bank
      dumpRawBank( *bank, error() );
    }
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Decoded in total " << smartIDs.size()
            << " HPDs from " << richBanks.size() << " RICH Level1 bank(s)" << endreq;
  }

}

void RawDataFormatTool::dumpRawBank( const LHCb::RawBank & bank,
                                     MsgStream & os ) const
{

  // Get bank version and ID
  const Level1ID L1ID ( bank.sourceID() );
  const BankVersion version = bankVersion( bank );

  std::ostringstream magicAsHex;
  magicAsHex << std::hex << bank.magic();
  os << "RawBank version=" << version << " L1ID=" << L1ID << " size=" << bank.size()
     << " magic=" << magicAsHex.str() << endreq;

  // Printout raw data

  // Data bank size in words
  const int bankSize = bank.size() / 4;

  // Is this an empty bank ?
  if ( bankSize > 0 )
  {
    const std::string & LINES = "-------------------------------------------------------------------------------------------------------------------";

    // Bit numbers
    os << "           bit :";
    for ( int iCol = 31; iCol >= 0; --iCol )
    {
      os << format("%3i",iCol);
    }
    os << endreq;
    os << LINES << endreq;
    typedef unsigned int Dtype;
    for ( const Dtype * dataW = bank.begin<Dtype>(); dataW != bank.end<Dtype>(); ++dataW )
    {
      // Data words
      std::ostringstream hexW;
      hexW << std::hex << *dataW;
      std::string tmpW = hexW.str();
      tmpW.resize(10,' ');
      os << " -- " << tmpW << " :";
      for ( int iCol = 31; iCol >= 0; --iCol )
      {
        os << "  " << isBitOn( *dataW, iCol );
      }
      os << endreq;
    }
    os << LINES << endreq;

  }
  else
  {
    os << "  -> Bank is empty" << endreq;
  }

}

