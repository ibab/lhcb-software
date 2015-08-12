
//-----------------------------------------------------------------------------
/** @file RichRawDataSize.cpp
 *
 *  Implementation file for monitor : Rich::DAQ::RawDataSize
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2008-10-14
 */
//-----------------------------------------------------------------------------

// local
#include "RichRawDataSize.h"

//-----------------------------------------------------------------------------

using namespace Rich::DAQ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawDataSize::RawDataSize( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HistoAlgBase       ( name, pSvcLocator ),
    m_SmartIDDecoder   ( NULL  ),
    m_RichSys          ( NULL  )
{
  declareProperty( "TAEEvents",                  m_taeEvents              = {""}  );
  declareProperty( "FillDetailedL1Plots",        m_detailedL1Plots        = true  );
  declareProperty( "FillDetailedL1IngressPlots", m_detailedL1IngressPlots = false );
  declareProperty( "FillDetailedHPDPlots",       m_detailedHPDPlots       = false );
  declareProperty( "WriteTextFile",              m_writeTextFile          = false );
  declareProperty( "RawEventLocations",
                   m_rawEvLocs = {LHCb::RawEventLocation::Rich,LHCb::RawEventLocation::Default} );
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
  acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, NULL, true );

  // RichDet
  m_RichSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  return sc;
}

StatusCode RawDataSize::prebookHistograms()
{
  const unsigned int nL1sMax = m_RichSys->level1HardwareIDs().size();
  const unsigned int nHPDs   = m_RichSys->nPDs();

  // # Headers per L1 board
  richProfile1D( HID("L1s/HeadersVL1CopyNumber"),
                 "# Headers (32bit words) V L1 Copy Number",
                 -0.5, nL1sMax - 0.5, nL1sMax,
                 "UKL1 Copy Number",
                 "Average # 32 bit header words / event" );

  // Size per L1 board
  richProfile1D( HID("L1s/SizeVL1CopyNumber"),
                 "Average Size (32bit words) V L1 Copy Number",
                 -0.5, nL1sMax - 0.5, nL1sMax,
                 "UKL1 Copy Number",
                 "Average # 32 bit data words / event" );

  // size per L1 ingress
  richProfile1D( HID("L1s/SizeVL1Ingress"),
                 "Average Size (32bit words) V (L1 Copy Number)*10 + L1 Ingress",
                 -0.5, nL1sMax*10 - 0.5, nL1sMax*10,
                 "(UKL1 Copy Number)*10 + L1 Ingress",
                 "Average # 32 bit data words / event" );

  // size per HPD
  richProfile1D( HID("hpds/SizeVHPDCopyNumber"),
                 "Average Size (32bit words) V HPD Copy Number",
                 -0.5, nHPDs - 0.5, nHPDs,
                 "PD Copy Number",
                 "Average # 32 bit data words / event" );

  return StatusCode::SUCCESS;
}

void RawDataSize::initHPDMap( HPDWordMap & hpdMap )
{
  // clear the map
  hpdMap.clear();
  // get list of all HPDs
  const LHCb::RichSmartID::Vector & hpds = m_RichSys->allPDRichSmartIDs();
  // Loop over all HPDs and (re)set count to zero
  for ( const auto& hpd : hpds ) { hpdMap[hpd] = 0; }
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RawDataSize::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  for ( const auto& taeEv : m_taeEvents )
  {
    sc = sc && processTAEEvent(taeEv);
  }

  return sc;
}

StatusCode RawDataSize::processTAEEvent( const std::string & taeEvent )
{

  // Map of words per L1 board
  Rich::Map<const Rich::DAQ::Level1HardwareID,unsigned int> l1SizeMap;

  // Event prefix
  const std::string prefixE = ( taeEvent.empty() ? "" : taeEvent + "/" );

  // Try and load the RawEvent from the list of possible locations
  LHCb::RawEvent * rawEvent = NULL;
  for ( const auto& evLoc : m_rawEvLocs )
  {
    rawEvent = getIfExists<LHCb::RawEvent>( prefixE + evLoc );
    if ( rawEvent ) break;
  }

  // If raw event was found
  if ( rawEvent )
  {
    // Get the banks for the Rich
    const LHCb::RawBank::Vector & richBanks = rawEvent->banks( LHCb::RawBank::Rich );

    // Loop over data banks
    for ( const auto* bank : richBanks )
    {
      const unsigned int L1size = bank->size() / 4; // size in # 32 bit words
      const Level1HardwareID L1ID ( bank->sourceID() );
      l1SizeMap[L1ID] += L1size;
    } // raw banks

    // get the decoded data for this tae event
    const Rich::DAQ::L1Map & l1Map = m_SmartIDDecoder->allRichSmartIDs(taeEvent);

    // HPD word counts
    HPDWordMap hpdWordMap;
    // initialise for all HPDs and L1 to 0
    initHPDMap(hpdWordMap);

    // loop over decoded data
    for ( const auto& l1Data : l1Map )
    {
      const Rich::DAQ::Level1HardwareID l1HardID = l1Data.first;
      const Rich::DAQ::Level1CopyNumber l1CopyN  = m_RichSys->copyNumber(l1HardID);
      const Rich::DAQ::IngressMap & ingressMap   = l1Data.second;

      // Number words for this L1 board
      // Start with 1 word per HPD ingress header
      unsigned int nL1HeaderWords = ingressMap.size();
      unsigned int nL1Words       = nL1HeaderWords;

      for ( const auto& ingress : ingressMap )
      {
        const Rich::DAQ::IngressInfo & ingressInfo = ingress.second;
        const Rich::DAQ::HPDMap           & hpdMap = ingressInfo.hpdData();
        const Rich::DAQ::L1IngressID ingressID = ingressInfo.ingressHeader().ingressID();

        // Number words for this L1 ingress
        // Start with 1 for the header
        unsigned int nIngressWords(1);

        for ( const auto& hpd : hpdMap )
        {
          const Rich::DAQ::HPDInfo & hpdInfo           = hpd.second;
          const LHCb::RichSmartID  & hpdID             = hpdInfo.hpdID();
          const Rich::DAQ::HPDInfo::Header & hpdHeader = hpdInfo.header();
          const Rich::DAQ::HPDInfo::Footer & hpdFooter = hpdInfo.footer();

          // header+footer words for this HPD
          const unsigned int nHPDHeaderwords = ( hpdHeader.nHeaderWords() +
                                                 hpdFooter.nFooterWords() );
          unsigned int nHPDwords = nHPDHeaderwords;

          // count data words only for valid HPD data blocks
          const bool hpdOK = !hpdHeader.inhibit() && hpdID.isValid();
          if ( hpdOK ) nHPDwords += hpdHeader.nDataWords();

          // count words per L1 board
          nL1Words       += nHPDwords;
          nL1HeaderWords += nHPDHeaderwords;

          // count words per ingress
          nIngressWords += nHPDwords;

          // count words and hits per HPD (for valid SmartIDs)
          if ( hpdID.isValid() ) { hpdWordMap[hpdID] = nHPDwords; }

        } // loop over HPDs

        // fill ingress plot
        richProfile1D( HID("L1s/SizeVL1Ingress") )
          -> fill ( 10*l1CopyN.data() + ingressID.data(), nIngressWords );

        // Fill detailed plots
        if ( UNLIKELY(m_detailedL1IngressPlots) )
        {
          const Rich::DAQ::Level1LogicalID l1LogID = m_RichSys->level1LogicalID(l1HardID);
          const Rich::DetectorType rich            = m_RichSys->richDetector(l1HardID);
          std::ostringstream ID, title;
          ID << "L1s/" << rich
             << "/L1-HardID" << l1HardID << "LogID" << l1LogID << "Ingress" << ingressID;
          title << "L1 Data Size (32bit words) | " << rich << " | HardwareID " << l1HardID
                << " LogicalID " << l1LogID
                << " Ingress " << ingressID;
          richHisto1D( ID.str(), title.str(), -0.5, 199.5, 50 ) -> fill ( nIngressWords );
        }

      } // loop over ingresses

      // fill L1 plots
      richProfile1D( HID("L1s/SizeVL1CopyNumber")    ) -> fill ( l1CopyN.data(), nL1Words );
      richProfile1D( HID("L1s/HeadersVL1CopyNumber") ) -> fill ( l1CopyN.data(), nL1HeaderWords );
      if ( m_detailedL1Plots )
      {
        const Rich::DAQ::Level1LogicalID l1LogID = m_RichSys->level1LogicalID(l1HardID);
        const Rich::DetectorType rich            = m_RichSys->richDetector(l1HardID);
        std::ostringstream ID, title;
        ID << "L1s/" << rich
           << "/L1-HardID" << l1HardID << "LogID" << l1LogID;
        title << "L1 Data Size (32bit words) | " << rich << " | HardwareID " << l1HardID
              << " LogicalID " << l1LogID;
        richHisto1D( ID.str(), title.str(), -0.5, 499.5, 50 ) -> fill ( nL1Words );
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
    for ( const auto& hpd : hpdWordMap )
    {
      if ( hpd.first.isValid() )
      {
        // use a try block in case of DB lookup errors
        try
        {
          const Rich::DAQ::HPDCopyNumber copyN = m_RichSys->copyNumber(hpd.first);
          // fill plots
          richProfile1D(HID("hpds/SizeVHPDCopyNumber"))->fill(copyN.data(),hpd.second);
          if ( UNLIKELY(m_detailedHPDPlots) )
          {
            const Rich::DAQ::HPDHardwareID hpdHardID = m_RichSys->hardwareID(hpd.first);
            const Rich::DAQ::Level0ID l0ID           = m_RichSys->level0ID(hpd.first);
            const Rich::DetectorType rich            = hpd.first.rich();
            std::ostringstream title, ID;
            title << "# Words (32bit) | "
                  << hpd.first << " L0ID=" << l0ID << " hardID=" << hpdHardID;
            ID << "hpds/" << rich << "/HPDHardwareID" << hpdHardID;
            richHisto1D( ID.str(), title.str(), -0.5, 35.5, 36 ) -> fill( hpd.second );
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
  // write text file
  if ( m_writeTextFile ) { writeToTextFile(); } 
  // return
  return HistoAlgBase::finalize();
}

//=============================================================================

void RawDataSize::writeToTextFile() const
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

}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RawDataSize )

//=============================================================================
