
//-----------------------------------------------------------------------------
/** @file RichPixelRecoEffMonitor.cpp
 *
 *  Implementation file for algorithm class : RichPixelRecoEffMonitor
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#include <fstream>

// local
#include "RichPixelRecoEffMonitor.h"

// namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PixelRecoEffMonitor )

// Standard constructor, initializes variables
PixelRecoEffMonitor::PixelRecoEffMonitor( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : HistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( 0 ),
    m_truth             ( 0 ),
    m_richSys           ( 0 ) { }

// Destructor
PixelRecoEffMonitor::~PixelRecoEffMonitor() { }

//  Initialize
StatusCode PixelRecoEffMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );
  acquireTool( "RichMCTruthTool",    m_truth,   NULL, true );
  acquireTool( "RichSmartIDDecoder", m_decoder, NULL, true );

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  // Force creation of tools some here, to avoid it happening during first
  // event and perturbing timing numbers
  pixelCreator();

  return sc;
}

// Main execution
StatusCode PixelRecoEffMonitor::execute()
{
  debug() << "Execute" << endmsg;

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all pixels have been formed
  if ( richPixels()->empty() )
  {
    if ( pixelCreator()->newPixels().isFailure() )
      return Error( "Problem creating RichRecPixels" );
    debug() << "No Pixels found : Created "
            << richPixels()->size() << " RichRecPixels" << endmsg;
  }

  // Histogramming
  const Rich::HistoID hid;
  //PD_GLOBAL_POSITIONS;
  //PD_LOCAL_POSITIONS;

  typedef std::vector<const LHCb::MCRichDigitSummary*> Summaries;

  // Obtain data from raw decoding
  const Rich::DAQ::L1Map & data = m_decoder->allRichSmartIDs();

  // Loop over L1 boards
  for ( Rich::DAQ::L1Map::const_iterator iL1 = data.begin();
        iL1 != data.end(); ++iL1 )
  {
    // loop over ingresses for this L1 board
    for ( Rich::DAQ::IngressMap::const_iterator iIn = (*iL1).second.begin();
          iIn != (*iL1).second.end(); ++iIn )
    {
      // Loop over HPDs in this ingress
      for ( Rich::DAQ::HPDMap::const_iterator iHPD = (*iIn).second.hpdData().begin();
            iHPD != (*iIn).second.hpdData().end(); ++iHPD )
      {      
        // Valid HPD ID
        if ( !(*iHPD).second.hpdID().isValid() ) { continue; }
        // inhibited HPD ?
        if ( (*iHPD).second.header().inhibit() ) { continue; }
        
        // HPD ID
        const LHCb::RichSmartID hpd = (*iHPD).second.hpdID();
        // Vector of SmartIDs
        const LHCb::RichSmartID::Vector & rawIDs = (*iHPD).second.smartIDs();
        // Hardware ID
        const Rich::DAQ::HPDHardwareID hardID = m_richSys->hardwareID(hpd);
        // HPD as a string
        std::ostringstream HPD; HPD << hpd;

        // HPD backgrounds
        bool isBkg(false), isHPDQCK(false);

        // Loop over raw RichSmartIDs
        for ( LHCb::RichSmartID::Vector::const_iterator iR = rawIDs.begin();
              iR != rawIDs.end(); ++iR )
        {
          // get MC histories for this RichSmartID
          Summaries summaries;
          m_truth->getMcHistories( *iR, summaries );
          // loop over summaries and see if this HPD has any background
          for ( Summaries::const_iterator iS = summaries.begin();
                iS != summaries.end(); ++iS )
          {
            if ( (*iS)->history().isBackground() ) { isBkg = true;    }
            if ( (*iS)->history().hpdQuartzCK()  ) { isHPDQCK = true; }
            if ( isBkg && isHPDQCK ) break;
          }
        }

        // Get the reconstructed pixels for this HPD
        IPixelCreator::PixelRange range = pixelCreator()->range(hpd);
        const unsigned int nHPDs = range.size();

        // pixel reco efficiency
        const double pixEff = ( rawIDs.size()>0 ? 100 * nHPDs/rawIDs.size() : 0 );

        // plots
        plot1D( nHPDs, hid(hpd.rich(),"allhpdOcc"), "Overall : HPD Occupancy (nDigits>0)", -0.5, 200.5, 201 );
        plot1D( pixEff, hid(hpd.rich(),"allEff"), "Overall : Pixel Reco Eff", -0.5, 100.5, 101 );
        if ( isBkg )
          plot1D( pixEff, hid(hpd.rich(),"bkgEff"), "Background : Pixel Reco Eff", -0.5, 100.5, 101 );
        if ( isHPDQCK )
          plot1D( pixEff, hid(hpd.rich(),"qckEff"), "HPD Quartz CK : Pixel Reco Eff", -0.5, 100.5, 101 );

        // individual HPD plots
        plot1D( nHPDs, hid(hpd.rich(),"hpdOcc/"+(std::string)hardID), "HPD Occupancy (nDigits>0)", -0.5, 200.5, 201 );
        plot1D( pixEff, hid(hpd.rich(),"hpdOverallEff/"+(std::string)hardID),
                "Pixel Reco Eff : "+HPD.str(), -0.5, 100.5, 101 );
        if ( isBkg )
          plot1D( pixEff, hid(hpd.rich(),"hpdBkgEff/"+(std::string)hardID),
                  "Pixel Reco Eff : "+HPD.str(), -0.5, 100.5, 101 );
        if ( isHPDQCK )
          plot1D( pixEff, hid(hpd.rich(),"hpdQckEff/"+(std::string)hardID),
                  "HPD Quartz CK Eff : "+HPD.str(), -0.5, 100.5, 101 );

        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << hpd << " Raw pixels " << rawIDs.size() << " reco " << nHPDs << endmsg;
        }

      } // HPDs
    } // ingresses
  } // L1 boards

  return StatusCode::SUCCESS;
}

