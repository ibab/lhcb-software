
//-----------------------------------------------------------------------------
/** @file RichPixelPositionMonitor.cpp
 *
 *  Implementation file for algorithm class : RichPixelPositionMonitor
 *
 *  $Id: RichPixelPositionMonitor.cpp,v 1.2 2006-04-12 13:46:28 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#include <fstream>

// local
#include "RichPixelPositionMonitor.h"

// namespace
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichPixelPositionMonitor>          s_factory ;
const        IAlgFactory& RichPixelPositionMonitorFactory = s_factory ;


// Standard constructor, initializes variables
RichPixelPositionMonitor::RichPixelPositionMonitor( const std::string& name,
                                                    ISvcLocator* pSvcLocator)
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth ( NULL ),
    m_mcTool         ( NULL ),
    m_idTool         ( NULL )
{
}

// Destructor
RichPixelPositionMonitor::~RichPixelPositionMonitor() {};

//  Initialize
StatusCode RichPixelPositionMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );
  acquireTool( "RichMCTruthTool", m_mcTool,   0, true );
  acquireTool( "RichSmartIDTool", m_idTool,   0, true );
  
  // initialise variables
  m_xHits.clear();
  m_yHits.clear();
  m_zHits.clear();
  m_hitCount.clear();

  return sc;
}

// Main execution
StatusCode RichPixelPositionMonitor::execute()
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
    
    // global position
    const Gaudi::XYZPoint & gPos = pixel->globalPosition();
    // local position on HP panels
    const Gaudi::XYZPoint & lPos = pixel->localPosition();

    // Which detector ?
    const Rich::DetectorType rich = pixel->detector();
    ++nPixs[rich];
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "  -> Pixel         " << pixel->smartID() << endreq
              << "     global        " << gPos << endreq
              << "     local         " << lPos << endreq
              << "     local Aerogel " << pixel->localPosition(Rich::Aerogel) << endreq
              << "     local C4F10   " << pixel->localPosition(Rich::C4F10) << endreq
              << "     local CF4     " << pixel->localPosition(Rich::CF4) << endreq;
    }
    
    // Position plots
    plot1D( gPos.x(), hid(rich,"obsXglo"), "Observed hits x global",
            xMinPDGlo[rich], xMaxPDGlo[rich] );
    plot1D( gPos.y(), hid(rich,"obsYglo"), "Observed hits y global",
            yMinPDGlo[rich], yMaxPDGlo[rich] );
    plot1D( gPos.z(), hid(rich,"obsZglo"), "Observed hits z global",
            zMinPDGlo[rich], zMaxPDGlo[rich] );
    plot2D( gPos.x(), gPos.y(), hid(rich,"obsXYglo"), "Observed hits yVx global",
            xMinPDGlo[rich], xMaxPDGlo[rich], yMinPDGlo[rich], yMaxPDGlo[rich], 100,100 );
    plot2D( gPos.x(), gPos.z(), hid(rich,"obsXZglo"), "Observed hits zVx global",
            xMinPDGlo[rich], xMaxPDGlo[rich], zMinPDGlo[rich], zMaxPDGlo[rich], 100,100 );
    plot2D( gPos.y(), gPos.z(), hid(rich,"obsYZglo"), "Observed hits yVz global",
            yMinPDGlo[rich], yMaxPDGlo[rich], zMinPDGlo[rich], zMaxPDGlo[rich], 100,100 );
    plot1D( lPos.x(), hid(rich,"obsXloc"), "Observed hits x local",
            xMinPDLoc[rich], xMaxPDLoc[rich] );
    plot1D( lPos.y(), hid(rich,"obsYloc"), "Observed hits y local",
            yMinPDLoc[rich], yMaxPDLoc[rich] );
    plot1D( lPos.z(), hid(rich,"obsZloc"), "Observed hits z local",
            zMinPDLoc[rich], zMaxPDLoc[rich] );
    plot2D( lPos.x(), lPos.y(), hid(rich,"obsXYloc"), "Observed hits yVx local",
            xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich], 100,100 );

    // Background plots
    if ( m_richRecMCTruth->isBackground(pixel) )
    {
      // global position
      plot1D( gPos.x(), hid(rich,"obsXgloB"), "Observed background hits x global",
              xMinPDGlo[rich], xMaxPDGlo[rich] );
      plot1D( gPos.y(), hid(rich,"obsYgloB"), "Observed background hits y global",
              yMinPDGlo[rich], yMaxPDGlo[rich] );
      plot1D( gPos.z(), hid(rich,"obsZgloB"), "Observed background hits z global",
              zMinPDGlo[rich], zMaxPDGlo[rich] );
      plot2D( gPos.x(), gPos.y(), hid(rich,"obsXYgloB"), "Observed background hits yVx global",
              xMinPDGlo[rich], xMaxPDGlo[rich], yMinPDGlo[rich], yMaxPDGlo[rich], 100,100 );
      plot2D( gPos.x(), gPos.z(), hid(rich,"obsXZgloB"), "Observed background hits zVx global",
              xMinPDGlo[rich], xMaxPDGlo[rich], zMinPDGlo[rich], zMaxPDGlo[rich], 100,100 );
      plot2D( gPos.y(), gPos.z(), hid(rich,"obsYZgloB"), "Observed background hits yVz global",
              yMinPDGlo[rich], yMaxPDGlo[rich], zMinPDGlo[rich], zMaxPDGlo[rich], 100,100 );
      // local position on HP panels
      plot1D( lPos.x(), hid(rich,"obsXlocB"), "Observed background hits x local",
              xMinPDLoc[rich], xMaxPDLoc[rich] );
      plot1D( lPos.y(), hid(rich,"obsYlocB"), "Observed background hits y local",
              yMinPDLoc[rich], yMaxPDLoc[rich] );
      plot1D( lPos.z(), hid(rich,"obsZlocB"), "Observed background hits z local",
              zMinPDLoc[rich], zMaxPDLoc[rich] );
      plot2D( lPos.x(), lPos.y(), hid(rich,"obsXYlocB"), "Observed background hits yVx local",
              xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich], 100,100 );
    }

    // background value distribution
    const double logPixBkg =
      ( pixel->currentBackground() > 1e-20 ? log(pixel->currentBackground()) : -20 );
    plot1D( logPixBkg, hid(rich,"pixLogBkg"), "Pixel log(background)",-20,0);

    // find out pd positions
    const RichSmartID pdID = pixel->smartID().hpdID();
    m_xHits[pdID] += gPos.x();
    m_yHits[pdID] += gPos.y();
    m_zHits[pdID] += gPos.z();
    ++m_hitCount[pdID];

    // True radiator hits
    for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad )
    {
      const Rich::RadiatorType rad = (Rich::RadiatorType)iRad;
      const MCParticle * mcP = m_richRecMCTruth->trueCherenkovRadiation(pixel,rad);
      if ( mcP )
      {
        plot2D( lPos.x(), lPos.y(), hid(rad,"signalHits"), "True Cherenkov signal hits",
                xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich], 100,100 );
      }
    }

    // MCHits
    const SmartRefVector<LHCb::MCRichHit> & mcHits = m_richRecMCTruth->mcRichHits( pixel );
    for ( SmartRefVector<MCRichHit>::const_iterator iHit = mcHits.begin();
          iHit != mcHits.end(); ++iHit )
    {
      if ( !(*iHit)->isBackground() )
      {
        // Get MCRichOptical Photon
        const MCRichOpticalPhoton * mcPhot = m_mcTool->mcOpticalPhoton(*iHit);
        // Compare position on HPD entrance window
        const Gaudi::XYZPoint & mcPoint = mcPhot->pdIncidencePoint();
        plot1D( gPos.X()-mcPoint.X(), hid(rich,"pdImpX"),
                "dX global on HPD entry window : CK signal only", -30, 30 );
        plot1D( gPos.Y()-mcPoint.Y(), hid(rich,"pdImpY"),
                "dY global on HPD entry window : CK signal only", -30, 30 );
        plot1D( gPos.Z()-mcPoint.Z(), hid(rich,"pdImpZ"),
                "dZ global on HPD entry window : CK signal only", -30, 30 );
        plot1D( sqrt((gPos-mcPoint).Mag2()), hid(rich,"pdImpR"),
                "dR global on HPD entry window : CK signal only",  0, 10 );
        // MC point in local coordinates
        const Gaudi::XYZPoint mcPointLoc = m_idTool->globalToPDPanel(mcPoint);
        plot1D( lPos.X()-mcPointLoc.X(), hid(rich,"pdImpXloc"),
                "dX local on HPD entry window : CK signal only", -30, 30 );
        plot1D( lPos.Y()-mcPointLoc.Y(), hid(rich,"pdImpYloc"),
                "dY local on HPD entry window : CK signal only", -30, 30 );
        plot1D( lPos.Z()-mcPointLoc.Z(), hid(rich,"pdImpZloc"),
                "dZ local on HPD entry window : CK signal only", -30, 30 );
        plot1D( sqrt((lPos-mcPointLoc).Mag2()), hid(rich,"pdImpRloc"),
                "dR local on HPD entry window : CK signal only",  0, 10 );
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
StatusCode RichPixelPositionMonitor::finalize()
{

  // Histogramming
  PD_GLOBAL_POSITIONS;

  // Final printout of histograms and PD numbering histograms
  debug() << "RichDigit PD Positions :-" << endreq;
  for ( Rich::Map<RichSmartID::KeyType, double>::const_iterator ix = m_xHits.begin();
        ix != m_xHits.end(); ++ix )
  {
    const RichSmartID pdID = ix->first;
    const int hits = m_hitCount[pdID];
    const double avX = ( hits>0 ? ix->second/hits : 0 );
    const double avY = ( hits>0 ? m_yHits[pdID]/hits : 0 );
    const double avZ = ( hits>0 ? m_zHits[pdID]/hits : 0 );
    debug() << "PD " << pdID << " Pos. " << Gaudi::XYZPoint(avX,avY,avZ)
            << " Hits " << hits << endreq;
    const Rich::DetectorType rich = ( avZ>5000 ? Rich::Rich2 : Rich::Rich1 );

    plot2D( avX, avY, "hpdNumbers/pdColXY", "HPD column numbers yVx",
            xMinPDGlo[rich], xMaxPDGlo[rich], yMinPDGlo[rich], yMaxPDGlo[rich],
            50, 50, pdID.hpdCol() );
    plot2D( avZ, avX, "hpdNumbers/pdColZX", "HPD column numbers xVz",
            zMinPDGlo[rich], zMaxPDGlo[rich], xMinPDGlo[rich], xMaxPDGlo[rich],
            50, 50, pdID.hpdCol() );
    plot2D( avZ, avY, "hpdNumbers/pdColZY", "HPD column numbers yVz",
            zMinPDGlo[rich], zMaxPDGlo[rich], yMinPDGlo[rich], yMaxPDGlo[rich],
            50, 50, pdID.hpdCol() );

    plot2D( avX, avY, "hpdNumbers/pdRowXY", "HPD row numbers yVx",
            xMinPDGlo[rich], xMaxPDGlo[rich], yMinPDGlo[rich], yMaxPDGlo[rich],
            50, 50, pdID.hpdNumInCol() );
    plot2D( avZ, avX, "hpdNumbers/pdRowZX", "HPD row numbers xVz",
            zMinPDGlo[rich], zMaxPDGlo[rich], xMinPDGlo[rich], xMaxPDGlo[rich],
            50, 50, pdID.hpdNumInCol() );
    plot2D( avZ, avY, "hpdNumbers/pdRowZY", "HPD row numbers yVz",
            zMinPDGlo[rich], zMaxPDGlo[rich], yMinPDGlo[rich], yMaxPDGlo[rich],
            50, 50, pdID.hpdNumInCol() );
  }

  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
