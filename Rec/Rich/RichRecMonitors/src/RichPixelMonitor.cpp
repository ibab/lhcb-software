
//-----------------------------------------------------------------------------
/** @file RichPixelMonitor.cpp
 *
 *  Implementation file for algorithm class : RichPixelMonitor
 *
 *  $Id: RichPixelMonitor.cpp,v 1.1.1.1 2005-06-18 11:44:46 jonrob Exp $
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
  : RichRecAlgBase ( name, pSvcLocator ) {

  declareProperty( "MCTruth", m_truth = true );
  declareProperty( "MCHistoPath", m_mcHistPth = "RICH/RECPIX/MC/" );
  declareProperty( "HistoPath", m_histPth = "RICH/RECPIX/" );

}

// Destructor
RichPixelMonitor::~RichPixelMonitor() {};

//  Initialize
StatusCode RichPixelMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  if ( m_truth ) 
  {
    acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );
    if ( !m_richRecMCTruth ) m_truth = false;
  }

  // Book histograms
  if ( !bookHistograms() ) return StatusCode::FAILURE;
  if ( m_truth ) { if ( !bookMCHistograms() ) return StatusCode::FAILURE; }

  // initialise variables
  m_xHits.clear();
  m_yHits.clear();
  m_zHits.clear();
  m_hitCount.clear();

  return sc;
}

StatusCode RichPixelMonitor::bookHistograms()
{

  std::string title;
  int nBins = 100;
  int id;

  // defines for various parameters
  RICH_HISTO_OFFSET;
  RICH_NAMES;
  PD_GLOBAL_POSITIONS;
  PD_LOCAL_POSITIONS;

  for ( int iRich = 0; iRich<Rich::NRiches; ++iRich ) {

    title = rich[iRich]+" observed hits x global";
    id = (1+iRich)*richOffset+1;
    m_obsHitsXGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                               nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

    title = rich[iRich]+" observed hits y global";
    id = (1+iRich)*richOffset+2;
    m_obsHitsYGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                               nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" observed hits z global";
    id = (1+iRich)*richOffset+3;
    m_obsHitsZGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                               nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich]);

    title = rich[iRich]+" observed hits yVx global";
    id = (1+iRich)*richOffset+4;
    m_obsHitsXYGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                                nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich],
                                                nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" observed hits xVz global";
    id = (1+iRich)*richOffset+5;
    m_obsHitsXZGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                                nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                                                nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

    title = rich[iRich]+" observed hits yVz global";
    id = (1+iRich)*richOffset+6;
    m_obsHitsYZGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                                nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                                                nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" observed hits x local";
    id = (1+iRich)*richOffset+11;
    m_obsHitsXLocal[iRich] = histoSvc()->book(m_histPth,id,title,
                                              nBins,xMinPDLoc[iRich],xMaxPDLoc[iRich]);

    title = rich[iRich]+" observed hits y local";
    id = (1+iRich)*richOffset+12;
    m_obsHitsYLocal[iRich] = histoSvc()->book(m_histPth,id,title,
                                              nBins,yMinPDLoc[iRich],yMaxPDLoc[iRich]);

    title = rich[iRich]+" observed hits z local";
    id = (1+iRich)*richOffset+13;
    m_obsHitsZLocal[iRich] = histoSvc()->book(m_histPth,id,title,
                                              nBins,zMinPDLoc[iRich],zMaxPDLoc[iRich]);

    title = rich[iRich]+" observed hits yVx local";
    id = (1+iRich)*richOffset+14;
    m_obsHitsXYLocal[iRich] = histoSvc()->book(m_histPth,id,title,
                                               nBins,xMinPDLoc[iRich],xMaxPDLoc[iRich],
                                               nBins,yMinPDLoc[iRich],yMaxPDLoc[iRich]);

    title = rich[iRich]+" pixel log(background)";
    id = (1+iRich)*richOffset+21;
    m_pixBkg[iRich] = histoSvc()->book(m_histPth,id,title,nBins,-20,0);

  } // end Rich loop

  // PD Close Up plots
  title = rich[Rich::Rich1]+" PD closeUp xVz";
  id = richOffset+41;
  m_pdCloseUpXZ[Rich::Rich1] = histoSvc()->book(m_histPth,id,title,
                                                nBins, 1485, 1560, nBins, -50, 50 );

  title = rich[Rich::Rich1]+" PD closeUp yVz";
  id = richOffset+42;
  m_pdCloseUpYZ[Rich::Rich1] = histoSvc()->book(m_histPth,id,title,
                                                nBins, 1485, 1560, nBins, 920, 1020 );

  title = rich[Rich::Rich2]+" PD closeUp xVz";
  id = 2*richOffset+41;
  m_pdCloseUpXZ[Rich::Rich2] = histoSvc()->book(m_histPth,id,title,
                                                nBins, 10720, 10800, nBins, 3800, 4000 );

  title = rich[Rich::Rich2]+" PD closeUp yVz";
  id = 2*richOffset+42;
  m_pdCloseUpYZ[Rich::Rich2] = histoSvc()->book(m_histPth,id,title,
                                                nBins, 10720, 10800, nBins, -70, 30 );

  return StatusCode::SUCCESS;
}

StatusCode RichPixelMonitor::bookMCHistograms()
{

  std::string title;
  int nBins = 100;
  int id;

  // defines for various parameters
  RICH_HISTO_OFFSET;
  RAD_HISTO_OFFSET;
  RICH_NAMES;
  RADIATOR_NAMES;
  PD_GLOBAL_POSITIONS;
  PD_LOCAL_POSITIONS;

  for ( int iRich = 0; iRich<Rich::NRiches; ++iRich ) {

    title = rich[iRich]+" observed background hits x global";
    id = (1+iRich)*richOffset+1;
    m_obsBkgHitsXGlobal[iRich] = histoSvc()->book(m_mcHistPth,id,title,
                                                  nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

    title = rich[iRich]+" observed background hits y global";
    id = (1+iRich)*richOffset+2;
    m_obsBkgHitsYGlobal[iRich] = histoSvc()->book(m_mcHistPth,id,title,
                                                  nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" observed background hits z global";
    id = (1+iRich)*richOffset+3;
    m_obsBkgHitsZGlobal[iRich] = histoSvc()->book(m_mcHistPth,id,title,
                                                  nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich]);

    title = rich[iRich]+" observed background hits yVx global";
    id = (1+iRich)*richOffset+4;
    m_obsBkgHitsXYGlobal[iRich] = histoSvc()->book(m_mcHistPth,id,title,
                                                   nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich],
                                                   nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" observed background hits xVz global";
    id = (1+iRich)*richOffset+5;
    m_obsBkgHitsXZGlobal[iRich] = histoSvc()->book(m_mcHistPth,id,title,
                                                   nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                                                   nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

    title = rich[iRich]+" observed background hits yVz global";
    id = (1+iRich)*richOffset+6;
    m_obsBkgHitsYZGlobal[iRich] = histoSvc()->book(m_mcHistPth,id,title,
                                                   nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                                                   nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" observed background hits x local";
    id = (1+iRich)*richOffset+11;
    m_obsBkgHitsXLocal[iRich] = histoSvc()->book(m_mcHistPth,id,title,
                                                 nBins,xMinPDLoc[iRich],xMaxPDLoc[iRich]);

    title = rich[iRich]+" observed background hits y local";
    id = (1+iRich)*richOffset+12;
    m_obsBkgHitsYLocal[iRich] = histoSvc()->book(m_mcHistPth,id,title,
                                                 nBins,yMinPDLoc[iRich],yMaxPDLoc[iRich]);

    title = rich[iRich]+" observed background hits z local";
    id = (1+iRich)*richOffset+13;
    m_obsBkgHitsZLocal[iRich] = histoSvc()->book(m_mcHistPth,id,title,
                                                 nBins,zMinPDLoc[iRich],zMaxPDLoc[iRich]);

    title = rich[iRich]+" observed background hits yVx local";
    id = (1+iRich)*richOffset+14;
    m_obsBkgHitsXYLocal[iRich] = histoSvc()->book(m_mcHistPth,id,title,
                                                  nBins,xMinPDLoc[iRich],xMaxPDLoc[iRich],
                                                  nBins,yMinPDLoc[iRich],yMaxPDLoc[iRich]);

  }

  // radiator loop
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) 
  {

    // which RICH ?
    const int iRich = ( iRad == Rich::CF4 ? Rich::Rich2 : Rich::Rich1 ); 

    title = radiator[iRad]+" signal hits";
    id = radOffset*(iRad+1) + 1;
    m_sigHitsLoc[iRad] =
      histoSvc()->book(m_mcHistPth,id,title,
                       nBins,xMinPDLoc[iRich],xMaxPDLoc[iRich],
                       nBins,yMinPDLoc[iRich],yMaxPDLoc[iRich]);

  }

  return StatusCode::SUCCESS;
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
    m_obsHitsXGlobal[iRich]->fill( gPos.x() );
    m_obsHitsYGlobal[iRich]->fill( gPos.y() );
    m_obsHitsZGlobal[iRich]->fill( gPos.z() );
    m_obsHitsXYGlobal[iRich]->fill( gPos.x(), gPos.y() );
    m_obsHitsXZGlobal[iRich]->fill( gPos.z(), gPos.x() );
    m_obsHitsYZGlobal[iRich]->fill( gPos.z(), gPos.y() );

    // local position on HP panels
    const HepPoint3D & lPos = pixel->localPosition();
    m_obsHitsXLocal[iRich]->fill( lPos.x() );
    m_obsHitsYLocal[iRich]->fill( lPos.y() );
    m_obsHitsZLocal[iRich]->fill( lPos.z() );
    m_obsHitsXYLocal[iRich]->fill( lPos.x(), lPos.y() );

    // Background plots
    if ( m_richRecMCTruth->isBackground(pixel) )
    {
      m_obsBkgHitsXGlobal[iRich]->fill( gPos.x() );
      m_obsBkgHitsYGlobal[iRich]->fill( gPos.y() );
      m_obsBkgHitsZGlobal[iRich]->fill( gPos.z() );
      m_obsBkgHitsXYGlobal[iRich]->fill( gPos.x(), gPos.y() );
      m_obsBkgHitsXZGlobal[iRich]->fill( gPos.z(), gPos.x() );
      m_obsBkgHitsYZGlobal[iRich]->fill( gPos.z(), gPos.y() );
      m_obsBkgHitsXLocal[iRich]->fill( lPos.x() );
      m_obsBkgHitsYLocal[iRich]->fill( lPos.y() );
      m_obsBkgHitsZLocal[iRich]->fill( lPos.z() );
      m_obsBkgHitsXYLocal[iRich]->fill( lPos.x(), lPos.y() );
    }

    // PD close up
    m_pdCloseUpXZ[iRich]->fill( gPos.z(), gPos.x() );
    m_pdCloseUpYZ[iRich]->fill( gPos.z(), gPos.y() );

    // background value distribution
    const double logPixBkg = 
      ( pixel->currentBackground() > 1e-20 ? log(pixel->currentBackground()) : -20 );
    m_pixBkg[iRich]->fill( logPixBkg );

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
      if ( mcP ) m_sigHitsLoc[iRad]->fill( lPos.x(), lPos.y() );
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

StatusCode RichPixelMonitor::bookFinalHistograms()
{

  // book pd number histograms
  RICH_HISTO_OFFSET;
  RICH_NAMES;
  PD_GLOBAL_POSITIONS;
  const std::string panel[] = { "panel0", "panel1" };
  int nBins = 25;
  int id;
  std::string title;

  for ( int iRich = 0; iRich<Rich::NRiches; ++iRich ) { // loop over rich detectors
    for ( int iPan = 0; iPan<Rich::NRiches; ++iPan ) {  // loop over panels

      title = rich[iRich]+" "+panel[iPan]+" PDCol yVx";
      id = richOffset*(iRich+1) + 101 +iPan*3;
      m_pdColNumXY[iRich][iPan] = histoSvc()->book(m_histPth,id,title,
                                                   nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich],
                                                   nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

      title = rich[iRich]+" "+panel[iPan]+" PDCol xVz";
      id = richOffset*(iRich+1) + 102 +iPan*3;
      m_pdColNumXZ[iRich][iPan] = histoSvc()->book(m_histPth,id,title,
                                                   nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                                                   nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

      title = rich[iRich]+" "+panel[iPan]+" PDCol yVz";
      id = richOffset*(iRich+1) + 103 +iPan*3;
      m_pdColNumYZ[iRich][iPan] = histoSvc()->book(m_histPth,id,title,
                                                   nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                                                   nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

      title = rich[iRich]+" "+panel[iPan]+" PDRow yVx";
      id = richOffset*(iRich+1) + 201 +iPan*3;
      m_pdRowNumXY[iRich][iPan] = histoSvc()->book(m_histPth,id,title,
                                                   nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich],
                                                   nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

      title = rich[iRich]+" "+panel[iPan]+" PDRow xVz";
      id = richOffset*(iRich+1) + 202 +iPan*3;
      m_pdRowNumXZ[iRich][iPan] = histoSvc()->book(m_histPth,id,title,
                                                   nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                                                   nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

      title = rich[iRich]+" "+panel[iPan]+" PDRow yVz";
      id = richOffset*(iRich+1) + 203 +iPan*3;
      m_pdRowNumYZ[iRich][iPan] = histoSvc()->book(m_histPth,id,title,
                                                   nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                                                   nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    }
  }

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichPixelMonitor::finalize()
{

  // Book final histograms
  if ( !bookFinalHistograms() ) return StatusCode::FAILURE;

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
    m_pdColNumXY[iRich][pdID.panel()]->fill( avX, avY, pdID.pdCol() );
    m_pdColNumXZ[iRich][pdID.panel()]->fill( avZ, avX, pdID.pdCol() );
    m_pdColNumYZ[iRich][pdID.panel()]->fill( avZ, avY, pdID.pdCol() );
    m_pdRowNumXY[iRich][pdID.panel()]->fill( avX, avY, pdID.pdRow() );
    m_pdRowNumXZ[iRich][pdID.panel()]->fill( avZ, avX, pdID.pdRow() );
    m_pdRowNumYZ[iRich][pdID.panel()]->fill( avZ, avY, pdID.pdRow() );
  }

  // Execute base class method
  return RichRecAlgBase::finalize();
}
