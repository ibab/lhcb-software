
//-----------------------------------------------------------------------------
/** @file RichRawDataFormatTool.cpp
 *
 *  Implementation file for class : RichRawDataFormatTool
 *
 *  CVS Log :-
 *  $Id: RichRawDataFormatTool.cpp,v 1.36 2006-09-21 08:30:59 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 2004-12-18
 */
//-----------------------------------------------------------------------------

// local
#include "RichRawDataFormatTool.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRawDataFormatTool>          s_factory ;
const        IToolFactory& RichRawDataFormatToolFactory = s_factory ;

namespace
{
  static const unsigned int nL1HeaderWords = 2;
}

// Standard constructor
RichRawDataFormatTool::RichRawDataFormatTool( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : RichToolBase    ( type, name , parent ),
    m_richSys       ( NULL                ),
    m_rawEvent      ( NULL                ),
    m_evtCount      ( 0                   ),
    m_hasBeenCalled ( false               ),
    m_maxHPDOc      ( 999999              )
{
  // interface
  declareInterface<IRichRawDataFormatTool>(this);

  // job opts
  declareProperty( "ZeroSuppressHitCut", m_zeroSuppresCut = 96 );
  declareProperty( "RawEventLocation",
                   m_rawEventLoc = RawEventLocation::Default );
  declareProperty( "PrintSummary", m_summary = true );
  declareProperty( "MaxHPDOccupancy", m_maxHPDOc );
  declareProperty( "DumpRawBanks", m_dumpBanks = false );
  declareProperty( "UseZeroSuppression", m_zeroSupp = true );
}

// Destructor
RichRawDataFormatTool::~RichRawDataFormatTool() {}

StatusCode RichRawDataFormatTool::initialize()
{
  // Initialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocation::RichSystem );

  // create a dummy L1data object with an empty vector for each L1 board
  m_dummyMap.clear();
  for ( RichDAQ::Level1IDs::const_iterator iID = m_richSys->level1IDs().begin();
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
  if ( m_maxHPDOc < 32 * RichDAQ::MaxDataSizeALICE )
  {
    info() << "Will suppress HPDs with more than " << m_maxHPDOc << " digits" << endreq;
  }

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  incSvc()->addListener( this, IncidentType::EndEvent   );

  return sc;
}

// Method that handles various Gaudi "software events"
void RichRawDataFormatTool::handle ( const Incident& incident )
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

StatusCode RichRawDataFormatTool::finalize()
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
RichRawDataFormatTool::printL1Stats( const L1TypeCount & count,
                                     const std::string & title ) const
{

  // See if any summary info has been collected
  if ( !count.empty() )
  {

    // Statistical calculators
    const RichStatDivFunctor occ1("%7.2f +-%5.2f"), occ2("%8.2f +-%5.2f");

    // Printout
    info() << "==========================================================================================================" << endreq
           << "                             " << title << " : " << m_evtCount << " events" << endreq;

    std::vector< unsigned long > totWordSize(Rich::NRiches,0), totBanks(Rich::NRiches,0), totHits(Rich::NRiches,0);
    Rich::DetectorType lastrich = Rich::InvalidDetector;
    for ( L1TypeCount::const_iterator iL1C = count.begin(); iL1C != count.end(); ++iL1C )
    {
      const RichDAQ::Level1ID      L1ID   = (*iL1C).first.second;
      const RichDAQ::BankVersion version  = (*iL1C).first.first;
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
const RichHPDDataBank *
RichRawDataFormatTool::createDataBank( const RichSmartID::Vector & smartIDs,
                                       const RichDAQ::BankVersion version ) const
{

  // flag the tool as having been used this event
  m_hasBeenCalled = true;

  // pointer to HPD data block
  RichHPDDataBank * dataBank = 0;

  // Check bank is not empty
  if ( smartIDs.empty() )
  {
    Warning( "RichSmartID vector is empty -> No data bank" );
    return dataBank;
  }

  // Level 0 ID
  const RichDAQ::Level0ID l0ID = m_richSys->level0ID( smartIDs.front().hpdID() );

  if ( RichDAQ::LHCb4 == version )
  {
    // NB : Does not yet support creation of ALICE mode data blocks...  To Be Done.

    // First try the ZS format
    RichDAQ_LHCb4::ZeroSuppLHCb * zsData = new RichDAQ_LHCb4::ZeroSuppLHCb( l0ID, smartIDs );

    // If too big, use non ZS instead
    if ( !m_zeroSupp || zsData->tooBig() )
    {
      delete zsData;
      dataBank = (RichHPDDataBank*) new RichDAQ_LHCb4::NonZeroSuppLHCb( l0ID, smartIDs );
    }
    else
    {
      dataBank = (RichHPDDataBank*)zsData;
    }

  }
  else if ( RichDAQ::LHCb3 == version )
  {
    // NB : Does not yet support creation of ALICE mode data blocks...  To Be Done.

    // First try the ZS format
    RichDAQ_LHCb3::ZeroSuppLHCb * zsData = new RichDAQ_LHCb3::ZeroSuppLHCb( l0ID, smartIDs );

    // If too big, use non ZS instead
    if ( !m_zeroSupp || zsData->tooBig() )
    {
      delete zsData;
      dataBank = (RichHPDDataBank*) new RichDAQ_LHCb3::NonZeroSuppLHCb( l0ID, smartIDs );
    }
    else
    {
      dataBank = (RichHPDDataBank*)zsData;
    }

  }
  else if ( RichDAQ::LHCb2 == version )
  {
    // First try the ZS format
    RichDAQ_LHCb2::ZeroSuppLHCb * zsData = new RichDAQ_LHCb2::ZeroSuppLHCb( l0ID, smartIDs );

    // If too big, use non ZS instead
    if ( !m_zeroSupp || zsData->tooBig() )
    {
      delete zsData;
      dataBank = (RichHPDDataBank*) new RichDAQ_LHCb2::NonZeroSuppLHCb( l0ID, smartIDs );
    }
    else
    {
      dataBank = (RichHPDDataBank*)zsData;
    }

  }
  else if ( RichDAQ::LHCb1 == version )
  {

    // Decide to zero suppress or not depending on number of hits
    if ( m_zeroSupp && smartIDs.size() < m_zeroSuppresCut )
    {
      dataBank = (RichHPDDataBank*) new RichDAQ_LHCb1::ZeroSuppLHCb( l0ID, smartIDs );
    }
    else
    {
      dataBank = (RichHPDDataBank*) new RichDAQ_LHCb1::NonZeroSuppLHCb( l0ID, smartIDs );
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
    verbose() << " Creating encoding data bank from " << smartIDs.size() << " RichSmartIDs :-" << endreq;
    for ( RichSmartID::Vector::const_iterator iID = smartIDs.begin();
          iID != smartIDs.end(); ++iID )
    {
      verbose() << "   " << *iID << endreq;
    }
    // Printout this bank
    verbose() << *dataBank << endreq;
  }

  return dataBank;
}

// Create data bank of correct version
// This function knows which bank objects to create for each version number
const RichHPDDataBank *
RichRawDataFormatTool::createDataBank( const RichDAQ::LongType * dataStart,
                                       const unsigned int dataSize,
                                       const RichDAQ::BankVersion version ) const
{
  RichHPDDataBank * dataBank = NULL;

  if ( RichDAQ::LHCb4 == version )
  {

    // Header
    const RichDAQ_LHCb4::Header header( dataStart );

    // Decide to zero suppress or not depending on number of hits
    if ( header.zeroSuppressed() )
    {
      dataBank = (RichHPDDataBank*) new RichDAQ_LHCb4::ZeroSuppLHCb( dataStart );
    }
    else
    {
      if ( header.aliceMode() )
      {
        dataBank = (RichHPDDataBank*) new RichDAQ_LHCb4::NonZeroSuppAlice( dataStart );
      }
      else
      {
        dataBank = (RichHPDDataBank*) new RichDAQ_LHCb4::NonZeroSuppLHCb( dataStart );
      }
    }

  }
  else if ( RichDAQ::LHCb3 == version )
  {

    // Header
    const RichDAQ_LHCb3::Header header( dataStart );

    // Decide to zero suppress or not depending on number of hits
    if ( header.zeroSuppressed() )
    {
      dataBank = (RichHPDDataBank*) new RichDAQ_LHCb3::ZeroSuppLHCb( dataStart );
    }
    else
    {
      if ( header.aliceMode() )
      {
        dataBank = (RichHPDDataBank*) new RichDAQ_LHCb3::NonZeroSuppAlice( dataStart );
      }
      else
      {
        dataBank = (RichHPDDataBank*) new RichDAQ_LHCb3::NonZeroSuppLHCb( dataStart );
      }
    }

  }
  else if ( RichDAQ::LHCb2 == version )
  {

    // Header
    const RichDAQ_LHCb2::Header header( dataStart );

    // Decide to zero suppress or not depending on number of hits
    if ( header.zeroSuppressed() )
    {
      dataBank = (RichHPDDataBank*) new RichDAQ_LHCb2::ZeroSuppLHCb( dataStart, dataSize );
    }
    else
    {
      dataBank = (RichHPDDataBank*) new RichDAQ_LHCb2::NonZeroSuppLHCb( dataStart );
    }

  }
  else if ( RichDAQ::LHCb1 == version )
  {

    // Header
    const RichDAQ_LHCb1::Header header( dataStart );

    // Decide to zero suppress or not depending on number of hits
    if ( header.zeroSuppressed() )
    {
      dataBank = (RichHPDDataBank*) new RichDAQ_LHCb1::ZeroSuppLHCb( dataStart, dataSize );
    }
    else
    {
      dataBank = (RichHPDDataBank*) new RichDAQ_LHCb1::NonZeroSuppLHCb( dataStart );
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

void RichRawDataFormatTool::fillRawEvent( const LHCb::RichSmartID::Vector & smartIDs,
                                          const RichDAQ::BankVersion version ) const
{

  // new rich data map
  RichDAQ::L1Map L1Data = m_dummyMap;

  // Loop over digits and sort according to L1 and HPD
  for ( RichSmartID::Vector::const_iterator iDigit = smartIDs.begin();
        iDigit != smartIDs.end(); ++iDigit )
  {

    // Get Level 1 ID number
    const RichDAQ::Level1ID L1ID = m_richSys->level1ID( *iDigit );

    // Get reference to L1 group
    RichDAQ::PDHashMap & PDs = L1Data[ L1ID ];

    // Finally, insert this pixel into correct place
    PDs[ (*iDigit).hpdID() ].push_back( *iDigit );

  }

  // Loop over Level1 board and make a RAWBank for each
  for ( RichDAQ::L1Map::const_iterator iL1 = L1Data.begin();
        iL1 != L1Data.end(); ++iL1 )
  {

    // Make a new data bank
    RichDAQ::RAWBank dataBank;

    // Loop over each active HPD for this Level1 board and fill RAWBank
    unsigned nHits(0);
    if ( !(*iL1).second.empty() )
    {
      for ( RichDAQ::PDHashMap::const_iterator iHPD = (*iL1).second.begin();
            iHPD != (*iL1).second.end(); ++iHPD )
      {
        // Get raw data bank for this HPD, and fill into RAWBank
        const RichHPDDataBank * hpdData = createDataBank( (*iHPD).second, version );
        if ( hpdData )
        {
          hpdData->fillRAWBank( dataBank );
          delete hpdData;
          nHits += (*iHPD).second.size();
        }
        else
        {
          Warning( "No RICH Data Bank Created" );
        }

      } // end photon detector loop
    }

    // Add this bank to the Raw buffer
    rawEvent()->addBank( (*iL1).first.data(), RawBank::Rich, version, dataBank );

    if ( m_summary )
    {
      // Count the number of banks and size
      L1CountAndSize & cands = m_l1encodeSummary[ L1IDandV(version,(*iL1).first) ];
      // Increment bank size
      cands.second.first += nL1HeaderWords+dataBank.size(); // L1 headers + data words
      // Increment hit occupancy
      cands.second.second += nHits;
      // Count number of HPD banks
      cands.first += (*iL1).second.size();
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

void RichRawDataFormatTool::decodeToSmartIDs( const RawBank & bank,
                                              RichDAQ::PDMap & smartIDs ) const
{
  // Check this is a RICH bank
  if ( bank.type() != RawBank::Rich )
  {
    Exception( "BankType is not RICH : type = " +
               boost::lexical_cast<std::string>(bank.type()) );
  }

  // if configured, dump raw event before decoding
  if      ( msgLevel(MSG::DEBUG) ) { dumpRawBank( bank, debug() ); }
  else if ( m_dumpBanks          ) { dumpRawBank( bank, info()  ); }

  // Get bank version
  const RichDAQ::BankVersion version = bankVersion( bank );

  // If old format, use old decoding method
  // To be removed when no longer needed
  if ( version == RichDAQ::LHCb0 ||
       version == RichDAQ::LHCb1 ||
       version == RichDAQ::LHCb2 ) return decodeToSmartIDs_OLD(bank,smartIDs);

  // Else new style .....

  // Get L1 ID
  const RichDAQ::Level1ID L1ID ( bank.sourceID() );

  // HPD count
  unsigned int nHPDbanks(0), decodedHits(0);

  // Data bank size in words
  const int bankSize = bank.size() / 4;

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
      const RichHPDDataBank * hpdBank ( createDataBank( &bank.data()[lineC], // pointer to start of data
                                                        0,                   // Not needed here (to be removed). Must be 0 though
                                                        version ) );

      // get HPD RichSmartID
      const RichSmartID hpdID = m_richSys->richSmartID( hpdBank->level0ID() );

      // decode to smartIDs
      LHCb::RichSmartID::Vector newids;
      const unsigned int hpdHitCount = hpdBank->fillRichSmartIDs( newids, hpdID );
      for ( LHCb::RichSmartID::Vector::const_iterator iS = newids.begin();
            iS != newids.end(); ++iS ) { smartIDs[hpdID].push_back(*iS); }

      // Do data integrity checks
      hpdBank->checkDataIntegrity(newids,warning());

      if ( msgLevel(MSG::VERBOSE) )
      {
        // printout decoded RichSmartIDs
        verbose() << " Decoded RichSmartIDs :-" << endreq;
        for ( RichSmartID::Vector::const_iterator iID = smartIDs[hpdID].begin();
              iID != smartIDs[hpdID].end(); ++iID )
        {
          verbose() << "   " << *iID << endreq;
        }
      }

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

      // Increment line number to next data block
      lineC += hpdBank->nTotalWords();

      // delete working object
      delete hpdBank;

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

void RichRawDataFormatTool::decodeToSmartIDs_OLD( const RawBank & bank,
                                                  RichDAQ::PDMap & smartIDs ) const
{

  // Get bank version and ID
  const RichDAQ::Level1ID L1ID ( bank.sourceID() );
  const RichDAQ::BankVersion version = bankVersion( bank );

  // Get max data size for LHCb mode
  const RichDAQ::ShortType maxDataSize = RichDAQ::MaxDataSize;

  // HPD count
  unsigned int nHPDbanks(0), decodedHits(0);

  // Data bank size in words
  const int bankSize = bank.size() / 4;

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
        const RichHPDDataBank * hpdBank ( createDataBank( &bank.data()[lineHeader],
                                                          dataSize,
                                                          version ) );
        // get hit count
        const unsigned int hpdHitCount = hpdBank->hitCount();
        // get HPD RichSmartID
        const RichSmartID hpdID = m_richSys->richSmartID( hpdBank->level0ID() );
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
RichRawDataFormatTool::decodeToSmartIDs( RichDAQ::PDMap & smartIDs ) const
{

  // Get the banks for the Rich
  const LHCb::RawBank::Vector & richBanks = rawEvent()->banks( RawBank::Rich );

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
    debug() << "Decoded " << smartIDs.size()
            << " HPDs from " << richBanks.size() << " RICH L1 bank(s)" << endreq;
  }

}

RawEvent * RichRawDataFormatTool::rawEvent() const
{
  if ( !m_rawEvent )
  {
    // Try and load from TES
    SmartDataPtr<RawEvent> rawEventTES( evtSvc(), m_rawEventLoc );
    if ( rawEventTES )
    {
      m_rawEvent = rawEventTES;
    }
    else
    {
      Exception ( "Unable to access RawEvent" );
      m_rawEvent = NULL;
    }
  }

  return m_rawEvent;
}

void RichRawDataFormatTool::dumpRawBank( const LHCb::RawBank & bank,
                                         MsgStream & os ) const
{

  // Get bank version and ID
  const RichDAQ::Level1ID L1ID ( bank.sourceID() );
  const RichDAQ::BankVersion version = bankVersion( bank );

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
