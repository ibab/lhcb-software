
//-----------------------------------------------------------------------------
/** @file RichRawDataFormatTool.cpp
 *
 *  Implementation file for class : Rich::RawDataFormatTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 2004-12-18
 */
//-----------------------------------------------------------------------------

// local
#include "RichRawDataFormatTool.h"

// Rich DAQ software
using namespace Rich::DAQ;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( RawDataFormatTool )

// private namespace
  namespace
  {
    /// Default 'fake' HPD RichSmartID
    static const LHCb::RichSmartID s_fakeHPDID( Rich::Rich1,Rich::top,0,0 );
  }

// Standard constructor
RawDataFormatTool::RawDataFormatTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : Rich::ToolBase  ( type, name , parent  ),
    m_richSys       ( NULL                 ),
    m_timeTool      ( NULL                 ),
    m_evtCount      ( 0                    ),
    m_hasBeenCalled ( false                ),
    m_richIsActive  ( Rich::NRiches, true  ),
    m_pixelsToSuppress ( false )
{
  // interface
  declareInterface<IRawDataFormatTool>(this);

  // job opts
  declareProperty( "ZeroSuppressHitCut", m_zeroSuppresCut     = 96     );
  declareProperty( "PrintSummary",       m_summary            = true   );
  declareProperty( "MaxHPDOccupancy",    m_maxHPDOc           = 999999 );
  declareProperty( "DumpRawBanks",       m_dumpBanks          = false  );
  declareProperty( "UseZeroSuppression", m_zeroSupp           = true   );
  declareProperty( "UseExtendedFormat",  m_extendedFormat     = false  );
  declareProperty( "DecodeUsingODIN",    m_decodeUseOdin      = false  );
  declareProperty( "CheckDataIntegrity", m_checkDataIntegrity = true   );
  declareProperty( "CheckODINEventIDs",  m_checkODINEventsIDs = false  );
  declareProperty( "CheckRICHEventIDs",  m_checkRICHEventsIDs = true   );
  declareProperty( "CheckBXIDs",         m_checkBxIDs         = true   );
  declareProperty( "CheckHPDL1IDs",      m_hpdL1check         = false  );
  declareProperty( "UseFakeHPDID",       m_useFakeHPDID       = false  );
  declareProperty( "VerboseErrors",      m_verboseErrors      = false  );
  declareProperty( "ActiveRICHes",       m_richIsActive                );
  declareProperty( "PurgeHPDsFailIntegrityTest", m_purgeHPDsFailIntegrity = true );
  declareProperty( "HotPixelsToMask",    m_hotChannels                 );
  declareProperty( "RawEventLocations",  m_rawEventLocations={LHCb::RawEventLocation::Rich, LHCb::RawEventLocation::Default},
                   "List of possible locations of the RawEvent object in the"
                   " transient store. By default it is LHCb::RawEventLocation::Rich,"
                   " LHCb::RawEventLocation::Default.");
}

// Destructor
RawDataFormatTool::~RawDataFormatTool() { }

StatusCode RawDataFormatTool::initialize()
{
  // Initialise base class
  const StatusCode sc = Rich::ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  // report inactive RICHes
  if ( m_richIsActive.size() != Rich::NRiches )
  { return Error( "Incorrectly configured active RICH options" ); }
  if ( !m_richIsActive[Rich::Rich1] )
  { Warning("Decoding for RICH1 disabled",StatusCode::SUCCESS).ignore(); }
  if ( !m_richIsActive[Rich::Rich2] )
  { Warning("Decoding for RICH2 disabled",StatusCode::SUCCESS).ignore(); }

  // if suppression is less than max possible number of (ALICE) hits, print a message.
  if ( m_maxHPDOc < BitsPerDataWord * MaxDataSizeALICE )
    info() << "Will suppress PDs with more than " << m_maxHPDOc << " digits" << endmsg;

  // messages if optional features are enabled
  if ( m_extendedFormat )
    info() << "Will encode RawEvent using extended HPD format" << endmsg;
  if ( m_decodeUseOdin )
    info() << "ODIN integrity checks are enabled" << endmsg;
  if ( m_hpdL1check )
    info() << "HPD L1 ID checks are enabled" << endmsg;

  // warnings if checks are disabled
  if ( !m_checkDataIntegrity )
    Warning( "HPD Data integrity checks are disabled",     StatusCode::SUCCESS ).ignore();
  if ( m_checkODINEventsIDs )
    Warning( "Ingress/ODIN Event ID checks are enabled",   StatusCode::SUCCESS ).ignore();
  if ( !m_checkRICHEventsIDs )
    Warning( "Internal RICH Event ID checks are disabled", StatusCode::SUCCESS ).ignore();
  if ( !m_checkBxIDs )
    Warning( "Header BX ID checks are disabled",           StatusCode::SUCCESS ).ignore();

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  incSvc()->addListener( this, IncidentType::EndEvent   );

  // Do we have pixels to suppress ?
  if ( !m_hotChannels.empty() )
  {
    info() << "Hot HPD pixel list to suppress :-" << endmsg;
    m_pixelsToSuppress = true;
    for ( HotPixelListType::const_iterator iC = m_hotChannels.begin();
          iC != m_hotChannels.end(); ++iC )
    {
      const LHCb::RichSmartID channel(*iC);
      m_hotPixels[channel.pdID()].insert(channel);
      info() << " " << channel << " (" << channel.key() << ")" << endmsg;
    }
  }

  // Initialise the RawEvent locations
  const bool usingDefaultLocation = (m_rawEventLocations.size()==2 
                                     && m_rawEventLocations[0]==LHCb::RawEventLocation::Rich 
                                     && m_rawEventLocations[1]==LHCb::RawEventLocation::Default);
  
  if ( !usingDefaultLocation )
  {
    info() << "Using " << m_rawEventLocations << " as search path for the RawEvent object"
           << endmsg;
  }

  return sc;
}

// Method that handles various Gaudi "software events"
void RawDataFormatTool::handle ( const Incident& incident )
{
  // Update prior to start of new event.
  if      ( IncidentType::BeginEvent == incident.type() )
  {
    InitEvent();
  }
  // End of event
  else if ( IncidentType::EndEvent   == incident.type() )
  {
    FinishEvent();
  }
}

StatusCode RawDataFormatTool::finalize()
{

  // Print out L1 information
  if ( m_summary )
  {
    for ( L1TypeCountTAE::const_iterator iSum = m_l1decodeSummary.begin();
          iSum != m_l1decodeSummary.end(); ++iSum )
    {
      printL1Stats( iSum->second, "RICH Level 1 : Decoding Summary "+iSum->first );
    }
    printL1Stats( m_l1encodeSummary, "RICH Level 1 : Encoding Summary" );
  }

  // base class finalize
  return Rich::ToolBase::finalize();
}

void
RawDataFormatTool::printL1Stats( const L1TypeCount & count,
                                 const std::string & title ) const
{

  // See if any summary info has been collected
  if ( !count.empty() )
  {

    const std::string & LINES = "========================================================================================================================";
    const std::string & lines = "------------------------------------------------------------------------------------------------------------------------";

    // Statistical calculators
    const StatDivFunctor occ1("%7.2f +-%5.2f"), occ2("%8.2f +-%5.2f");

    // Printout
    info() << LINES << endmsg
           << "                             " << title << " : " << m_evtCount << " events" << endmsg;

    Rich::Map<Rich::DetectorType,unsigned long long> totWordSize, totBanks, totHits, nEvFills, s;
    Rich::DetectorType lastrich = Rich::InvalidDetector;
    for ( L1TypeCount::const_iterator iL1C = count.begin(); iL1C != count.end(); ++iL1C )
    {
      const Level1HardwareID L1HardID = (*iL1C).first.l1HardwareID;
      Level1LogicalID L1LogID(0);
      try { L1LogID = m_richSys->level1LogicalID(L1HardID); }
      catch ( ... )
      {
        Warning( "Unknown L1 Hardware ID " + (std::string)L1HardID ).ignore();
      }
      const BankVersion version    = (*iL1C).first.bankVersion;
      Rich::DetectorType rich;
      try { rich = m_richSys->richDetector( L1HardID ); }
      catch ( ... )
      {
        Warning( "Unknown L1 Hardware ID " + (std::string)L1HardID ).ignore();
        rich = Rich::InvalidDetector;
      }
      const unsigned long long nBanks      = (*iL1C).second.nHPDs;
      totBanks[rich]                      += nBanks;
      const unsigned long long words       = (*iL1C).second.nWords;
      totWordSize[rich]                   += words;
      const unsigned long long hits        = (*iL1C).second.nHits;
      totHits[rich]                       += hits;
      const unsigned long long nFills      = (*iL1C).second.nFills;
      nEvFills[rich]                      += nFills;
      ++s[rich];

      if ( rich != lastrich )
      {
        lastrich = rich;
        info() << lines << endmsg;
      }

      info() << " " << rich << " L1ID(hard/log) "
             << boost::format("%2i/%2i") % L1HardID.data() % L1LogID.data();
      info() << " V" << boost::format("%3i") % version;
      info() << " | L1 size ="
             << occ1(nBanks,nFills) << " pds :"
             << occ2(words,nFills) << " words :"
             << occ2(hits,nFills) << " hits / event" << endmsg;

    } // loop over L1s

    info() << lines << endmsg;
    if ( totBanks[Rich::Rich1]>0 )
    {
      const double evtCount1 =
        ( s[Rich::Rich1]>0 ? (double)(nEvFills[Rich::Rich1])/(double)(s[Rich::Rich1]) : 0 );
      info() << " Rich1 Average                   | L1 size ="
             << occ1(totBanks[Rich::Rich1],evtCount1) << " pds :"
             << occ2(totWordSize[Rich::Rich1],evtCount1) << " words :"
             << occ2(totHits[Rich::Rich1],evtCount1) << " hits / event" << endmsg;
    }
    if ( totBanks[Rich::Rich2]>0 )
    {
      const double evtCount2 =
        ( s[Rich::Rich2]>0 ? (double)(nEvFills[Rich::Rich2])/(double)(s[Rich::Rich2]) : 0 );
      info() << " Rich2 Average                   | L1 size ="
             << occ1(totBanks[Rich::Rich2],evtCount2) << " pds :"
             << occ2(totWordSize[Rich::Rich2],evtCount2) << " words :"
             << occ2(totHits[Rich::Rich2],evtCount2) << " hits / event" << endmsg;
    }

    info() << LINES << endmsg;

  } // end stats available

}

// Create data bank of correct version
// This function knows which bank objects to create for each version number
const HPDDataBank *
RawDataFormatTool::createDataBank( const LHCb::RichSmartID::Vector & smartIDs,
                                   const BankVersion version,
                                   const LHCb::ODIN * odin ) const
{

  // pointer to HPD data block
  HPDDataBank * dataBank = NULL;

  // Check bank is not empty
  if ( smartIDs.empty() )
  {
    Warning( "RichSmartID vector is empty -> No data bank" ).ignore();
    return dataBank;
  }

  // Level 0 ID
  const Level0ID l0ID = m_richSys->level0ID( smartIDs.front().pdID() );

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
  else if ( FlatList == version )
  {

    dataBank = (HPDDataBank*) new RichDAQ_FlatList::Data( l0ID, smartIDs );

  }
  else
  {
    std::ostringstream message;
    message << "Unknown RICH Raw Buffer version " << version << " -> No data bank";
    Warning ( message.str() );
    return NULL;
  }

  if ( msgLevel(MSG::VERBOSE) )
  {
    // Print out SmartIDs to encode
    verbose() << endmsg << "   Creating encoding HPD " << smartIDs.front().pdID()
              << " bank from " << smartIDs.size() << " RichSmartIDs :-" << endmsg;
    for ( const auto& ID : smartIDs ) { verbose() << "    " << ID << endmsg; }
    // Printout this bank
    verbose() << *dataBank << endmsg;
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
      if ( header.aliceMode() )
      {
        dataBank = (HPDDataBank*) new RichDAQ_LHCb5::ZeroSuppAlice( dataStart );
      }
      else
      {
        dataBank = (HPDDataBank*) new RichDAQ_LHCb5::ZeroSuppLHCb( dataStart );
      }
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
      if ( header.aliceMode() )
      {
        Warning ( "LHCb4 data format does not support ZS Alice mode data" ).ignore();
      }
      else
      {
        dataBank = (HPDDataBank*) new RichDAQ_LHCb4::ZeroSuppLHCb( dataStart );
      }
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
      if ( header.aliceMode() )
      {
        Warning ( "LHCb3 data format does not support ZS Alice mode data" ).ignore();
      }
      else
      {
        dataBank = (HPDDataBank*) new RichDAQ_LHCb3::ZeroSuppLHCb( dataStart );
      }
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
  else if ( FlatList == version )
  {

    dataBank = (HPDDataBank*) new RichDAQ_FlatList::Data( dataStart );

  }
  else
  {
    std::ostringstream message;
    message << "Unknown RICH Raw Buffer version " << version << " -> No data bank";
    Exception ( message.str() );
  }

  // Printout this bank
  if ( dataBank && msgLevel(MSG::VERBOSE) )
  {
    verbose() << endmsg
              << "Created HPD Data Bank for Decoding :-" << endmsg;
    verbose() << *dataBank << endmsg;
  }

  return dataBank;
}

void RawDataFormatTool::fillRawEvent( const LHCb::RichSmartID::Vector & smartIDs,
                                      const BankVersion version ) const
{
  m_hasBeenCalled = true;

  // new rich data map
  L1Map L1Data = dummyMap();

  // Loop over digits and sort according to L1 and HPD
  for ( LHCb::RichSmartID::Vector::const_iterator iDigit = smartIDs.begin();
        iDigit != smartIDs.end(); ++iDigit )
  {

    // Get Level 1 board number and input
    const Level1HardwareID L1ID = m_richSys->level1HardwareID ( *iDigit );
    const Level1Input L1Input   = m_richSys->level1InputNum   ( *iDigit );

    // Get reference to correct data group
    IngressMap & ingress   = L1Data  [ L1ID                ];
    IngressInfo & hpds     = ingress [ L1Input.ingressID() ];
    HPDInfo & hpdInfo      = (hpds.hpdData())[L1Input];

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << *iDigit << " mapped to L1ID " << L1ID << " L1Input " << L1Input << endmsg;
    }

    // Finally, insert this pixel into correct place
    hpdInfo.smartIDs().push_back( *iDigit );

    // Set the HPD information
    hpdInfo.setHpdID( (*iDigit).pdID() );

  }

  // Get some ODIN info
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "ODIN : " << *odin() << endmsg;
  }

  // Loop over Level1 board and make a RAWBank for each
  for ( L1Map::const_iterator iL1 = L1Data.begin();
        iL1 != L1Data.end(); ++iL1 )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Creating Bank for L1 board " << (*iL1).first << endmsg;

    // Make a new data bank
    RAWBank dataBank;

    // local hit and HPD count
    ShortType nHits(0), nHPDs(0);

    // Loop over ingresses for this L1 board
    L1IngressID ingress(0);
    for ( IngressMap::const_iterator iIngress = (*iL1).second.begin();
          iIngress != (*iL1).second.end(); ++iIngress, ++ingress )
    {

      // If 2007 data format, add Ingress header word to the raw bank
      if ( version != LHCb0 &&
           version != LHCb1 &&
           version != LHCb2 &&
           version != LHCb3 &&
           version != LHCb4 &&
           version != MaPMT0 )
      {
        // create ingress header word
        L1IngressHeader ingressWord;
        // set ingress number
        ingressWord.setIngressID( ingress );
        // set data from ODIN
        ingressWord.readFromODIN( odin() );
        // fill with HPDs with data
        for ( Rich::DAQ::HPDMap::const_iterator iHPD = (*iIngress).second.hpdData().begin();
              iHPD != (*iIngress).second.hpdData().end(); ++iHPD )
        {
          ingressWord.setHPDActive( (*iHPD).first.l1InputWithinIngress() );
        }
        // fill header into raw data
        ingressWord.fillRAWBank(dataBank);
        if ( msgLevel(MSG::DEBUG) )
          debug() << " Ingress " << ingress << " : " << ingressWord << endmsg;
      }

      if ( msgLevel(MSG::DEBUG) )
        debug() << "  Found " << (*iIngress).second.hpdData().size() << " HPD(s) with data" << endmsg;

      // Loop over each active HPD for this Level1 board ingress and fill RAWBank
      for ( HPDMap::const_iterator iHPD = (*iIngress).second.hpdData().begin();
            iHPD != (*iIngress).second.hpdData().end(); ++iHPD )
      {
        if ( msgLevel(MSG::DEBUG) )
        {
          debug() << "   HPD " << (*iHPD).second.hpdID() << " L1input=" << (*iHPD).first
                  << " Found " << (*iHPD).second.smartIDs().size() << " hits" << endmsg;
        }
        if ( version != MaPMT0 )
        {
          // Get raw data bank for this HPD, and fill into RAWBank
          const HPDDataBank * hpdData = createDataBank( (*iHPD).second.smartIDs(), version, odin() );
          if ( hpdData )
          {
            hpdData->fillRAWBank( dataBank );
            delete hpdData;
            nHits += (*iHPD).second.smartIDs().size();
            ++nHPDs;
          }
          else
          {
            Warning( "No RICH Data Bank Created" ).ignore();
          }
        }
        else // Special MaPMT0 version
        {
          for ( LHCb::RichSmartID::Vector::const_iterator iDigit = (*iHPD).second.smartIDs().begin();
                iDigit != (*iHPD).second.smartIDs().end(); ++iDigit )
          {
            dataBank.push_back( (*iDigit).key() );
          }
        }

      } // end photon detector loop

    } // ingress loop

    // Add this bank to the Raw buffer
    m_currentTAE = "";
    rawEvent()->addBank( (*iL1).first.data(),
                         LHCb::RawBank::Rich,
                         version,
                         dataBank );

    if ( m_summary )
    {
      // Count the number of banks and size
      L1CountAndSize & cands = m_l1encodeSummary[ L1IDandV(version,(*iL1).first) ];
      // Increment bank size
      cands.nWords += dataBank.size(); // L1 headers + data words
      // Increment hit occupancy
      cands.nHits += nHits;
      // Count number of HPD banks
      cands.nHPDs += nHPDs;
      // Count fills
      ++cands.nFills;
    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Encoded " << boost::format("%2i") % (*iL1).second.size();
      debug() << " ingresses for Level1 Bank "
              << boost::format("%2i") % (*iL1).first.data();
      debug() << " : Size "
              << boost::format("%4i") % dataBank.size()
              << " words : Version " << version << endmsg;
    }

  } // Level1 loop

}

void RawDataFormatTool::decodeToSmartIDs( const LHCb::RawBank & bank,
                                          L1Map & decodedData ) const
{

  // Check magic code for general data couurption
  if ( LHCb::RawBank::MagicPattern != bank.magic() )
  {
    std::ostringstream mess;
    mess << "Magic Pattern mis-match : Expected " << LHCb::RawBank::MagicPattern
         << " Obtained " << bank.magic();
    Error( mess.str() ).ignore();
  }
  else
  { // All OK so decode

    // Check this is a RICH bank
    if ( bank.type() != LHCb::RawBank::Rich )
    {
      std::ostringstream message;
      message << "BankType is not RICH : type = " << bank.type();
      Exception( message.str() );
    }

    // Get L1 ID
    const Level1HardwareID L1ID ( bank.sourceID() );

    // Get bank version
    const BankVersion version = bankVersion( bank );

    // Is the RICH this L1 ID is for active ?
    const Rich::DetectorType rich = m_richSys->richDetector(L1ID);
    if ( rich == Rich::InvalidDetector )
    {
      std::ostringstream mess;
      mess << "L1 bank " << L1ID << " has an invalid RICH detector mapping -> Bank skipped";
      Warning( mess.str() ).ignore();
    }
    else if ( m_richIsActive[rich] )
    {

      // if configured, dump raw event before decoding
      if      ( msgLevel(MSG::VERBOSE) ) { dumpRawBank( bank, verbose() ); }
      else if ( m_dumpBanks            ) { dumpRawBank( bank, info()    ); }

      // Flag tool as having been used this event
      m_hasBeenCalled = true;

      // Count decoded L1IDs
      ++m_l1IdsDecoded[ L1ID ];

      // Now, delegate the work to a version of the decoding appropriate to the version
      if      ( version == LHCb5    ||
                version == FlatList  )
      {
        decodeToSmartIDs_2007(bank,decodedData);
      }
      else if ( version == LHCb3 ||
                version == LHCb4  ) // RICH 2006 Testbeam
      {
        decodeToSmartIDs_2006TB(bank,decodedData);
      }
      else if ( version == LHCb0 ||
                version == LHCb1 ||
                version == LHCb2  )  // DC04 or DC06
      {
        decodeToSmartIDs_DC0406(bank,decodedData);
      }
      else if ( MaPMT0 == version )
      {
        decodeToSmartIDs_MaPMT0(bank,decodedData);
      }
      else // Some problem ...
      {
        std::ostringstream mess;
        mess << "Unknown RICH L1 version number " << version;
        Exception( mess.str() );
      }

    } // active RICH

  } // magic OK

}

void
RawDataFormatTool::decodeToSmartIDs_MaPMT0( const LHCb::RawBank & bank,
                                            Rich::DAQ::L1Map & decodedData ) const
{
  // Get L1 ID
  const Level1HardwareID L1ID ( bank.sourceID() );
  if ( msgLevel(MSG::DEBUG) )
    debug() << "FlatList Decoding L1 bank " << L1ID << endmsg;

  // Data bank size in 32 bit words
  const int bankSize = bank.size() / 4;

  // Get bank version
  const BankVersion version = bankVersion( bank );

  // various counts
  unsigned int decodedHits(0);
  std::set<LHCb::RichSmartID> pdSet;

  // If we have some words to process, start the decoding
  if ( bankSize > 0 )
  {

    // Get Ingress map to decode into for this L1 board
    IngressMap & ingressMap = decodedData[L1ID];

    // Loop over bank, Fill data into RichSmartIDs
    int lineC(0);
    while ( lineC < bankSize )
    {
      // Read the smartID direct from the banks
      const LHCb::RichSmartID id( bank.data()[lineC++] );
      if ( !id.isValid() )
      {
        Warning( "Invalid RichSmartID read from FlatList data format" ).ignore();
      }
      else
      {
        if ( msgLevel(MSG::DEBUG) ) debug() << " -> " << id << endmsg;

        // Get the L1 input from the DB
        const Level1Input l1Input = m_richSys->level1InputNum(id);

        // Do we have an entry for this Ingress ID ?
        if ( ingressMap.find(l1Input.ingressID()) == ingressMap.end() )
        {
          IngressInfo & info = ingressMap[l1Input.ingressID()];
          L1IngressHeader iHeader;
          iHeader.setIngressID( l1Input.ingressID() );
          info.setIngressHeader(iHeader);
        }
        IngressInfo & ingressInfo = ingressMap[l1Input.ingressID()];
        HPDMap      & hpdMap      = ingressInfo.hpdData();

        // Does this PD have an entry
        if ( hpdMap.find(l1Input) == hpdMap.end() )
        {
          hpdMap[l1Input] = HPDInfo();
          // Set the PD ID
          hpdMap[l1Input].setHpdID( id.pdID() );
          // set the header
          HPDInfo::Header header;
          // CRJ - Comment out until decide what to do about maPMT Level0 IDs ...
          //           const Level0ID l0id = m_richSys->level0ID(id);
          //           if ( ! header.setL0ID( l0id ) )
          //           {
          //             std::ostringstream mess;
          //             mess << "Failed to set L0ID " << l0id;
          //             Warning( mess.str() ).ignore();
          //           }
          hpdMap[l1Input].setHeader( header );
        }
        HPDInfo & hpdInfo = hpdMap[l1Input];

        // add the hit to the list
        hpdInfo.smartIDs().push_back( id );

        // count the hits and hpds
        ++decodedHits;
        pdSet.insert( id.pdID() );

      }
    }

  }

  if ( m_summary )
  {
    // Count the number of banks and size
    L1CountAndSize & cands = (m_l1decodeSummary[m_currentTAE])[ L1IDandV(version,L1ID) ];
    // Increment bank size
    cands.nWords += bank.size()/4; // 2 L1 headers + data words
    // Increment hit occupancy
    cands.nHits += decodedHits;
    // Count number of HPD banks
    cands.nHPDs += pdSet.size();
    // Count fills
    ++cands.nFills;
  }

}

void RawDataFormatTool::decodeToSmartIDs_2007( const LHCb::RawBank & bank,
                                               L1Map & decodedData ) const
{
  // Get L1 ID
  const Level1HardwareID L1ID ( bank.sourceID() );
  if ( msgLevel(MSG::DEBUG) ) debug() << "Decoding L1 bank " << L1ID << endmsg;

  // various counts
  unsigned int nHPDbanks(0), decodedHits(0);

  // Data bank size in 32 bit words
  const int bankSize = bank.size() / 4;

  // Get bank version
  const BankVersion version = bankVersion( bank );

  // Flag to indicate if a given L1 bank has been printed out in case of an error
  bool l1BankErrorDump = true;

  // If we have some words to process, start the decoding
  if ( bankSize > 0 )
  {

    // Get Ingress map to decode into for this L1 board
    IngressMap & ingressMap = decodedData[L1ID];

    // Loop over bank, find headers and produce a data bank for each
    // Fill data into RichSmartIDs
    int lineC(0);
    while ( lineC < bankSize )
    {

      // First, the ingress header word (and increment to the next line)
      const L1IngressHeader ingressWord( bank.data()[lineC++] );
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << " Ingress " << ingressWord << endmsg;
      }

      // Get data for this ingress
      IngressInfo & ingressInfo = ingressMap[ingressWord.ingressID()];

      // Set ingress header in decoded data map
      ingressInfo.setIngressHeader( ingressWord );

      // Compare Ingress header to the ODIN
      if (  msgLevel(MSG::VERBOSE) )
      {
        verbose() << "ODIN : EventNumber=" << EventID(odin()->eventNumber())
                  << " BunchID=" << BXID(odin()->bunchId()) << endmsg;
      }
      const bool odinOK
        = ( !m_decodeUseOdin ? true :
            ( !m_checkODINEventsIDs || ingressWord.eventID() == EventID(odin()->eventNumber()) ) &&
            ( !m_checkBxIDs         || ingressWord.bxID()    == BXID   (odin()->bunchId())     ) );
      if ( !odinOK )
      {
        std::ostringstream mess;
        mess << "ODIN Mismatch : L1ID " << L1ID
             << " : ODIN EvID="         << EventID(odin()->eventNumber())
             << " BxID="                << BXID(odin()->bunchId())
             << " : L1IngressHeader "   << ingressWord
             << " -> Data Suppressed";
        Error( mess.str() ).ignore();
      }

      // get list of active ingress inputs
      L1IngressInputs inputs;
      ingressWord.activeHPDInputs(inputs);
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "  Found " << inputs.size() << " PDs with data blocks : " << inputs << endmsg;
      }

      // Check the Ingress supression flag
      if ( ingressWord.hpdsSuppressed() )
      {
        std::ostringstream mess;
        mess << "L1 board " << L1ID << " : Ingress " << ingressWord
             << " is HARDWARE suppressed";
        Warning( mess.str(), StatusCode::SUCCESS, 1 ).ignore();
        //procStatus()->addAlgorithmStatus( name(), "RICH", mess.str(), -1, false );
      }
      else
      {
        // Ingress is OK, so read HPD data

        // Loop over active HPDs
        for ( L1IngressInputs::const_iterator iHPD = inputs.begin(); iHPD != inputs.end(); ++iHPD )
        {

          // Create data bank and decode into RichSmartIDs
          std::auto_ptr<const HPDDataBank>
            hpdBank ( createDataBank( &bank.data()[lineC], // pointer to start of data
                                      0, // Not needed here (to be removed). Must be 0 though
                                      version ) );

          // is this HPD suppressed ?
          const bool hpdIsSuppressed = hpdBank->suppressed();
          if ( hpdIsSuppressed )
          {
            std::ostringstream mess;
            mess << "L1 board " << L1ID << " : Ingress "
                 << ingressWord.ingressID() << " Input " << *iHPD << " is HARDWARE suppressed";
            Warning( mess.str(), StatusCode::SUCCESS, 0 ).ignore();
          }

          // Try to add a new HPDInfo to map
          const Level1Input l1Input(ingressWord.ingressID(),*iHPD);
          std::pair<HPDMap::iterator,bool> p
            = ingressInfo.hpdData().insert( HPDMap::value_type(l1Input,
                                                               HPDInfo(LHCb::RichSmartID(),
                                                                       HPDInfo::Header(hpdBank->headerWords()),
                                                                       HPDInfo::Footer(hpdBank->footerWords()) ) ) );
          // disable test (gives wrong warnings in TAE events)
          //if ( !p.second )
          //{
          //  std::ostringstream mess;
          //  mess << "Found multiple data blocks L1=" << L1ID << " input=" << l1Input;
          //  Warning( mess.str() );
          //}
          HPDInfo & hpdInfo = p.first->second;

          // Only try and decode this HPD if ODIN test was OK
          if ( odinOK && !hpdIsSuppressed )
          {

            // get HPD RichSmartID
            // do in a try block incase HPD ID is unknown
            LHCb::RichSmartID hpdID;
            try
            {
              hpdID = ( m_useFakeHPDID ? s_fakeHPDID :
                        m_richSys->richSmartID( hpdBank->level0ID() ) );
            }
            catch ( const GaudiException & expt )
            {
              std::ostringstream errMsg;
              errMsg << "'" << expt.message() << "' | L1HardID=" << L1ID
                     << " Ingress=" << ingressWord.ingressID() << " Input=" << *iHPD;
              Error( errMsg.str() ).ignore();
            }
            // If the HPD smartID was successfully found, continue with decoding
            if ( hpdID.isValid() )
            {

              if ( msgLevel(MSG::DEBUG) )
                debug() << "   Decoding HPD " << hpdID << endmsg;

              // save HPD ID
              hpdInfo.setHpdID(hpdID);

              // local hit count
              unsigned int hpdHitCount(0);

              // smartIDs
              LHCb::RichSmartID::Vector & newids = hpdInfo.smartIDs();

              // Compare Event IDs for errors
              bool OK = ( hpdIsSuppressed ? true :
                          !m_checkRICHEventsIDs || ingressWord.eventID() == hpdBank->eventID() );
              if ( !OK )
              {
                std::ostringstream mess;
                mess << "EventID Mismatch : HPD L0ID="
                     <<  hpdBank->level0ID() << " " << hpdID;
                Error( mess.str() ).ignore();
              }
              else
              {

                // Check this HPD is connected to the expected L1 board
                OK = true;                    // default to OK
                if ( UNLIKELY(m_hpdL1check) ) // Do check ?
                {
                  const Level1HardwareID db_L1ID = m_richSys->level1HardwareID(hpdBank->level0ID());
                  OK = ( L1ID == db_L1ID );
                  if ( !OK )
                  {
                    std::ostringstream mess;
                    mess << "HPD L0ID=" << hpdBank->level0ID() << " found in L1HardID=" << L1ID
                         << " but database says it should be in L1HardID=" << db_L1ID
                         << " -> rejected";
                    Error( mess.str() ).ignore();
                  }
                }
                if ( OK ) // only carry on if OK
                {

                  // decode to smartIDs
                  hpdHitCount = hpdBank->fillRichSmartIDs( newids, hpdID );

                  // Do data integrity checks
                  OK = ( !m_checkDataIntegrity || hpdBank->checkDataIntegrity(newids,warning()) );
                  if ( !OK )
                  {
                    std::ostringstream mess;
                    mess << "HPD L0ID=" << hpdBank->level0ID() << " " << hpdID
                         << " data block failed integrity check";
                    Error( mess.str() ).ignore();
                    if ( m_purgeHPDsFailIntegrity ) { newids.clear(); }
                  }

                  // Is all 'OK' but header is in extended mode ?
                  if ( OK && hpdBank->headerWords().size()>1 )
                  {
                    std::ostringstream mess;
                    mess << "HPD L0ID=" << hpdBank->level0ID() << " " << hpdID
                         << " in extended mode for UNKNOWN reasons";
                    Error( mess.str() ).ignore();
                  }

                  if ( msgLevel(MSG::VERBOSE) && hpdHitCount>0 )
                  {
                    // printout decoded RichSmartIDs
                    verbose() << " Decoded RichSmartIDs :-" << endmsg;
                    for ( LHCb::RichSmartID::Vector::const_iterator iID = newids.begin();
                          iID != newids.end(); ++iID )
                    {
                      verbose() << "   " << *iID << endmsg;
                    }
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

                  // suppress hot pixels
                  suppressHotPixels(hpdID,newids);

                }
                else
                {
                  std::ostringstream hpd;
                  hpd << hpdID.panelID();
                  Warning( "Forced suppression of HPD "+hpd.str(), StatusCode::SUCCESS, 0 ).ignore();
                  newids.clear();
                }

              }
              else if ( m_verboseErrors )
              {
                // decoding error ....
                error() << "Error in decoding -> Data is rejected for HPD " << hpdID << endmsg;

                if ( l1BankErrorDump )
                {
                  error() << " -> Dump of raw L1 data :-" << endmsg;
                  dumpRawBank( bank, error() );
                  l1BankErrorDump = false;
                }

                error() << " -> Ingress header : " << ingressWord << endmsg;
                error() << "  -> ";
                rawDump(error(),ingressWord.data());
                error() << endmsg;
                error() << " -> HPD data block : " << endmsg;
                error() << "  -> HPD Event ID = " << hpdBank->eventID() << " ";
                rawDump(error(),hpdBank->eventID().data(),hpdBank->eventID().activeBits());
                error() << endmsg;
                error() << *hpdBank << endmsg;

              }

            } // HPD ID OK

          } // ODIN OK and not suppressed

          // Increment line number to next data block
          lineC += hpdBank->nTotalWords();

        } // active HPDs

      } // ingress not suppressed

    } // bank while loop

  } // data bank not empty

  if ( m_summary )
  {
    // Count the number of banks and size
    L1CountAndSize & cands = (m_l1decodeSummary[m_currentTAE])[ L1IDandV(version,L1ID) ];
    // Increment bank size
    cands.nWords += bank.size()/4; // 2 L1 headers + data words
    // Increment hit occupancy
    cands.nHits += decodedHits;
    // Count number of HPD banks
    cands.nHPDs += nHPDbanks;
    // Count fills
    ++cands.nFills;
  }

  // debug printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Decoded " << boost::format("%2i") % nHPDbanks;
    debug() << " PDs from Level1 Bank ID = "
            << boost::format("%2i") % L1ID.data();
    debug() << " : Size " << boost::format("%4i") % (bank.size()/4) << " words : Version "
            << version << endmsg;
  }

}

void RawDataFormatTool::suppressHotPixels( const LHCb::RichSmartID& hpdID,
                                           LHCb::RichSmartID::Vector & newids ) const
{
  // clean out hot pixels enabled at all ?
  if ( m_pixelsToSuppress )
  {
    // Does this HPD have some pixels to suppress
    HPDHotPixels::const_iterator iHPDSup = m_hotPixels.find(hpdID);
    if ( iHPDSup != m_hotPixels.end() )
    {
      // temporary copy of original hit channels
      const LHCb::RichSmartID::Vector oldids = newids;

      // clear hits before suppression
      newids.clear();

      // loop over original hits and save only those not supressed
      for ( LHCb::RichSmartID::Vector::const_iterator iID = oldids.begin();
            iID != oldids.end(); ++iID )
      {
        if ( iHPDSup->second.find(*iID) == iHPDSup->second.end() )
        {
          // not suppressed, so keep
          newids.push_back(*iID);
        }
        else
        {
          // Get detector info
          const Rich::DAQ::Level0ID l0ID           = m_richSys->level0ID(hpdID);
          const Rich::DAQ::Level1HardwareID l1ID   = m_richSys->level1HardwareID(hpdID);
          const Rich::DAQ::Level1Input l1Input     = m_richSys->level1InputNum(hpdID);
          // Print warning
          std::ostringstream mess;
          mess << "L1HardID=" << l1ID << " L1Input=" << l1Input
               << " L0ID=" << l0ID << " " << *iID << " is software SUPPRESSED";
          Warning( mess.str(), StatusCode::SUCCESS );
        }

      } // loop over pixels

    } // this HPD has pixels to suppress

  } // hot pixel suppression enabled

}

void RawDataFormatTool::decodeToSmartIDs_2006TB( const LHCb::RawBank & bank,
                                                 L1Map & decodedData ) const
{

  // Get L1 ID
  const Level1HardwareID L1ID ( bank.sourceID() );

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

    // Get Ingress map to decode into for this L1 board
    IngressMap & ingressMap = decodedData[L1ID];

    // This data version does not have ingress info, so just put all data into ingress 0
    const L1IngressID ingressNum(0);

    // Get data for this ingress
    IngressInfo & ingressInfo = ingressMap[ingressNum];

    // Make up L1 input numbers
    Level1Input l1Input(0);

    // Loop over bank, find headers and produce a data bank for each
    // Fill data into RichSmartIDs
    int lineC(0);
    while ( lineC < bankSize )
    {

      // Create data bank and decode into RichSmartIDs
      std::auto_ptr<const HPDDataBank> hpdBank ( createDataBank( &bank.data()[lineC], // pointer to start of data
                                                                 0, // Not needed here (to be removed). Must be 0 though
                                                                 version ) );

      // get HPD RichSmartID
      const LHCb::RichSmartID hpdID = ( m_useFakeHPDID ? s_fakeHPDID :
                                        m_richSys->richSmartID( hpdBank->level0ID() ) );

      // decode to smartIDs
      Rich::DAQ::HPDInfo & hpdInfo = (ingressInfo.hpdData())[ l1Input ];
      hpdInfo.setHpdID(hpdID);
      ++l1Input;
      LHCb::RichSmartID::Vector & newids = hpdInfo.smartIDs();
      const unsigned int hpdHitCount = hpdBank->fillRichSmartIDs( newids, hpdID );

      // Do data integrity checks
      const bool OK = ( !m_checkDataIntegrity || hpdBank->checkDataIntegrity(newids,warning()) );
      if ( !OK || msgLevel(MSG::VERBOSE) )
      {
        // printout decoded RichSmartIDs
        verbose() << " Decoded RichSmartIDs :-" << endmsg;
        for ( LHCb::RichSmartID::Vector::const_iterator iID = newids.begin();
              iID != newids.end(); ++iID )
        {
          verbose() << "   " << *iID << endmsg;
        }
      }
      if ( !OK && m_purgeHPDsFailIntegrity ) { newids.clear(); }

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
          Warning( "Forced suppression of HPD "+hpd.str(), StatusCode::SUCCESS, 0 ).ignore();
          newids.clear();
        }

      }
      else if ( m_verboseErrors )
      {
        // decoding error ....
        error() << "Corruption in decoding -> Data is rejected for HPD " << hpdID << endmsg;

        error() << " -> Dump of offending raw L1 data :-" << endmsg;
        dumpRawBank( bank, error() );

        error() << " -> Badly decoded HPD :-" << endmsg;
        error() << *hpdBank << endmsg;
      }

      // Increment line number to next data block
      lineC += hpdBank->nTotalWords();

    } // bank while loop

  } // data bank not empty

  if ( m_summary )
  {
    // Count the number of banks and size
    L1CountAndSize & cands = (m_l1decodeSummary[m_currentTAE])[ L1IDandV(version,L1ID) ];
    // Increment bank size
    cands.nWords += bank.size()/4; // 2 L1 headers + data words
    // Increment hit occupancy
    cands.nHits += decodedHits;
    // Count number of HPD banks
    cands.nHPDs += nHPDbanks;
    // Count fills
    ++cands.nFills;
  }

  // debug printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Decoded " << boost::format("%2i") % nHPDbanks;
    debug() << " PDs from Level1 Bank "
            << boost::format("%2i") % L1ID.data();
    debug() << " : Size " << boost::format("%4i") % (bank.size()/4) << " words : Version "
            << version << endmsg;
  }

}

void RawDataFormatTool::decodeToSmartIDs_DC0406( const LHCb::RawBank & bank,
                                                 L1Map & decodedData ) const
{

  // Get L1 ID
  const Level1HardwareID base_L1ID ( bank.sourceID() );

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

    // Make up L1 input numbers when using fake HPDIDs
    Level1Input fake_l1Input(0);

    // Loop over bank, find headers and produce a data bank for each
    // Fill data into RichSmartIDs
    int lineC(0);
    while ( lineC < bankSize )
    {

      // Find HPD bank start
      const MyHeader header ( &bank.data()[lineC] );

      // Is this a true header
      if ( header.startPD() )
      {

        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " Found HPD header at line " << lineC << " of " << bankSize << endmsg;

        // Store start line for header
        const int lineHeader = lineC;

        // Find last line of block
        int lineLast = lineC;
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
            verbose() << "  -> Bank is zero surpressed : ends at " << lineLast << endmsg;

        }
        else
        {
          // non-ZS blocks have fixed length, so skip straight to the end

          lineC   += 1 + maxDataSize; // data block + header
          lineLast = lineC - 1;

          if ( msgLevel(MSG::VERBOSE) )
            verbose() << "  -> Bank is non zero surpressed : ends at " << lineLast << endmsg;

        }

        // Check data size
        const unsigned int dataSize = lineLast-lineHeader;
        if ( dataSize < 1 )
        {
          std::ostringstream message;
          message << "Invalid HPD data block size : " << dataSize;
          Exception( message.str() );
        }

        // Create data bank and decode into RichSmartIDs
        const HPDDataBank * hpdBank ( createDataBank( &bank.data()[lineHeader],
                                                      dataSize,
                                                      version ) );

        // get HPD RichSmartID
        const LHCb::RichSmartID hpdID = ( m_useFakeHPDID ? s_fakeHPDID :
                                          m_richSys->richSmartID( hpdBank->level0ID() ) );

        // L1 ID
        const Level1HardwareID L1ID = m_richSys->level1HardwareID(hpdID);
        if ( L1ID != base_L1ID )
        {
          error() << "L1ID Mis-match" << endmsg;
          error() << "  -> base : " << base_L1ID << endmsg;
          error() << "  -> HPD  : " << L1ID << endmsg;
        }

        // Get Ingress map to decode into for this L1 board
        IngressMap & ingressMap = decodedData[L1ID];

        // L1 input number
        const Level1Input l1Input = ( m_useFakeHPDID ?
                                      fake_l1Input :
                                      m_richSys->level1InputNum(hpdID) );
        if ( m_useFakeHPDID ) ++fake_l1Input;

        // Ingress info
        IngressInfo & ingressInfo = ingressMap[l1Input.ingressID()];

        // get HPD data
        Rich::DAQ::HPDInfo & hpdInfo = (ingressInfo.hpdData())[ l1Input ];
        hpdInfo.setHpdID(hpdID);
        LHCb::RichSmartID::Vector & newids = hpdInfo.smartIDs();

        // get hit count
        const unsigned int hpdHitCount = hpdBank->fillRichSmartIDs( newids, hpdID );

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
          Warning( "Suppressed HPD in "+hpd.str(), StatusCode::SUCCESS, 0 ).ignore();
          newids.clear();
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
    L1CountAndSize & cands = (m_l1decodeSummary[m_currentTAE])[ L1IDandV(version,base_L1ID) ];
    // Increment bank size
    cands.nWords += bank.size()/4; // 2 L1 headers + data words
    // Increment hit occupancy
    cands.nHits += decodedHits;
    // Count number of HPD banks
    cands.nHPDs += nHPDbanks;
    // Count fills
    ++cands.nFills;
  }

  // debug printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Decoded " << boost::format("%2i") % nHPDbanks;
    debug() << " PDs from Level1 Bank "
            << boost::format("%2i") % base_L1ID.data();
    debug() << " : Size " << boost::format("%4i") % (bank.size()/4) << " words : Version "
            << version << endmsg;
  }

}

void
RawDataFormatTool::decodeToSmartIDs( const RawEventLocations & taeLocations,
                                     L1Map & decodedData ) const
{
  // set the been used flag
  m_hasBeenCalled = true;

  // clear the L1 ID map
  m_l1IdsDecoded.clear();

  // Loop over all RawEvent locations
  for ( RawEventLocations::const_iterator iLoc = taeLocations.begin();
        iLoc != taeLocations.end(); ++iLoc )
  {
    // Set the current TAE type (for use by odin and raw event methods)
    m_currentTAE = *iLoc;
    if ( !m_currentTAE.empty() ) m_currentTAE += "/";

    // Get the raw event
    LHCb::RawEvent * rawEv = rawEvent();
    if ( !rawEv ) continue;

    // Get the banks for the Rich
    const LHCb::RawBank::Vector & richBanks = rawEv->banks( LHCb::RawBank::Rich );

    // Loop over data banks
    for ( LHCb::RawBank::Vector::const_iterator iBank = richBanks.begin();
          iBank != richBanks.end(); ++iBank )
    {
      // get bank
      LHCb::RawBank * bank = *iBank;
      // test bank is OK
      if ( bank )
      {
        // Decode this bank
        try
        {
          decodeToSmartIDs( *bank, decodedData );
        }
        catch ( const GaudiException & expt )
        {
          // Print error message
          std::ostringstream mess;
          mess << "Error decoding bank ID=" << bank->sourceID() << " version=" << bankVersion(*bank)
               << " '" << expt.message() << "' '" << expt.tag() << "'";
          Error( mess.str() ).ignore();
          // dump the full bank
          if ( m_verboseErrors ) dumpRawBank( *bank, error() );
        }
      }
      else
      {
        Error( "Retrieved null pointer to RawBank" ).ignore();
      }
    }

    // do not print if faking HPDID, since smartIDs.size() then has no meaning
    if ( !m_useFakeHPDID && msgLevel(MSG::DEBUG) )
    {
      debug() << "Decoded in total " << richBanks.size() << " RICH Level1 bank(s) for RawEvent '"
              << *iLoc << "'"
              << endmsg;
    }

  } // loop over RawEvent locations in the TES

  // loop over the L1 map to check we only got each L1 ID once
  // (only if not using more than one RawEvent)
  // CRJ : Is this check really needed ?
  if ( taeLocations.size() == 1 )
  {
    for ( Rich::Map<Rich::DAQ::Level1HardwareID,unsigned int>::const_iterator iL1 = m_l1IdsDecoded.begin();
          iL1 != m_l1IdsDecoded.end(); ++iL1 )
    {
      if ( iL1->second > 1 )
      {
        std::ostringstream mess;
        mess << "Found multiple RawBanks (" << iL1->second << ") for L1 ID " << iL1->first;
        Warning( mess.str() ).ignore();
      }
    }
  }

}

LHCb::RawEvent * RawDataFormatTool::rawEvent() const
{
  LHCb::RawEvent *& raw = m_rawEvent[m_currentTAE];
  if (!raw)
  {
    std::string loc = "";
    for ( std::vector<std::string>::const_iterator p = m_rawEventLocations.begin();
          p != m_rawEventLocations.end(); ++p )
    {
      loc = m_currentTAE + (*p);
      if ( exist<LHCb::RawEvent>(loc) )
      {
        raw = get<LHCb::RawEvent>(loc);
        break;
      }
    }
    if ( !raw )
    {
      Warning( "No RawEvent at '"+loc+"'" ).ignore();
    }
  }
  return raw;
}

const Rich::DAQ::L1Map & RawDataFormatTool::dummyMap() const
{
  static Rich::DAQ::L1Map dummyMap;
  if ( dummyMap.empty() )
  {
    // create an ingressmap for each L1 board
    Rich::DAQ::IngressMap ingressMap;
    for ( unsigned int input = 0; input<Rich::DAQ::NumIngressPerL1; ++input )
    {
      L1IngressHeader header;
      header.setIngressID( L1IngressID(input) );
      ingressMap[L1IngressID(input)].setIngressHeader(header);
    }
    // create a dummy L1data object with a default ingress map for each L1 board
    for ( Level1HardwareIDs::const_iterator iID = m_richSys->level1HardwareIDs().begin();
          iID != m_richSys->level1HardwareIDs().end(); ++iID )
    {
      dummyMap[ *iID ] = ingressMap;
    }
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Created " << dummyMap.size()
              << " entries in empty L1 map : L1HardwareIDs = "
              << m_richSys->level1HardwareIDs() << endmsg;
    }
  }
  return dummyMap;
}

void RawDataFormatTool::dumpRawBank( const LHCb::RawBank & bank,
                                     MsgStream & os ) const
{

  // Get bank version and ID
  const Level1HardwareID L1ID ( bank.sourceID() );
  const BankVersion version = bankVersion( bank );

  // Data bank size in words
  const int bankSize = bank.size() / 4;

  std::ostringstream magicAsHex;
  magicAsHex << std::hex << bank.magic();
  os << "RawBank version=" << version << " L1ID=" << L1ID << " datasize=" << bankSize
     << " magic=" << magicAsHex.str() << endmsg;

  // Printout raw data

  // Is this an empty bank ?
  if ( bankSize > 0 )
  {
    const std::string & LINES =
      "----------------------------------------------------------------------------------------------------------------";

    // Bit numbers
    os << LINES << endmsg;
    os << "      bit |";
    for ( int iCol = 31; iCol >= 0; --iCol )
    {
      os << boost::format("%3i") % iCol;
    }
    os << endmsg;
    os << LINES << endmsg;
    typedef unsigned int Dtype;
    for ( const Dtype * dataW = bank.begin<Dtype>(); dataW != bank.end<Dtype>(); ++dataW )
    {
      rawDump( os, *dataW );
      os << endmsg;
    }
    os << LINES << endmsg;

  }
  else
  {
    os << "  -> Bank is empty" << endmsg;
  }

}

