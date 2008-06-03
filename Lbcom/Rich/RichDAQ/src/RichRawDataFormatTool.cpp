
//-----------------------------------------------------------------------------
/** @file RichRawDataFormatTool.cpp
 *
 *  Implementation file for class : Rich::RawDataFormatTool
 *
 *  CVS Log :-
 *  $Id: RichRawDataFormatTool.cpp,v 1.67 2008-06-03 12:46:57 jonrob Exp $
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
  /// Default 'fake' HPD RichSmartID
  static const LHCb::RichSmartID s_fakeHPDID( Rich::Rich1,Rich::top,0,0 );
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
  declareProperty( "DumpRawBanks",       m_dumpBanks          = false );
  declareProperty( "UseZeroSuppression", m_zeroSupp           = true );
  declareProperty( "UseExtendedFormat",  m_extendedFormat     = false );
  declareProperty( "DecodeUsingODIN",    m_decodeUseOdin      = false );
  declareProperty( "CheckDataIntegrity", m_checkDataIntegrity = true );
  declareProperty( "CheckEventIDs",      m_checkEventsIDs     = true );
  declareProperty( "CheckBXIDs",         m_checkBxIDs         = true );
  declareProperty( "UseFakeHPDID",       m_useFakeHPDID       = false );
}

// Destructor
RawDataFormatTool::~RawDataFormatTool() {}

StatusCode RawDataFormatTool::initialize()
{
  // Initialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  // if suppression is less than max possible number of (ALICE) hits, print a message.
  if ( m_maxHPDOc < BitsPerDataWord * MaxDataSizeALICE )
  {
    info() << "Will suppress HPDs with more than " << m_maxHPDOc << " digits" << endreq;
  }

  if ( m_extendedFormat ) info() << "Will encode RawEvent using extended HPD format" << endreq;

  if ( m_decodeUseOdin )       Warning( "ODIN integrity checks are enabled", StatusCode::SUCCESS );
  if ( !m_checkDataIntegrity ) Warning( "HPD Data integrity checks are disabled", StatusCode::SUCCESS );
  if ( !m_checkEventsIDs )     Warning( "Header Event ID checks are disabled", StatusCode::SUCCESS );
  if ( !m_checkBxIDs )         Warning( "Header BX ID checks are disabled", StatusCode::SUCCESS );

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

    std::map<Rich::DetectorType,unsigned long> totWordSize, totBanks, totHits;
    Rich::DetectorType lastrich = Rich::InvalidDetector;
    for ( L1TypeCount::const_iterator iL1C = count.begin(); iL1C != count.end(); ++iL1C )
    {
      const Level1ID      L1ID   = (*iL1C).first.second;
      const BankVersion version  = (*iL1C).first.first;
      Rich::DetectorType rich;
      try                                   { rich = m_richSys->richDetector( L1ID ); }
      catch ( const GaudiException & expt ) { rich = Rich::InvalidDetector;           }
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

      info() << " " << rich << " L1 " << boost::format("%3i") % L1ID.data();
      info() << " V" << boost::format("%3i") % version;
      info() << " | L1 size ="
             << occ1(nBanks,m_evtCount) << " hpds :"
             << occ2(words,m_evtCount) << " words :"
             << occ2(hits,m_evtCount) << " hits / event" << endreq;
    }

    info() << "----------------------------------------------------------------------------------------------------------" << endreq;
    if ( totBanks[Rich::Rich1]>0 )
    {
      info() << " Rich1 Average     | L1 size =" << occ1(totBanks[Rich::Rich1],m_evtCount) << " hpds :"
             << occ2(totWordSize[Rich::Rich1],m_evtCount) << " words :"
             << occ2(totHits[Rich::Rich1],m_evtCount) << " hits / event" << endreq;
    }
    if ( totBanks[Rich::Rich2]>0 )
    {
      info() << " Rich2 Average     | L1 size =" << occ1(totBanks[Rich::Rich2],m_evtCount) << " hpds :"
             << occ2(totWordSize[Rich::Rich2],m_evtCount) << " words :"
             << occ2(totHits[Rich::Rich2],m_evtCount) << " hits / event" << endreq;
    }

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
    verbose() << endreq << "   Creating encoding HPD " << smartIDs.front().hpdID()
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
        Warning ( "LHCb4 data format does not support ZS Alice mode data" );
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
        Warning ( "LHCb3 data format does not support ZS Alice mode data" );
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
  else
  {
    Exception ( "Unknown RICH Raw Buffer version " +
                boost::lexical_cast<std::string>(version) + " -> No data bank" );
  }

  // Printout this bank
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << endreq
              << "Created HPD Data Bank for Decoding :-" << endreq;
    verbose() << *dataBank << endreq;
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
    const Level1ID    L1ID    = m_richSys->level1ID       ( *iDigit );
    const Level1Input L1Input = m_richSys->level1InputNum ( *iDigit );

    // Get reference to correct data group
    IngressMap & ingress   = L1Data  [ L1ID                ];
    IngressInfo & hpds     = ingress [ L1Input.ingressID() ];
    HPDInfo & hpdInfo      = (hpds.hpdData())[L1Input];

    // Finally, insert this pixel into correct place
    hpdInfo.smartIDs().push_back( *iDigit );

    // Set the HPD information
    hpdInfo.setHpdID( (*iDigit).hpdID() );

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
    L1IngressID ingress(0);
    for ( IngressMap::const_iterator iIngress = (*iL1).second.begin();
          iIngress != (*iL1).second.end(); ++iIngress, ++ingress )
    {

      // If 2007 data format, add Ingress header word to the raw bank
      // Eventually, should be done by default for all formats
      // (once DC04/06 compatibility is no longer needed)
      // In this case this if clause can be removed
      if ( version != LHCb0 && version != LHCb1 &&
           version != LHCb2 && version != LHCb3 && version != LHCb4 )
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
          debug() << " Ingress " << ingress << " : " << ingressWord << endreq;
      }

      if ( msgLevel(MSG::DEBUG) )
        debug() << "  Found " << (*iIngress).second.hpdData().size() << " HPD(s) with data" << endreq;

      // Loop over each active HPD for this Level1 board ingress and fill RAWBank
      for ( HPDMap::const_iterator iHPD = (*iIngress).second.hpdData().begin();
            iHPD != (*iIngress).second.hpdData().end(); ++iHPD )
      {
        debug() << "   HPD " << (*iHPD).second.hpdID() << " L1input=" << (*iHPD).first 
                << " Found " << (*iHPD).second.smartIDs().size() << " hits" << endreq;
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
      debug() << "Encoded " << boost::format("%2i") % (*iL1).second.size();
      debug() << " ingresses for Level1 Bank "
              << boost::format("%2i") % (*iL1).first.data();
      debug() << " : Size "
              << boost::format("%4i") % (nL1HeaderWords+dataBank.size())
              << " words : Version " << version << endreq;
    }

  } // Level1 loop

}

void RawDataFormatTool::decodeToSmartIDs( const LHCb::RawBank & bank,
                                          L1Map & decodedData ) const
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

  m_hasBeenCalled = true;
  ++m_l1IdsDecoded[ Level1ID(bank.sourceID()) ];

  // Now, delegate the work to a version of the decoding appropriate to the version

  if ( version == LHCb5 ) // latest version
  {
    decodeToSmartIDs_2007(bank,decodedData);
  }
  else if ( version == LHCb3 || version == LHCb4 ) // RICH 2006 Testbeam
  {
    decodeToSmartIDs_2006TB(bank,decodedData);
  }
  else if ( version == LHCb0 ||
            version == LHCb1 ||
            version == LHCb2 )  // DC04 or DC06
  {
    decodeToSmartIDs_DC0406(bank,decodedData);
  }
  else // Some problem ...
  {
    std::ostringstream mess;
    mess << "Unknown RICH L1 version number " << version;
    Exception( mess.str() );
  }

}

void RawDataFormatTool::decodeToSmartIDs_2007( const LHCb::RawBank & bank,
                                               L1Map & decodedData ) const
{
  // Get L1 ID
  const Level1ID L1ID ( bank.sourceID() );
  if ( msgLevel(MSG::DEBUG) ) debug() << "Decoding L1 bank " << L1ID << endreq;

  // various counts
  unsigned int nHPDbanks(0), decodedHits(0);

  // Data bank size in words
  const int bankSize = bank.size() / 4;

  // Get bank version
  const BankVersion version = bankVersion( bank );

  // Flag to indicate if a given L1 bank has been printed out in case of an error
  bool l1BankErrorDump = true;

  if ( bankSize > 0 )
  {

    // ... otherwise, must have at least 2 entries
    if ( bankSize < 2 )
    {
      Exception( "Non-empty RICH Bank size is less than 2 !" );
    }

    // Get Ingress map to decode into for this L1 board
    IngressMap & ingressMap = decodedData[L1ID];

    // Loop over bank, find headers and produce a data bank for each
    // Fill data into RichSmartIDs
    long lineC(0);
    while ( lineC < bankSize )
    {

      // First, the ingress header word (and increment to the next line)
      const L1IngressHeader ingressWord( bank.data()[lineC++] );
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << " Ingress " << ingressWord << endreq;
      }

      // Get data for this ingress
      IngressInfo & ingressInfo = ingressMap[ingressWord.ingressID()];

      // Set ingress header in decoded data map
      ingressInfo.setIngressHeader( ingressWord );

      // Compare Ingress header to the ODIN
      if (  msgLevel(MSG::VERBOSE) )
      {
        verbose() << "ODIN : EventNumber=" << odin()->eventNumber() 
                  << " BunchID=" << odin()->bunchId() << endreq;
      }
      const bool odinOK
        = ( !m_decodeUseOdin ? true :
            ( !m_checkEventsIDs || ingressWord.eventID() == EventID(odin()->eventNumber()) ) &&
            ( !m_checkBxIDs     || ingressWord.bxID()    == BXID   (odin()->bunchId())     ) );
      if ( !odinOK )
      {
        std::ostringstream mess;
        mess << "ODIN Mismatch : L1ID " << L1ID
             << " : ODIN EvID=" << odin()->eventNumber() << " BxID=" << odin()->bunchId()
             << " : L1IngressHeader " << ingressWord
             << " -> Data Suppressed";
        Error( mess.str() );
      }

      // get list of active ingress inputs
      L1IngressInputs inputs;
      ingressWord.activeHPDInputs(inputs);
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "  Found " << inputs.size() << " HPDs with data blocks" << endreq;
      }

      // Check the Ingress supression flag
      if ( ingressWord.hpdsSuppressed() )
      {
        std::ostringstream mess;
        mess << "L1 board " << L1ID << " : Ingress " << ingressWord
             << " is suppressed : " << inputs.size() << " Active HPDs";
        Warning( mess.str(), 1 );
      }
      else
      {
        // Ingress is OK, so read HPD data

        // Loop over active HPDs
        for ( L1IngressInputs::const_iterator iHPD = inputs.begin(); iHPD != inputs.end(); ++iHPD )
        {

          // Create data bank and decode into RichSmartIDs
          std::auto_ptr<const HPDDataBank> hpdBank ( createDataBank( &bank.data()[lineC], // pointer to start of data
                                                                     0, // Not needed here (to be removed). Must be 0 though
                                                                     version ) );

          // is this HPD suppressed ?
          const bool hpdIsSuppressed = hpdBank->suppressed();
          if ( hpdIsSuppressed )
          {
            std::ostringstream mess;
            mess << "L1 board " << L1ID << " : Ingress "
                 << ingressWord.ingressID() << " HPD " << *iHPD << " is suppressed";
            Warning( mess.str(), StatusCode::SUCCESS, 1 );
          }

          // Try to add a new HPDInfo to map
          const Level1Input l1Input(ingressWord.ingressID(),*iHPD);
          std::pair<HPDMap::iterator,bool> p
            = ingressInfo.hpdData().insert( HPDMap::value_type(l1Input,
                                                               HPDInfo(LHCb::RichSmartID(),
                                                                       HPDInfo::Header(hpdBank->headerWords()),
                                                                       HPDInfo::Footer(hpdBank->footerWords()) ) ) );
          if ( !p.second )
          {
            std::ostringstream mess;
            mess << "Found multiple data blocks L1=" << L1ID << " input=" << l1Input;
            Warning( mess.str() );
          }   
          HPDInfo & hpdInfo = p.first->second;
          
          // Only try and decode this HPD if ODIN test was OK
          if ( odinOK && !hpdIsSuppressed )
          {

            // get HPD RichSmartID
            // do in a try block incase HPDID is unknown
            LHCb::RichSmartID hpdID;
            try
            {
              hpdID = ( m_useFakeHPDID ? s_fakeHPDID :
                        m_richSys->richSmartID( hpdBank->level0ID() ) );
            }
            catch ( const GaudiException & expt )
            {
              Error( expt.message() );
            }
            // If the HPD smartID was successfully found, continue with decoding
            if ( hpdID.isValid() )
            {

              if ( msgLevel(MSG::DEBUG) )
                debug() << "   Decoding HPD " << hpdID << endreq;

              // save HPD ID
              hpdInfo.setHpdID(hpdID);

              // local hit count
              unsigned int hpdHitCount(0);

              // smartIDs
              LHCb::RichSmartID::Vector & newids = hpdInfo.smartIDs();

              // Compare Event IDs for errors
              bool OK = ( hpdIsSuppressed ? true :
                          !m_checkEventsIDs || ingressWord.eventID() == hpdBank->eventID() );
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
                OK = ( !m_checkDataIntegrity || hpdBank->checkDataIntegrity(newids,warning()) );
                if ( !OK ) Warning( "HPD data block failed integrity check" );

                if ( msgLevel(MSG::VERBOSE) && hpdHitCount>0 )
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
                  Warning( "Forced suppression of HPD "+hpd.str(), StatusCode::SUCCESS, 0 );
                  newids.clear();
                }

              }
              else
              {
                // decoding error ....
                error() << "Error in decoding -> Data is rejected for HPD " << hpdID << endreq;

                if ( l1BankErrorDump )
                {
                  error() << " -> Dump of raw L1 data :-" << endreq;
                  dumpRawBank( bank, error() );
                  l1BankErrorDump = false;
                }

                error() << " -> Ingress header : " << ingressWord << endreq;
                error() << "  -> ";
                rawDump(error(),ingressWord.data());
                error() << endreq;
                error() << " -> HPD data block : " << endreq;
                error() << "  -> HPD Event ID = " << hpdBank->eventID() << " ";
                rawDump(error(),hpdBank->eventID().data(),hpdBank->eventID().activeBits());
                error() << endreq;
                error() << *hpdBank << endreq;

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
    debug() << "Decoded " << boost::format("%2i") % nHPDbanks;
    debug() << " HPDs from Level1 Bank ID = "
            << boost::format("%2i") % L1ID.data();
    debug() << " : Size " << boost::format("%4i") % (nL1HeaderWords+(bank.size()/4)) << " words : Version "
            << version << endreq;
  }

}

void RawDataFormatTool::decodeToSmartIDs_2006TB( const LHCb::RawBank & bank,
                                                 L1Map & decodedData ) const
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
    long lineC(0);
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
        verbose() << " Decoded RichSmartIDs :-" << endreq;
        for ( LHCb::RichSmartID::Vector::const_iterator iID = newids.begin();
              iID != newids.end(); ++iID )
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
          Warning( "Forced suppression of HPD "+hpd.str(), StatusCode::SUCCESS, 0 );
          newids.clear();
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
    debug() << "Decoded " << boost::format("%2i") % nHPDbanks;
    debug() << " HPDs from Level1 Bank "
            << boost::format("%2i") % L1ID.data();
    debug() << " : Size " << boost::format("%4i") % (nL1HeaderWords+(bank.size()/4)) << " words : Version "
            << version << endreq;
  }

}

void RawDataFormatTool::decodeToSmartIDs_DC0406( const LHCb::RawBank & bank,
                                                 L1Map & decodedData ) const
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

        // get HPD RichSmartID
        const LHCb::RichSmartID hpdID = ( m_useFakeHPDID ? s_fakeHPDID :
                                          m_richSys->richSmartID( hpdBank->level0ID() ) );

        // Vector to decode into
        Rich::DAQ::HPDInfo & hpdInfo = (ingressInfo.hpdData())[ l1Input ];
        hpdInfo.setHpdID(hpdID);
        ++l1Input;
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
          Warning( "Suppressed HPD in "+hpd.str(), StatusCode::SUCCESS, 0 );
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
    debug() << "Decoded " << boost::format("%2i") % nHPDbanks;
    debug() << " HPDs from Level1 Bank "
            << boost::format("%2i") % L1ID.data();
    debug() << " : Size " << boost::format("%4i") % (nL1HeaderWords+(bank.size()/4)) << " words : Version "
            << version << endreq;
  }

}

void
RawDataFormatTool::decodeToSmartIDs( L1Map & decodedData ) const
{
  m_hasBeenCalled = true;

  // Get the banks for the Rich
  const LHCb::RawBank::Vector & richBanks = rawEvent()->banks( LHCb::RawBank::Rich );

  // Purge data container
  decodedData.clear();

  // clear the L1 ID map
  m_l1IdsDecoded.clear();

  // Loop over data banks
  for ( LHCb::RawBank::Vector::const_iterator iBank = richBanks.begin();
        iBank != richBanks.end(); ++iBank )
  {
    // get bank
    LHCb::RawBank * bank = *iBank;
    // test bank is OK
    if ( !bank ) Exception( "Retrieved null pointer to RawBank" );
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
           << " '" << expt.message() << "'";
      Error( mess.str() );
      // dump the full bank
      dumpRawBank( *bank, error() );
    }
  }

  // loop over the L1 map to check we only got each L1 ID once
  for ( Rich::Map<Rich::DAQ::Level1ID,unsigned int>::const_iterator iL1 = m_l1IdsDecoded.begin();
        iL1 != m_l1IdsDecoded.end(); ++iL1 )
  {
    if ( iL1->second > 1 )
    {
      std::ostringstream mess;
      mess << "Found multiple RawBanks (" << iL1->second << ") for L1 ID " << iL1->first;
      Warning( mess.str() );
    }
  }

  // do not print if faking HPDID, since smartIDs.size() then has no meaning
  if ( !m_useFakeHPDID && msgLevel(MSG::DEBUG) )
  {
    debug() << "Decoded in total " << richBanks.size() << " RICH Level1 bank(s)" << endreq;
  }

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
    for ( Level1IDs::const_iterator iID = m_richSys->level1IDs().begin();
          iID != m_richSys->level1IDs().end(); ++iID )
    {
      dummyMap[ *iID ] = ingressMap;
    }
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Created " << dummyMap.size() 
              << " entries in empty L1 map : L1IDs = "
              << m_richSys->level1IDs() << endreq;
    }
  }
  return dummyMap;
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
    const std::string & LINES =
      "----------------------------------------------------------------------------------------------------------------";

    // Bit numbers
    os << LINES << endreq;
    os << "          bit |";
    for ( int iCol = 31; iCol >= 0; --iCol )
    {
      os << boost::format("%3i") % iCol;
    }
    os << endreq;
    os << LINES << endreq;
    typedef unsigned int Dtype;
    for ( const Dtype * dataW = bank.begin<Dtype>(); dataW != bank.end<Dtype>(); ++dataW )
    {
      rawDump( os, *dataW );
      os << endreq;
    }
    os << LINES << endreq;

  }
  else
  {
    os << "  -> Bank is empty" << endreq;
  }

}

// Print the given data word as Hex and as bits, to the given precision
void RawDataFormatTool::rawDump( MsgStream & os,
                                 const LongType word,
                                 const ShortType nBits ) const
{
  std::ostringstream hexW;
  hexW << std::hex << word;
  std::string tmpW = hexW.str();
  if ( tmpW.size() < 8 ) { tmpW = std::string(8-tmpW.size(),'0')+tmpW; }
  os << tmpW << "  |";
  for ( int iCol = nBits-1; iCol >= 0; --iCol )
  {
    os << "  " << isBitOn( word, iCol );
  }
}

