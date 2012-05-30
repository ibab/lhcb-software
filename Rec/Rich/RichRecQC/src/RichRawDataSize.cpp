
//-----------------------------------------------------------------------------
/** @file RichRawDataSize.cpp
 *
 *  Implementation file for monitor : Rich::DAQ::RawDataSize
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2008-10-14
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichRawDataSize.h"

//-----------------------------------------------------------------------------

using namespace Rich::DAQ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RawDataSize )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawDataSize::RawDataSize( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HistoAlgBase       ( name , pSvcLocator ),
    m_SmartIDDecoder   ( NULL  ),
    m_RichSys          ( NULL  ),
    m_taeEvents        ( 1, "" )
{
  declareProperty( "RawEventLocations", m_taeEvents );
  declareProperty( "FillDetailedPlots", m_detailedPlots = false );
  declareProperty( "WriteTextFile",     m_writeTextFile = false );
  setProperty( "HistoPrint", false );
}

//=============================================================================
// Destructor
//=============================================================================
RawDataSize::~RawDataSize() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RawDataSize::initialize()
{
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, 0, true );

  // RichDet
  m_RichSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  return sc;
}

StatusCode RawDataSize::prebookHistograms()
{
  const unsigned int nL1sMax = m_RichSys->level1HardwareIDs().size();
  const unsigned int nHPDs   = m_RichSys->allPDRichSmartIDs().size();

  // Size per L1 board
  richProfile1D( HID("L1s/SizeVL1CopyNumber"),
                 "Average Size (32bit words) V L1 Copy Number",
                 -0.5, nL1sMax - 0.5, nL1sMax );

  // size per L1 ingress
  richProfile1D( HID("L1s/SizeVL1Ingress"),
                 "Average Size (32bit words) V (L1 Copy Number)*10 + L1 Ingress",
                 -0.5, nL1sMax*10 - 0.5, nL1sMax*10 );

  // size per HPD
  richProfile1D( HID("hpds/SizeVHPDCopyNumber"),
                 "Average Size (32bit words) V HPD Copy Number",
                 -0.5, nHPDs - 0.5, nHPDs );

  return StatusCode::SUCCESS;
}

void RawDataSize::initHPDMap( HPDWordMap & hpdMap )
{
  // clear the map
  hpdMap.clear();
  // get list of all active HPDs
  const LHCb::RichSmartID::Vector & hpds = m_RichSys->activePDRichSmartIDs();
  // Loop over all HPDs and (re)set count to zero
  for ( LHCb::RichSmartID::Vector::const_iterator iHPD = hpds.begin();
        iHPD != hpds.end(); ++iHPD )
  {
    hpdMap[*iHPD] = 0;
  }
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RawDataSize::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  for ( std::vector<std::string>::const_iterator iLoc = m_taeEvents.begin();
        iLoc != m_taeEvents.end(); ++iLoc )
  {
    sc = sc && processTAEEvent( *iLoc );
  }

  return sc;
}

StatusCode RawDataSize::processTAEEvent( const std::string & taeEvent )
{

  // Map of words per L1 board
  Rich::Map<const Rich::DAQ::Level1HardwareID,unsigned int> l1SizeMap;

  // full TES location for this raw event
  const std::string reLoc = ( taeEvent.empty() ?
                              LHCb::RawEventLocation::Default :
                              taeEvent + "/" + LHCb::RawEventLocation::Default );

  // load the raw RICH data at this location
  if ( exist<LHCb::RawEvent>(reLoc) )
  {
    LHCb::RawEvent * rawEvent = get<LHCb::RawEvent>(reLoc);

    // Get the banks for the Rich
    const LHCb::RawBank::Vector & richBanks = rawEvent->banks( LHCb::RawBank::Rich );

    // Loop over data banks
    for ( LHCb::RawBank::Vector::const_iterator iBank = richBanks.begin();
          iBank != richBanks.end(); ++iBank )
    {
      const unsigned int L1size = (*iBank)->size() / 4; // size in # 32 bit words
      const Level1HardwareID L1ID ( (*iBank)->sourceID() );
      l1SizeMap[L1ID] += L1size;
    } // raw banks

    // get the decoded data for this tae event
    const Rich::DAQ::L1Map & l1Map = m_SmartIDDecoder->allRichSmartIDs(taeEvent);

    // HPD word counts
    HPDWordMap hpdWordMap;
    // initialise for all active HPDs and L1 to 0
    initHPDMap(hpdWordMap);

    // loop over decoded data
    for ( Rich::DAQ::L1Map::const_iterator iL1Map = l1Map.begin();
          iL1Map != l1Map.end(); ++iL1Map )
    {
      const Rich::DAQ::Level1HardwareID l1HardID = iL1Map->first;
      const Rich::DAQ::Level1CopyNumber l1CopyN  = m_RichSys->copyNumber(l1HardID);
      const Rich::DAQ::IngressMap & ingressMap   = iL1Map->second;

      // Number words for this L1 board
      // Start with 1 word per HPD ingress header
      unsigned int nL1Words( ingressMap.size() );

      for ( Rich::DAQ::IngressMap::const_iterator iIngressMap = ingressMap.begin();
            iIngressMap != ingressMap.end(); ++iIngressMap )
      {
        const Rich::DAQ::IngressInfo & ingressInfo = iIngressMap->second;
        const Rich::DAQ::HPDMap & hpdMap = ingressInfo.hpdData();
        const Rich::DAQ::L1IngressID ingressID = ingressInfo.ingressHeader().ingressID();

        // Number words for this L1 ingress
        // Start with 1 for the header
        unsigned int nIngressWords(1);

        for ( Rich::DAQ::HPDMap::const_iterator iHPDMap = hpdMap.begin();
              iHPDMap != hpdMap.end(); ++iHPDMap )
        {
          const Rich::DAQ::HPDInfo & hpdInfo           = iHPDMap->second;
          const LHCb::RichSmartID  & hpdID             = hpdInfo.hpdID();
          const Rich::DAQ::HPDInfo::Header & hpdHeader = hpdInfo.header();
          const Rich::DAQ::HPDInfo::Footer & hpdFooter = hpdInfo.footer();

          // header+footer words for this HPD
          unsigned int nHPDwords = ( hpdHeader.nHeaderWords() +
                                     hpdFooter.nFooterWords() );

          // count data words only for valid HPD data blocks
          const bool hpdOK = !hpdHeader.inhibit() && hpdID.isValid();
          if ( hpdOK ) nHPDwords += hpdHeader.nDataWords();

          // count words per L1 board
          nL1Words += nHPDwords;

          // count words per ingress
          nIngressWords += nHPDwords;

          // count words and hits per HPD (for valid SmartIDs)
          if ( hpdID.isValid() ) { hpdWordMap[hpdID] = nHPDwords; }

        } // loop over HPDs

        // fill ingress plot
        richProfile1D( HID("L1s/SizeVL1Ingress") )
          -> fill ( 10*l1CopyN.data() + ingressID.data(), nIngressWords );
        if ( m_detailedPlots )
        {
          const Rich::DAQ::Level1LogicalID l1LogID = m_RichSys->level1LogicalID(l1HardID);
          const Rich::DetectorType rich            = m_RichSys->richDetector(l1HardID);
          std::ostringstream ID, title;
          ID << "L1s/" << rich 
             << "/L1-HardID" << l1HardID << "LogID" << l1LogID << "Ingress" << ingressID;
          title << "L1 Data Size (32bit words) | " << rich << " | HardwareID " << l1HardID 
                << " LogicalID " << l1LogID
                << " Ingress " << ingressID;
          richHisto1D( ID.str(), title.str(), -0.5, 200.5, 201 ) -> fill ( nIngressWords );
        }

      } // loop over ingresses

      richProfile1D( HID("L1s/SizeVL1CopyNumber") ) -> fill ( l1CopyN.data(), nL1Words );
      if ( m_detailedPlots )
      {
        const Rich::DAQ::Level1LogicalID l1LogID = m_RichSys->level1LogicalID(l1HardID);
        const Rich::DetectorType rich            = m_RichSys->richDetector(l1HardID);
        std::ostringstream ID, title;
        ID << "L1s/" << rich
           << "/L1-HardID" << l1HardID << "LogID" << l1LogID;
        title << "L1 Data Size (32bit words) | " << rich << " | HardwareID " << l1HardID 
              << " LogicalID " << l1LogID;
        richHisto1D( ID.str(), title.str(), -0.5, 500.5, 501 ) -> fill ( nL1Words );
      }

      // Cross check with L1 size direct from raw bank
      if ( nL1Words != l1SizeMap[l1HardID] )
      {
        std::ostringstream mess;
        mess << "L1 size mis-match | HardwareID " << l1HardID
             << " RawSize=" << l1SizeMap[l1HardID]
             << " DecodedSize=" << nL1Words;
        Warning( mess.str() ).ignore();
      }

    } // loop over L1 boards

    // loop over HPDs
    for ( HPDWordMap::const_iterator iHPD = hpdWordMap.begin();
          iHPD != hpdWordMap.end(); ++iHPD )
    {
      if ( iHPD->first.isValid() )
      {
        // use a try block in case of DB lookup errors
        try
        {
          const Rich::DAQ::HPDCopyNumber copyN = m_RichSys->copyNumber(iHPD->first);
          // fill plots
          richProfile1D(HID("hpds/SizeVHPDCopyNumber"))->fill(copyN.data(),iHPD->second);
          if ( m_detailedPlots )
          {
            const Rich::DAQ::HPDHardwareID hpdHardID = m_RichSys->hardwareID(iHPD->first);
            const Rich::DAQ::Level0ID l0ID           = m_RichSys->level0ID(iHPD->first);
            const Rich::DetectorType rich            = iHPD->first.rich();
            std::ostringstream title, ID;
            title << "# Words (32bit) | "
                  << iHPD->first << " L0ID=" << l0ID << " hardID=" << hpdHardID;
            ID << "hpds/" << rich << "/HPDHardwareID" << hpdHardID;
            richHisto1D( ID.str(), title.str(), -0.5, 35.5, 36 ) -> fill( iHPD->second );
          }
        }
        catch ( const GaudiException & excpt )
        {
          Error( excpt.message() ).ignore();
        }
      }
    }

  } // raw event exists

  return StatusCode::SUCCESS;
}

StatusCode RawDataSize::finalize()
{
  if ( m_writeTextFile )
  {

    // load the HPD occ plot
    TProfile * hist =
      Gaudi::Utils::Aida2ROOT::aida2root( richProfile1D(HID("hpds/SizeVHPDCopyNumber")) );
    if ( hist )
    {

      // Open a text file
      const std::string filename = name() + ".txt";
      info() << "Writing HPD data to text file '" << filename << "'" << endmsg;
      std::ofstream file(filename.c_str());

      // loop over bins (ROOT numbers from 1 ....)
      for ( int bin = 1; bin <= hist->GetNbinsX(); ++bin )
      {

        // use a try block in case of DB lookup errors
        try
        {

          // Get HPD data
          const Rich::DAQ::HPDCopyNumber hpdCopyN(bin-1); // convert bin number to copy number
          const LHCb::RichSmartID hpdSmartID         = m_RichSys->richSmartID(hpdCopyN);
          const Rich::DAQ::HPDHardwareID hpdHardID   = m_RichSys->hardwareID(hpdSmartID);
          const Rich::DAQ::Level0ID l0ID             = m_RichSys->level0ID(hpdSmartID);
          const Rich::DAQ::Level1HardwareID l1HardID = m_RichSys->level1HardwareID(l0ID);
          const Rich::DAQ::Level1LogicalID  l1LogID  = m_RichSys->level1LogicalID(l1HardID);
          const Rich::DAQ::Level1Input l1Input       = m_RichSys->level1InputNum(hpdSmartID);

          // HPD occ data from histogram
          const double hpdOcc    ( hist->GetBinContent(bin) );
          const double hpdOccErr ( hist->GetBinError(bin)   );

          // write data to file
          file << hpdCopyN.data() << " "
               << hpdHardID.data() << " "
               << l0ID.data() << " "
               << l1HardID.data() << " "
               << l1LogID.data() << " "
               << l1Input.data() << " "
               << l1Input.ingressID().data() << " "
               << hpdOcc << " " << hpdOccErr
               << std::endl;

        }
        catch ( const GaudiException & excpt )
        {
          Error( excpt.message() ).ignore();
        }

      }

      // close the file
      file.close();

    } // hist OK

  } // write to text file

  // return
  return HistoAlgBase::finalize();
}
