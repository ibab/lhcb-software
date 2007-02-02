
//-----------------------------------------------------------------------------
/** @file RichPixelPositionMonitor.cpp
 *
 *  Implementation file for algorithm class : RichPixelPositionMonitor
 *
 *  $Id: RichPixelPositionMonitor.cpp,v 1.9 2007-02-02 10:07:12 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#include <fstream>

// local
#include "RichPixelPositionMonitor.h"

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PixelPositionMonitor );

// Standard constructor, initializes variables
PixelPositionMonitor::PixelPositionMonitor( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth ( NULL ),
    m_mcTool         ( NULL ),
    m_idTool         ( NULL ),
    m_richSys        ( NULL ) { }

// Destructor
PixelPositionMonitor::~PixelPositionMonitor() { }

//  Initialize
StatusCode PixelPositionMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );
  acquireTool( "RichMCTruthTool", m_mcTool,   0, true );
  acquireTool( "RichSmartIDTool", m_idTool,   0, true );

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocation::RichSystem );

  return sc;
}

// Main execution
StatusCode PixelPositionMonitor::execute()
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

  DAQ::PDMap pdMap;

  // Iterate over pixels
  debug() << "All Pixels " << richPixels()->size() << " :-" << endreq;
  std::vector<unsigned int> nPixs( Rich::NRiches, 0 );
  for ( LHCb::RichRecPixels::const_iterator iPix = richPixels()->begin();
        iPix != richPixels()->end();
        ++iPix )
  {
    const LHCb::RichRecPixel * pixel = *iPix;

    // global position
    const Gaudi::XYZPoint & gPos = pixel->globalPosition();
    // local position on HP panels
    const Gaudi::XYZPoint & lPos = pixel->localPosition();

    // Which detector ?
    const Rich::DetectorType rich = pixel->detector();
    ++nPixs[rich];

    // HPD ID
    const LHCb::RichSmartID pdID = pixel->smartID().hpdID();

    // Centre point of HPD
    const Gaudi::XYZPoint hpdGlo = m_idTool->hpdPosition(pdID);
    const Gaudi::XYZPoint hpdLoc = m_idTool->globalToPDPanel(hpdGlo);

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "  -> Pixel            " << pixel->smartID() << endreq
                << "     global           " << gPos << endreq
                << "     local            " << lPos << endreq;
      if ( rich == Rich::Rich1)
      {
        verbose() << "     local Aerogel    " << pixel->localPosition(Rich::Aerogel) << endreq
                  << "     local Rich1Gas   " << pixel->localPosition(Rich::Rich1Gas) << endreq;
      }
      else
      {
        verbose() << "     local Rich2Gas   " << pixel->localPosition(Rich::Rich2Gas) << endreq;
      }
      verbose() << "     HPD centre       " << hpdGlo << endreq
                << "     local HPD centre " << hpdLoc << endreq;
    }

    // map of hits in each HPD
    pdMap[pixel->smartID().hpdID()].push_back( pixel->smartID() );

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

    // global - local correlations
    profile1D( lPos.x(), gPos.x(), hid(rich,"gloLocCorrX"), "Global X versus Local X",
               xMinPDLoc[rich], xMaxPDLoc[rich] );
    profile1D( lPos.y(), gPos.y(), hid(rich,"gloLocCorrY"), "Global Y versus Local Y",
               yMinPDLoc[rich], yMaxPDLoc[rich] );

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

    // True radiator hits
    for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad )
    {
      const Rich::RadiatorType rad = (Rich::RadiatorType)iRad;
      const LHCb::MCParticle * mcP = m_richRecMCTruth->trueCherenkovRadiation(pixel,rad);
      if ( mcP )
      {
        plot2D( lPos.x(), lPos.y(), hid(rad,"signalHits"), "True Cherenkov signal hits",
                xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich], 100,100 );
      }
    }

    // MCHits
    const SmartRefVector<LHCb::MCRichHit> & mcHits = m_richRecMCTruth->mcRichHits( pixel );
    for ( SmartRefVector<LHCb::MCRichHit>::const_iterator iHit = mcHits.begin();
          iHit != mcHits.end(); ++iHit )
    {
      if ( !(*iHit)->isBackground() )
      {
        // Get MCRichOptical Photon
        const LHCb::MCRichOpticalPhoton * mcPhot = m_mcTool->mcOpticalPhoton(*iHit);
        if ( mcPhot )
        {
          // Compare position on HPD entrance window
          const Gaudi::XYZPoint & mcPoint = mcPhot->pdIncidencePoint();
          plot1D( gPos.X()-mcPoint.X(), hid(rich,"pdImpX"),
                  "dX global on HPD entry window : CK signal only", -30, 30 );
          plot1D( gPos.Y()-mcPoint.Y(), hid(rich,"pdImpY"),
                  "dY global on HPD entry window : CK signal only", -30, 30 );
          plot1D( gPos.Z()-mcPoint.Z(), hid(rich,"pdImpZ"),
                  "dZ global on HPD entry window : CK signal only", -30, 30 );
          plot1D( sqrt((gPos-mcPoint).Mag2()), hid(rich,"pdImpR"),
                  "dR global on HPD entry window : CK signal only",  0,  10 );
          // MC point in local coordinates
          const Gaudi::XYZPoint mcPointLoc = m_idTool->globalToPDPanel(mcPoint);
          plot1D( lPos.X()-mcPointLoc.X(), hid(rich,"pdImpXloc"),
                  "dX local on HPD entry window : CK signal only", -30, 30 );
          plot1D( lPos.Y()-mcPointLoc.Y(), hid(rich,"pdImpYloc"),
                  "dY local on HPD entry window : CK signal only", -30, 30 );
          plot1D( lPos.Z()-mcPointLoc.Z(), hid(rich,"pdImpZloc"),
                  "dZ local on HPD entry window : CK signal only", -30, 30 );
          plot1D( sqrt((lPos-mcPointLoc).Mag2()), hid(rich,"pdImpRloc"),
                  "dR local on HPD entry window : CK signal only",  0,  10 );
          // differences as a function of position on HPD
          plot2D( lPos.X()-hpdLoc.X(), lPos.X()-mcPointLoc.X(), hid(rich,"pdImpXlocVX"),
                  "dX V X local on HPD entry window : CK signal only", -35, 35, -8, 8 );
          plot2D( lPos.Y()-hpdLoc.Y(), lPos.Y()-mcPointLoc.Y(), hid(rich,"pdImpYlocVY"),
                  "dY V Y local on HPD entry window : CK signal only", -35, 35, -8, 8 );
          const double R   = sqrt( gsl_pow_2(lPos.X()-hpdLoc.X()) +
                                   gsl_pow_2(lPos.Y()-hpdLoc.Y()) );
          const double McR = sqrt( gsl_pow_2(mcPointLoc.X()-hpdLoc.X()) +
                                   gsl_pow_2(mcPointLoc.Y()-hpdLoc.Y()) );
          plot2D( R, R-McR, hid(rich,"pdImpRlocVR"),
                  "dR V R local on HPD entry window : CK signal only", 0, 40, -10, 10 );
          if ( msgLevel(MSG::VERBOSE) )
          {
            verbose() << "Global : Pixel = " << (*iHit)->entry()
                      << " : MC HPD window = " << mcPoint << endreq
                      << "Local  : Pixel = " << m_idTool->globalToPDPanel((*iHit)->entry())
                      << " : MC HPD window = " << mcPointLoc << endreq;
          }
        }
      }
    }

  } // loop over all pixels

  debug() << "RICH1 Pixels :-" << endreq;
  unsigned int nR1(0);
  for ( LHCb::RichRecPixels::const_iterator iPix = pixelCreator()->begin(Rich::Rich1);
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
  for ( LHCb::RichRecPixels::const_iterator iPix = pixelCreator()->begin(Rich::Rich2);
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

  const double maxAvO[] = { 1500*Gaudi::Units::mm, 2000*Gaudi::Units::mm};

  // HPD plots
  for ( DAQ::PDMap::const_iterator iPD = pdMap.begin();
        iPD != pdMap.end(); ++iPD )
  {
    // HPD
    const LHCb::RichSmartID hpdID = (*iPD).first;
    const DAQ::HPDHardwareID hardID = m_richSys->hardwareID(hpdID);
    const DAQ::Level0ID l0ID        = m_richSys->level0ID(hpdID);
    // Centre point of HPD in local coords
    const Gaudi::XYZPoint hpdGlo = m_idTool->hpdPosition(hpdID);
    const Gaudi::XYZPoint hpdLoc = m_idTool->globalToPDPanel(hpdGlo);
    // create histo title
    std::ostringstream HPD;
    HPD << hpdID << " L0ID=" << l0ID << " hardID=" << hardID << " pos=" << hpdGlo;
    // histo ID
    const std::string Hid = "hpds/"+(std::string)hardID ;
    for ( LHCb::RichSmartID::Vector::const_iterator iS = (*iPD).second.begin();
          iS != (*iPD).second.end(); ++iS )
    {
      // hit point on silicon in local coords
      const Gaudi::XYZVector hitP
        = m_idTool->globalToPDPanel( m_idTool->globalPosition(*iS) ) - hpdLoc;
      verbose() << "Hit " << (*iS) << " " << hitP << endreq;
      plot2D( hitP.X(), hitP.Y(), Hid, HPD.str(), -40*Gaudi::Units::mm,
              40*Gaudi::Units::mm, -40*Gaudi::Units::mm, 40*Gaudi::Units::mm, 32, 32 );
    }
    // Profile plot of average HPD occupancy versus radial (local) distance
    const double localRadial = sqrt( hpdLoc.x()*hpdLoc.x() + hpdLoc.y()*hpdLoc.y() );
    const Rich::DetectorType rich = hpdID.rich();
    profile1D( localRadial, (*iPD).second.size(), hid(rich,"AvHPDOccVD"),
               "Average HPD occupancy versus radial distance", 0,maxAvO[rich] , 50 );
    profile2D( hpdLoc.x(), hpdLoc.y(), (*iPD).second.size(), hid(rich,"AvHPDOccVXY"),
               "Average HPD occupancy versus X,Y", 
               xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich], 100, 100 );
  }

  return StatusCode::SUCCESS;
}

