
//-----------------------------------------------------------------------------
/** @file RichPixelRecoEffMonitor.cpp
 *
 *  Implementation file for algorithm class : RichPixelRecoEffMonitor
 *
 *  $Id: RichPixelRecoEffMonitor.cpp,v 1.2 2006-12-01 16:34:07 cattanem Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#include <fstream>

// local
#include "RichPixelRecoEffMonitor.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// namespace
using namespace LHCb;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RichPixelRecoEffMonitor );

// Standard constructor, initializes variables
RichPixelRecoEffMonitor::RichPixelRecoEffMonitor( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( 0 ),
    m_truth             ( 0 ),
    m_richSys           ( 0 )
{
}

// Destructor
RichPixelRecoEffMonitor::~RichPixelRecoEffMonitor() {};

//  Initialize
StatusCode RichPixelRecoEffMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );
  acquireTool( "RichMCTruthTool",    m_truth );
  acquireTool( "RichSmartIDDecoder", m_decoder, 0, true );

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocation::RichSystem );

  // Force creation of tools some here, to avoid it happening during first
  // event and perturbing timing numbers
  pixelCreator();

  return sc;
}

// Main execution
StatusCode RichPixelRecoEffMonitor::execute()
{
  debug() << "Execute" << endreq;

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all pixels have been formed
  if ( richPixels()->empty() )
  {
    if ( pixelCreator()->newPixels().isFailure() )
      return Error( "Problem creating RichRecPixels" );
    debug() << "No Pixels found : Created "
            << richPixels()->size() << " RichRecPixels" << endreq;
  }

  // Histogramming
  const RichHistoID hid;
  //PD_GLOBAL_POSITIONS;
  //PD_LOCAL_POSITIONS;

  typedef std::vector<const LHCb::MCRichDigitSummary*> Summaries;

  // Obtain RichSmartIDs from raw decoding
  const RichDAQ::PDMap & smartIDs = m_decoder->allRichSmartIDs();

  // Loop over HPDs and RichSmartIDs
  for ( RichDAQ::PDMap::const_iterator iHPD = smartIDs.begin();
        iHPD != smartIDs.end(); ++iHPD )
  {
    // HPD ID
    const RichSmartID hpd = (*iHPD).first;
    // Vector of SmartIDs
    const RichSmartID::Vector & rawIDs = (*iHPD).second;
    // Hardware ID
    const RichDAQ::HPDHardwareID hardID = m_richSys->hardwareID(hpd);
    // HPD as a string
    std::ostringstream HPD; HPD << hpd; 

    // HPD backgrounds
    bool isBkg(false), isHPDQCK(false);

    // Loop over raw RichSmartIDs
    for ( RichSmartID::Vector::const_iterator iR = rawIDs.begin();
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
    RichRecPixels::const_iterator iPixel = pixelCreator()->begin ( hpd );
    RichRecPixels::const_iterator endPix = pixelCreator()->end   ( hpd );
    // loop over reconstructed pixels
    unsigned int nHPDs(0);
    for ( ; iPixel != endPix; ++iPixel )
    {
      ++nHPDs;
    }

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
      verbose() << hpd << " Raw pixels " << rawIDs.size() << " reco " << nHPDs << endreq;
    }

  }


  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichPixelRecoEffMonitor::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
