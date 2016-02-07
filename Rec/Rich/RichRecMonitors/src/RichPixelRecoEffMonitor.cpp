
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
  : HistoAlgBase ( name, pSvcLocator ) { }

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
  acquireTool( "RichMCTruthTool",    m_truth,   nullptr, true );
  acquireTool( "RichSmartIDDecoder", m_decoder, nullptr, true );

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
  _ri_debug << "Execute" << endmsg;

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all pixels have been formed
  if ( richPixels()->empty() )
  {
    if ( pixelCreator()->newPixels().isFailure() )
      return Error( "Problem creating RichRecPixels" );
    _ri_debug << "No Pixels found : Created "
              << richPixels()->size() << " RichRecPixels" << endmsg;
  }

  // Histogramming
  const Rich::HistoID hid;
  //PD_GLOBAL_POSITIONS;
  //PD_LOCAL_POSITIONS;

  typedef std::vector<const LHCb::MCRichDigitSummary*> Summaries;

  // Obtain data from raw decoding
  const auto & data = m_decoder->allRichSmartIDs();

  // Loop over L1 boards
  for ( const auto & L1 : data )
  {
    // loop over ingresses for this L1 board
    for ( const auto & In : L1.second )
    {
      // Loop over HPDs in this ingress
      for ( const auto & HPD : In.second.hpdData() )
      {      
        // Valid HPD ID
        if ( !HPD.second.hpdID().isValid() ) { continue; }
        // inhibited HPD ?
        if ( HPD.second.header().inhibit() ) { continue; }
        
        // HPD ID
        const auto hpd = HPD.second.hpdID();
        // Vector of SmartIDs
        const auto & rawIDs = HPD.second.smartIDs();
        // Hardware ID
        const auto hardID = m_richSys->hardwareID(hpd);
        // HPD as a string
        std::ostringstream sHPD; sHPD << hpd;

        // HPD backgrounds
        bool isBkg(false), isHPDQCK(false);

        // Loop over raw RichSmartIDs
        for ( const auto & id : rawIDs )
        {
          // get MC histories for this RichSmartID
          Summaries summaries;
          m_truth->getMcHistories( id, summaries );
          // loop over summaries and see if this HPD has any background
          for ( const auto * S : summaries )
          {
            if ( S->history().isBackground() ) { isBkg = true;    }
            if ( S->history().hpdQuartzCK()  ) { isHPDQCK = true; }
            if ( isBkg && isHPDQCK ) break;
          }
        }

        // Get the reconstructed pixels for this HPD
        auto range = pixelCreator()->range(hpd);
        const auto nHPDs = range.size();

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
                "Pixel Reco Eff : "+sHPD.str(), -0.5, 100.5, 101 );
        if ( isBkg )
          plot1D( pixEff, hid(hpd.rich(),"hpdBkgEff/"+(std::string)hardID),
                  "Pixel Reco Eff : "+sHPD.str(), -0.5, 100.5, 101 );
        if ( isHPDQCK )
          plot1D( pixEff, hid(hpd.rich(),"hpdQckEff/"+(std::string)hardID),
                  "HPD Quartz CK Eff : "+sHPD.str(), -0.5, 100.5, 101 );

        _ri_verbo << hpd << " Raw pixels " << rawIDs.size() << " reco " << nHPDs << endmsg;

      } // HPDs
    } // ingresses
  } // L1 boards

  return StatusCode::SUCCESS;
}

