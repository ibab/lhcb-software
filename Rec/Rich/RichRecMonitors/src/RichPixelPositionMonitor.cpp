
//-----------------------------------------------------------------------------
/** @file RichPixelPositionMonitor.cpp
 *
 *  Implementation file for algorithm class : RichPixelPositionMonitor
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#include <sstream>

// local
#include "RichPixelPositionMonitor.h"

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PixelPositionMonitor )

// Standard constructor, initializes variables
  PixelPositionMonitor::PixelPositionMonitor( const std::string& name,
                                              ISvcLocator* pSvcLocator)
    : HistoAlgBase     ( name, pSvcLocator ),
      m_richRecMCTruth ( NULL ),
      m_mcTool         ( NULL ),
      m_idTool         ( NULL ),
      m_richSys        ( NULL )
{
  declareProperty( "N2DBins", m_n2DBins = 500 );
}

// Destructor
PixelPositionMonitor::~PixelPositionMonitor() { }

//  Initialize
StatusCode PixelPositionMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );
  acquireTool( "RichMCTruthTool", m_mcTool,   0, true );
  acquireTool( "RichSmartIDTool", m_idTool,   0, true );

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  return sc;
}

// Main execution
StatusCode PixelPositionMonitor::execute()
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
  PD_GLOBAL_POSITIONS;
  PD_LOCAL_POSITIONS;

  DAQ::PDMap pdMap;

  // Iterate over pixels
  debug() << "All Pixels " << richPixels()->size() << " :-" << endmsg;
  std::vector<unsigned int> nPixs( Rich::NRiches, 0 );
  for ( LHCb::RichRecPixels::const_iterator iPix = richPixels()->begin();
        iPix != richPixels()->end();
        ++iPix )
  {
    const LHCb::RichRecPixel * pixel = *iPix;

    // global position
    const Gaudi::XYZPoint & gPos = pixel->globalPosition();
    // local position on HPD panels
    const Gaudi::XYZPoint & lPos = pixel->localPosition();
    // position on anode in global coords
    //Gaudi::XYZPoint anodeGlobal;
    //StatusCode sc = m_idTool->anodeGlobalPosition( pixel->hpdPixelCluster(), anodeGlobal );
    //if ( sc.isFailure() ) { Warning( "Problem with getting anode global position" ); continue; }
    // position on anode in local coords
    //const Gaudi::XYZPoint anodeLocal = m_idTool->globalToPDPanel(anodeGlobal);

    // Which detector ?
    const Rich::DetectorType rich = pixel->detector();
    ++nPixs[rich];

    // HPD ID
    const LHCb::RichSmartID pdID = pixel->hpdPixelCluster().hpd();

    // Which panel
    const Rich::Side side = pdID.panel();

    // Centre point of HPD
    Gaudi::XYZPoint hpdGlo;
    StatusCode sc = m_idTool->pdPosition(pdID,hpdGlo);
    if ( sc.isFailure() ) { Warning( "Problem with get HPD position" ); continue; }
    const Gaudi::XYZPoint hpdLoc = m_idTool->globalToPDPanel(hpdGlo);

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "  -> Pixel            " << pixel->hpdPixelCluster() << endmsg
                << "     global           " << gPos << endmsg
                << "     local            " << lPos << endmsg;
      if ( rich == Rich::Rich1)
      {
        verbose() << "     local Aerogel    " << pixel->radCorrLocalPositions().position(Rich::Aerogel) << endmsg
                  << "     local Rich1Gas   " << pixel->radCorrLocalPositions().position(Rich::Rich1Gas) << endmsg;
      }
      else
      {
        verbose() << "     local Rich2Gas   " << pixel->radCorrLocalPositions().position(Rich::Rich2Gas) << endmsg;
      }
      verbose() << "     HPD centre       " << hpdGlo << endmsg
                << "     local HPD centre " << hpdLoc << endmsg;
    }

    // map of hits in each HPD
    for ( auto sID : pixel->hpdPixelCluster().smartIDs() )
    {
      pdMap[pdID].push_back( sID );
    }

    // Position plots
    plot1D( gPos.x(), hid(rich,"obsXglo"), "Observed hits x global",
            xMinPDGlo[rich], xMaxPDGlo[rich] );
    plot1D( gPos.y(), hid(rich,"obsYglo"), "Observed hits y global",
            yMinPDGlo[rich], yMaxPDGlo[rich] );
    plot1D( gPos.z(), hid(rich,"obsZglo"), "Observed hits z global",
            zMinPDGlo[rich], zMaxPDGlo[rich] );
    plot2D( gPos.x(), gPos.y(), hid(rich,"obsXYglo"), "Observed hits yVx global",
            xMinPDGlo[rich], xMaxPDGlo[rich], yMinPDGlo[rich], yMaxPDGlo[rich], m_n2DBins,m_n2DBins );
    plot2D( gPos.x(), gPos.z(), hid(rich,"obsXZglo"), "Observed hits zVx global",
            xMinPDGlo[rich], xMaxPDGlo[rich], zMinPDGlo[rich], zMaxPDGlo[rich], m_n2DBins,m_n2DBins );
    plot2D( gPos.y(), gPos.z(), hid(rich,"obsYZglo"), "Observed hits yVz global",
            yMinPDGlo[rich], yMaxPDGlo[rich], zMinPDGlo[rich], zMaxPDGlo[rich], m_n2DBins,m_n2DBins );
    plot1D( lPos.x(), hid(rich,"obsXloc"), "Observed hits x local",
            xMinPDLoc[rich], xMaxPDLoc[rich] );
    plot1D( lPos.y(), hid(rich,"obsYloc"), "Observed hits y local",
            yMinPDLoc[rich], yMaxPDLoc[rich] );
    plot1D( lPos.z(), hid(rich,"obsZloc"), "Observed hits z local",
            zMinPDLoc[rich], zMaxPDLoc[rich] );
    plot2D( lPos.x(), lPos.y(), hid(rich,"obsXYloc"), "Observed hits yVx local",
            xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich], m_n2DBins,m_n2DBins );

    //plot2D( anodeLocal.x(), anodeLocal.y(), hid(rich,"obsXYAnodeloc"), "Observed hits yVx anode local",
    //        xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich], m_n2DBins,m_n2DBins );

    // global - local correlations
    profile1D( lPos.x(), gPos.x(), hid(rich,"gloLocCorrX"), "Global X versus Local X",
               xMinPDLoc[rich], xMaxPDLoc[rich] );
    profile1D( lPos.y(), gPos.y(), hid(rich,"gloLocCorrY"), "Global Y versus Local Y",
               yMinPDLoc[rich], yMaxPDLoc[rich] );

    // Pixel plots using new scheme
    const Rich::SmartIDGlobalOrdering order( pixel->hpdPixelCluster().primaryID() );
    // Make some plots for each RICH detector
    plot2D( order.globalPixelX(), order.globalPixelY(),
            hid(rich,"pixHitMap"), "Pixel Hit Map",
            order.minGlobalPixelX()-0.5,  order.maxGlobalPixelX()+0.5,
            order.minGlobalPixelY()-0.5,  order.maxGlobalPixelY()+0.5,
            order.totalNumInGlobalX()+1,  order.totalNumInGlobalY()+1 );
    plot2D( order.globalPixelX(),
            lPos.X(),
            //gPos.X(),
            hid(rich,side,"gPixXVlocalX"), "local X versus 'global' pixel X",
            order.minGlobalPixelX()-0.5,  order.maxGlobalPixelX()+0.5,
            xMinPDLoc[rich], xMaxPDLoc[rich],
            order.totalNumInGlobalX()+1, m_n2DBins );
    plot2D( order.globalPixelY(),
            lPos.Y(),
            //gPos.Y(),
            hid(rich,side,"gPixYVlocalY"), "local Y versus 'global' pixel Y",
            order.minGlobalPixelY()-0.5,  order.maxGlobalPixelY()+0.5,
            yMinPDLoc[rich], yMaxPDLoc[rich],
            order.totalNumInGlobalY()+1, m_n2DBins );

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
              xMinPDGlo[rich], xMaxPDGlo[rich], yMinPDGlo[rich], yMaxPDGlo[rich], m_n2DBins,m_n2DBins );
      plot2D( gPos.x(), gPos.z(), hid(rich,"obsXZgloB"), "Observed background hits zVx global",
              xMinPDGlo[rich], xMaxPDGlo[rich], zMinPDGlo[rich], zMaxPDGlo[rich], m_n2DBins,m_n2DBins );
      plot2D( gPos.y(), gPos.z(), hid(rich,"obsYZgloB"), "Observed background hits yVz global",
              yMinPDGlo[rich], yMaxPDGlo[rich], zMinPDGlo[rich], zMaxPDGlo[rich], m_n2DBins,m_n2DBins );
      // local position on HP panels
      plot1D( lPos.x(), hid(rich,"obsXlocB"), "Observed background hits x local",
              xMinPDLoc[rich], xMaxPDLoc[rich] );
      plot1D( lPos.y(), hid(rich,"obsYlocB"), "Observed background hits y local",
              yMinPDLoc[rich], yMaxPDLoc[rich] );
      plot1D( lPos.z(), hid(rich,"obsZlocB"), "Observed background hits z local",
              zMinPDLoc[rich], zMaxPDLoc[rich] );
      plot2D( lPos.x(), lPos.y(), hid(rich,"obsXYlocB"), "Observed background hits yVx local",
              xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich], m_n2DBins,m_n2DBins );
    }

    // background value distribution
    const double logPixBkg =
      ( pixel->currentBackground() > 1e-20 ? log10(pixel->currentBackground()) : -20 );
    plot1D( logPixBkg, hid(rich,"pixLogBkg"), "Pixel log(background)",-20,0);

    // True radiator hits
    for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad )
    {
      const Rich::RadiatorType rad = (Rich::RadiatorType)iRad;
      const LHCb::MCParticle * mcP = m_richRecMCTruth->trueCherenkovRadiation(pixel,rad);
      if ( mcP )
      {
        plot2D( lPos.x(), lPos.y(), hid(rad,"signalHits"), "True Cherenkov signal hits",
                xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich], m_n2DBins,m_n2DBins );
      }
    }

    // MCHits
    SmartRefVector<LHCb::MCRichHit> mcHits;
    m_richRecMCTruth->mcRichHits( pixel, mcHits );
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
          plot2D( mcPoint.x(), mcPoint.y(),
                  hid(rich,"mcRichHitGloXvY"), "MCRichHits : Global coordinates",
                  xMinPDGlo[rich], xMaxPDGlo[rich], yMinPDGlo[rich], yMaxPDGlo[rich], m_n2DBins,m_n2DBins );
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
          plot2D( mcPointLoc.x(), mcPointLoc.y(),
                  hid(rich,"mcRichHitLocXvY"), "MCRichHits : Local coordinates",
                  xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich], m_n2DBins,m_n2DBins );
          plot1D( lPos.X()-mcPointLoc.X(), hid(rich,"pdImpXloc"),
                  "dX local on HPD entry window : CK signal only", -30, 30 );
          plot1D( lPos.Y()-mcPointLoc.Y(), hid(rich,"pdImpYloc"),
                  "dY local on HPD entry window : CK signal only", -30, 30 );
          plot1D( lPos.Z()-mcPointLoc.Z(), hid(rich,"pdImpZloc"),
                  "dZ local on HPD entry window : CK signal only", -30, 30 );
          plot1D( std::sqrt((lPos-mcPointLoc).Mag2()), hid(rich,"pdImpRloc"),
                  "dR local on HPD entry window : CK signal only",  0,  10 );
          // differences as a function of position on HPD
          plot2D( lPos.X()-hpdLoc.X(), lPos.X()-mcPointLoc.X(), hid(rich,"pdImpXlocVX"),
                  "dX V X local on HPD entry window : CK signal only", -35, 35, -8, 8 );
          plot2D( lPos.Y()-hpdLoc.Y(), lPos.Y()-mcPointLoc.Y(), hid(rich,"pdImpYlocVY"),
                  "dY V Y local on HPD entry window : CK signal only", -35, 35, -8, 8 );
          const double R   = std::sqrt( gsl_pow_2(lPos.X()-hpdLoc.X()) +
                                        gsl_pow_2(lPos.Y()-hpdLoc.Y()) );
          const double McR = std::sqrt( gsl_pow_2(mcPointLoc.X()-hpdLoc.X()) +
                                        gsl_pow_2(mcPointLoc.Y()-hpdLoc.Y()) );
          plot2D( R, R-McR, hid(rich,"pdImpRlocVR"),
                  "dR V R local on HPD entry window : CK signal only", 0, 40, -10, 10 );
          if ( msgLevel(MSG::VERBOSE) )
          {
            verbose() << "Global : Pixel = " << (*iHit)->entry()
                      << " : MC HPD window = " << mcPoint << endmsg
                      << "Local  : Pixel = " << m_idTool->globalToPDPanel((*iHit)->entry())
                      << " : MC HPD window = " << mcPointLoc << endmsg;
          }
        }
//         // MC to rec position on HPD anode
//         const Gaudi::XYZPoint & mcPointOnAnode = (*iHit)->entry();
//         // position on anode in local coords
//         const Gaudi::XYZPoint mcPointOnAnodeLoc = m_idTool->globalToPDPanel(mcPointOnAnode);
//         plot1D( anodeGlobal.x() - mcPointOnAnode.x(), hid(rich,side,"anodeXDiffGlobal"),
//                 "dX on HPD Anode : Global LHCb Coords", -1, 1 );
//         plot1D( anodeGlobal.y() - mcPointOnAnode.y(), hid(rich,side,"anodeYDiffGlobal"),
//                 "dY on HPD Anode : Global LHCb Coords", -1, 1 );
//         plot1D( anodeGlobal.z() - mcPointOnAnode.z(), hid(rich,side,"anodeZDiffGlobal"),
//                 "dZ on HPD Anode : Global LHCb Coords", -1, 1 );
//         plot1D( anodeLocal.x() - mcPointOnAnodeLoc.x(), hid(rich,side,"anodeXDiffLocal"),
//                 "dX on HPD Anode : HPD Panel local Coords", -1, 1 );
//         plot1D( anodeLocal.y() - mcPointOnAnodeLoc.y(), hid(rich,side,"anodeYDiffLocal"),
//                 "dY on HPD Anode : HPD Panel local Coords", -1, 1 );
//         plot1D( anodeLocal.z() - mcPointOnAnodeLoc.z(), hid(rich,side,"anodeZDiffLocal"),
//                 "dZ on HPD Anode : HPD Panel local Coords", -1, 1 );
      }
    }

  } // loop over all pixels

  debug() << "RICH1 Pixels :-" << endmsg;
  unsigned int nR1(0);
  IPixelCreator::PixelRange range1 = pixelCreator()->range(Rich::Rich1);
  for ( IPixelCreator::PixelRange::const_iterator iPix = range1.begin();
        iPix != range1.end(); ++iPix )
  {
    ++nR1;
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "  -> Pixel " << (*iPix)->hpdPixelCluster() << endmsg;
    }
  }
  if ( nR1 != nPixs[Rich::Rich1] )
    Error("Mis-match in RICH1 pixels between full container and RICH1 iterators");

  debug() << "RICH2 Pixels :-" << endmsg;
  unsigned int nR2(0);
  IPixelCreator::PixelRange range2 = pixelCreator()->range(Rich::Rich2);
  for ( IPixelCreator::PixelRange::const_iterator iPix = range2.begin();
        iPix != range2.end(); ++iPix )
  {
    ++nR2;
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "  -> Pixel " << (*iPix)->hpdPixelCluster() << endmsg;
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
    Gaudi::XYZPoint hpdGlo;
    const StatusCode sc = m_idTool->pdPosition(hpdID,hpdGlo);
    if ( sc.isFailure() ) { Warning( "Problem getting HPD position" ); continue; }
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
      Gaudi::XYZPoint tmpP;
      if ( !m_idTool->globalPosition(*iS,tmpP) ) continue;
      const Gaudi::XYZVector hitP = m_idTool->globalToPDPanel(tmpP) - hpdLoc;
      verbose() << "Hit " << (*iS) << " " << hitP << endmsg;
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
               xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich], m_n2DBins, m_n2DBins );
  }

  return StatusCode::SUCCESS;
}

