
//-----------------------------------------------------------------------------
/** @file RichPixelMonitor.cpp
 *
 *  Implementation file for algorithm class : RichPixelMonitor
 *
 *  $Id: RichPixelMonitor.cpp,v 1.2 2005-10-31 13:30:58 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#include <fstream>

// local
#include "RichPixelMonitor.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichPixelMonitor>          s_factory ;
const        IAlgFactory& RichPixelMonitorFactory = s_factory ;


// Standard constructor, initializes variables
RichPixelMonitor::RichPixelMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : RichRecHistoAlgBase ( name, pSvcLocator )
{
}

// Destructor
RichPixelMonitor::~RichPixelMonitor() {};

//  Initialize
StatusCode RichPixelMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );

  // initialise variables
  m_xHits.clear();
  m_yHits.clear();
  m_zHits.clear();
  m_hitCount.clear();

  return sc;
}

// Main execution
StatusCode RichPixelMonitor::execute()
{
  debug() << "Execute" << endreq;

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all pixels have been formed
  if ( richPixels()->empty() )
  {
    if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;
    debug() << "No Pixels found : Created "
            << richPixels()->size() << " RichRecPixels" << endreq;
  }

  // Histogramming
  const RichHistoID hid;
  PD_GLOBAL_POSITIONS;
  PD_LOCAL_POSITIONS;

  // Iterate over pixels
  debug() << "All Pixels " << richPixels()->size() << " :-" << endreq;
  std::vector<unsigned int> nPixs( Rich::NRiches, 0 );
  for ( RichRecPixels::const_iterator iPix = richPixels()->begin();
        iPix != richPixels()->end();
        ++iPix )
  {
    const RichRecPixel * pixel = *iPix;

    // Which detector ?
    const Rich::DetectorType iRich = pixel->detector();
    ++nPixs[iRich];
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "  -> Pixel         " << pixel->smartID() << endreq
              << "     global        " << pixel->globalPosition() << endreq
              << "     local         " << pixel->localPosition() << endreq
              << "     local Aerogel " << pixel->localPosition(Rich::Aerogel) << endreq
              << "     local C4F10   " << pixel->localPosition(Rich::C4F10) << endreq
              << "     local CF4     " << pixel->localPosition(Rich::CF4) << endreq
        ;
    }

    // global position
    const HepPoint3D & gPos = pixel->globalPosition();
    plot1D( gPos.x(), hid(iRich,"obsXglo"), "Observed hits x global",
            xMinPDGlo[iRich], xMaxPDGlo[iRich] );
    plot1D( gPos.y(), hid(iRich,"obsYglo"), "Observed hits y global",
            yMinPDGlo[iRich], yMaxPDGlo[iRich] );
    plot1D( gPos.z(), hid(iRich,"obsZglo"), "Observed hits z global",
            zMinPDGlo[iRich], zMaxPDGlo[iRich] );
    plot2D( gPos.x(), gPos.y(), hid(iRich,"obsXYglo"), "Observed hits yVx global",
            xMinPDGlo[iRich], xMaxPDGlo[iRich], yMinPDGlo[iRich], yMaxPDGlo[iRich] );
    plot2D( gPos.x(), gPos.z(), hid(iRich,"obsXZglo"), "Observed hits zVx global",
            xMinPDGlo[iRich], xMaxPDGlo[iRich], zMinPDGlo[iRich], zMaxPDGlo[iRich] );
    plot2D( gPos.y(), gPos.z(), hid(iRich,"obsYZglo"), "Observed hits yVz global",
            yMinPDGlo[iRich], yMaxPDGlo[iRich], zMinPDGlo[iRich], zMaxPDGlo[iRich] );
    // local position on HP panels
    const HepPoint3D & lPos = pixel->localPosition();
    plot1D( lPos.x(), hid(iRich,"obsXloc"), "Observed hits x local",
            xMinPDLoc[iRich], xMaxPDLoc[iRich] );
    plot1D( lPos.y(), hid(iRich,"obsYloc"), "Observed hits y local",
            yMinPDLoc[iRich], yMaxPDLoc[iRich] );
    plot1D( lPos.z(), hid(iRich,"obsZloc"), "Observed hits z local",
            zMinPDLoc[iRich], zMaxPDLoc[iRich] );
    plot2D( lPos.x(), lPos.y(), hid(iRich,"obsXYloc"), "Observed hits yVx local",
            xMinPDLoc[iRich],xMaxPDLoc[iRich],yMinPDLoc[iRich],yMaxPDLoc[iRich] );

    // Background plots
    if ( m_richRecMCTruth->isBackground(pixel) )
    {
      // global position
      plot1D( gPos.x(), hid(iRich,"obsXgloB"), "Observed background hits x global",
              xMinPDGlo[iRich], xMaxPDGlo[iRich] );
      plot1D( gPos.y(), hid(iRich,"obsYgloB"), "Observed background hits y global",
              yMinPDGlo[iRich], yMaxPDGlo[iRich] );
      plot1D( gPos.z(), hid(iRich,"obsZgloB"), "Observed background hits z global",
              zMinPDGlo[iRich], zMaxPDGlo[iRich] );
      plot2D( gPos.x(), gPos.y(), hid(iRich,"obsXYgloB"), "Observed background hits yVx global",
              xMinPDGlo[iRich], xMaxPDGlo[iRich], yMinPDGlo[iRich], yMaxPDGlo[iRich] );
      plot2D( gPos.x(), gPos.z(), hid(iRich,"obsXZgloB"), "Observed background hits zVx global",
              xMinPDGlo[iRich], xMaxPDGlo[iRich], zMinPDGlo[iRich], zMaxPDGlo[iRich] );
      plot2D( gPos.y(), gPos.z(), hid(iRich,"obsYZgloB"), "Observed background hits yVz global",
              yMinPDGlo[iRich], yMaxPDGlo[iRich], zMinPDGlo[iRich], zMaxPDGlo[iRich] );
      // local position on HP panels
      plot1D( lPos.x(), hid(iRich,"obsXlocB"), "Observed background hits x local",
              xMinPDLoc[iRich], xMaxPDLoc[iRich] );
      plot1D( lPos.y(), hid(iRich,"obsYlocB"), "Observed background hits y local",
              yMinPDLoc[iRich], yMaxPDLoc[iRich] );
      plot1D( lPos.z(), hid(iRich,"obsZlocB"), "Observed background hits z local",
              zMinPDLoc[iRich], zMaxPDLoc[iRich] );
      plot2D( lPos.x(), lPos.y(), hid(iRich,"obsXYlocB"), "Observed background hits yVx local",
              xMinPDLoc[iRich],xMaxPDLoc[iRich],yMinPDLoc[iRich],yMaxPDLoc[iRich] );
    }

    // background value distribution
    const double logPixBkg =
      ( pixel->currentBackground() > 1e-20 ? log(pixel->currentBackground()) : -20 );
    plot1D( logPixBkg, hid(iRich,"pixLogBkg"), "Pixel log(background)",-20,0);

    // find out pd positions
    const RichSmartID pdID = pixel->smartID().pdID();
    m_xHits[pdID] += pixel->globalPosition().x();
    m_yHits[pdID] += pixel->globalPosition().y();
    m_zHits[pdID] += pixel->globalPosition().z();
    ++m_hitCount[pdID];

    // True radiator hits
    for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad )
    {
      const Rich::RadiatorType rad = (Rich::RadiatorType)iRad;
      const MCParticle * mcP = m_richRecMCTruth->trueCherenkovRadiation(pixel,rad);
      if ( mcP )
      {
        plot2D( lPos.x(), lPos.y(), hid(rad,"signalHits"), "True Cherenkov signal hits",
                xMinPDLoc[iRich],xMaxPDLoc[iRich],yMinPDLoc[iRich],yMaxPDLoc[iRich] );
      }
    }

  } // loop over all pixels

  debug() << "RICH1 Pixels :-" << endreq;
  unsigned int nR1(0);
  for ( RichRecPixels::const_iterator iPix = pixelCreator()->begin(Rich::Rich1);
        iPix != pixelCreator()->end(Rich::Rich1); ++iPix )
  {
    ++nR1;
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "  -> Pixel " << (*iPix)->smartID() << endreq;
    }
  }
  if ( nR1 != nPixs[Rich::Rich1] )
    Error("Mis-match in RICH1 pixels between full container and RICH1 iterators");

  debug() << "RICH2 Pixels :-" << endreq;
  unsigned int nR2(0);
  for ( RichRecPixels::const_iterator iPix = pixelCreator()->begin(Rich::Rich2);
        iPix != pixelCreator()->end(Rich::Rich2); ++iPix )
  {
    ++nR2;
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "  -> Pixel " << (*iPix)->smartID() << endreq;
    }
  }
  if ( nR2 != nPixs[Rich::Rich2] )
    Error("Mis-match in RICH2 pixels between full container and RICH2 iterators");

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichPixelMonitor::finalize()
{

  // Histogramming
  PD_GLOBAL_POSITIONS;

  // Final printout of histograms and PD numbering histograms
  debug() << "RichDigit PD Positions :-" << endreq;
  for ( RichMap<RichSmartID::KeyType, double>::const_iterator ix = m_xHits.begin();
        ix != m_xHits.end(); ++ix )
  {
    const RichSmartID pdID = ix->first;
    const int hits = m_hitCount[pdID];
    const double avX = ( hits>0 ? ix->second/hits : 0 );
    const double avY = ( hits>0 ? m_yHits[pdID]/hits : 0 );
    const double avZ = ( hits>0 ? m_zHits[pdID]/hits : 0 );
    debug() << "PD " << pdID << " Pos. " << HepPoint3D(avX,avY,avZ)
            << " Hits " << hits << endreq;
    const Rich::DetectorType iRich = ( avZ>5000 ? Rich::Rich2 : Rich::Rich1 );

    plot2D( avX, avY, "hpdNumbers/pdColXY", "HPD column numbers yVx",
            xMinPDGlo[iRich], xMaxPDGlo[iRich], yMinPDGlo[iRich], yMaxPDGlo[iRich],
            50, 50, pdID.pdCol() );
    plot2D( avZ, avX, "hpdNumbers/pdColZX", "HPD column numbers xVz",
            zMinPDGlo[iRich], zMaxPDGlo[iRich], xMinPDGlo[iRich], xMaxPDGlo[iRich],
            50, 50, pdID.pdCol() );
    plot2D( avZ, avY, "hpdNumbers/pdColZY", "HPD column numbers yVz",
            zMinPDGlo[iRich], zMaxPDGlo[iRich], yMinPDGlo[iRich], yMaxPDGlo[iRich],
            50, 50, pdID.pdCol() );

    plot2D( avX, avY, "hpdNumbers/pdRowXY", "HPD row numbers yVx",
            xMinPDGlo[iRich], xMaxPDGlo[iRich], yMinPDGlo[iRich], yMaxPDGlo[iRich],
            50, 50, pdID.pdRow() );
    plot2D( avZ, avX, "hpdNumbers/pdRowZX", "HPD row numbers xVz",
            zMinPDGlo[iRich], zMaxPDGlo[iRich], xMinPDGlo[iRich], xMaxPDGlo[iRich],
            50, 50, pdID.pdRow() );
    plot2D( avZ, avY, "hpdNumbers/pdRowZY", "HPD row numbers yVz",
            zMinPDGlo[iRich], zMaxPDGlo[iRich], yMinPDGlo[iRich], yMaxPDGlo[iRich],
            50, 50, pdID.pdRow() );

  }

  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
