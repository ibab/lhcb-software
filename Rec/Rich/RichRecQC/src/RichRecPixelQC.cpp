
//-----------------------------------------------------------------------------
/** @file RichRecPixelQC.cpp
 *
 *  Implementation file for algorithm class : RichRecPixelQC
 *
 *  $Id: RichRecPixelQC.cpp,v 1.6 2006-05-05 10:51:38 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#include <fstream>

// local
#include "RichRecPixelQC.h"

// namespace
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichRecPixelQC>          s_factory ;
const        IAlgFactory& RichRecPixelQCFactory = s_factory ;


// Standard constructor, initializes variables
RichRecPixelQC::RichRecPixelQC( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( 0 ),
    m_truth             ( 0 ),
    m_richSys           ( 0 ),
    m_pixelsRaw         ( Rich::NRiches, 0 ),
    m_bkgsRaw           ( Rich::NRiches, 0 ),
    m_npdqcksRaw        ( Rich::NRiches, 0 ),
    m_signalRaw         ( Rich::NRiches, 0 ),
    m_radHitsRaw        ( Rich::NRadiatorTypes, 0 ),
    m_pixels            ( Rich::NRiches, 0 ),
    m_bkgs              ( Rich::NRiches, 0 ),
    m_npdqcks           ( Rich::NRiches, 0 ),
    m_signal            ( Rich::NRiches, 0 ),
    m_radHits           ( Rich::NRadiatorTypes, 0 ),
    m_nEvts             ( 0 )
{
}

// Destructor
RichRecPixelQC::~RichRecPixelQC() {};

//  Initialize
StatusCode RichRecPixelQC::initialize()
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

  return sc;
}

// Main execution
StatusCode RichRecPixelQC::execute()
{
  debug() << "Execute" << endreq;

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all pixels have been formed
  if ( richPixels()->empty() )
  {
    const StatusCode sc = pixelCreator()->newPixels();
    if ( sc.isFailure() )
    { return Error( "Problem creating RichRecPixels", sc ); }
    debug() << "No Pixels found : Created "
            << richPixels()->size() << " RichRecPixels" << endreq;
  }

  // count events
  ++m_nEvts;

  // Histogramming
  const RichHistoID hid;

  std::vector<unsigned int> pixels( Rich::NRiches, 0 );
  std::vector<unsigned int> bkgs( Rich::NRiches, 0 );
  std::vector<unsigned int> npdqcks( Rich::NRiches, 0 );
  std::vector<unsigned int> signal( Rich::NRiches, 0 );

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
    // RICH
    const Rich::DetectorType rich = hpd.rich();

    // Loop over raw RichSmartIDs
    for ( RichSmartID::Vector::const_iterator iR = rawIDs.begin();
          iR != rawIDs.end(); ++iR )
    {
      verbose() << "Decoded hit " << *iR << endreq;
      // flags
      bool isBkg,isHPDQCK,isSignal,isAerogelCK,isRich1GasCK,isRich2GasCK;
      getHistories( *iR, isBkg,isHPDQCK,isSignal,isAerogelCK,isRich1GasCK,isRich2GasCK );
      // count
      ++m_pixelsRaw[rich];
      if ( isBkg    ) ++m_bkgsRaw[rich];
      if ( isHPDQCK ) ++m_npdqcksRaw[rich];
      if ( isSignal ) ++m_signalRaw[rich];
      if ( isAerogelCK ) ++m_radHitsRaw[Rich::Aerogel];
      if ( isRich1GasCK ) ++m_radHitsRaw[Rich::Rich1Gas];
      if ( isRich2GasCK   ) ++m_radHitsRaw[Rich::Rich2Gas];
    } // raw channel ids

    // Get the reconstructed pixels for this HPD
    RichRecPixels::const_iterator iPixel = pixelCreator()->begin ( hpd );
    RichRecPixels::const_iterator endPix = pixelCreator()->end   ( hpd );
    unsigned int nHPDHits(0);
    // loop over reconstructed pixels
    for ( ; iPixel != endPix; ++iPixel )
    {
      // flags
      bool isBkg,isHPDQCK,isSignal,isAerogelCK,isRich1GasCK,isRich2GasCK;
      getHistories( (*iPixel)->smartID(),
                    isBkg,isHPDQCK,isSignal,isAerogelCK,isRich1GasCK,isRich2GasCK );
      // count
      ++nHPDHits;
      ++pixels[rich];
      if ( isBkg    ) ++bkgs[rich];
      if ( isHPDQCK ) ++npdqcks[rich];
      if ( isSignal ) ++signal[rich];
      ++m_pixels[rich];
      if ( isBkg    ) ++m_bkgs[rich];
      if ( isHPDQCK ) ++m_npdqcks[rich];
      if ( isSignal ) ++m_signal[rich];
      if ( isAerogelCK ) ++m_radHits[Rich::Aerogel];
      if ( isRich1GasCK ) ++m_radHits[Rich::Rich1Gas];
      if ( isRich2GasCK   ) ++m_radHits[Rich::Rich2Gas];
    }

    plot1D( nHPDHits, hid(rich,"nPixsPerHPD"), "Average HPD occupancy (nHits>0)", 0, 150, 75 );

  } // loop over HPDs

  plot1D( pixels[Rich::Rich1], hid(Rich::Rich1,"nPixs"), "Overall occupancy (nHits>0)", 0, 5000, 100 );
  plot1D( pixels[Rich::Rich2], hid(Rich::Rich2,"nPixs"), "Overall occupancy (nHits>0)", 0, 2000, 100 );

  return StatusCode::SUCCESS;
}

void RichRecPixelQC::getHistories( const RichSmartID id,
                                   bool & isBkg,
                                   bool & isHPDQCK,
                                   bool & isSignal,
                                   bool & isAerogelCK,
                                   bool & isRich1GasCK,
                                   bool & isRich2GasCK ) const
{
  // set to defaults
  isBkg       = false;
  isHPDQCK    = false;
  isSignal    = false;
  isAerogelCK = false;
  isRich1GasCK   = false;
  isRich2GasCK     = false;
  // get MC histories for this RichSmartID
  typedef std::vector<const LHCb::MCRichDigitSummary*> Summaries;
  Summaries summaries;
  m_truth->getMcHistories( id, summaries );
  // loop over summaries and set various flags
  for ( Summaries::const_iterator iS = summaries.begin();
        iS != summaries.end(); ++iS )
  {
    if ( (*iS)->history().isBackground() ) { isBkg    = true; }
    if ( (*iS)->history().hpdQuartzCK()  ) { isHPDQCK = true; }
    if ( (*iS)->history().isSignal()     ) { isSignal = true; }
    if ( (*iS)->history().aerogelHit()   ) { isAerogelCK = true; }
    if ( (*iS)->history().rich1GasHit()  ) { isRich1GasCK = true; }
    if ( (*iS)->history().rich2GasHit()  ) { isRich2GasCK = true; }
  }
}

//  Finalize
StatusCode RichRecPixelQC::finalize()
{

  if ( m_nEvts > 0 )
  {
    info() << "===============================================================================" << endreq
           << "                         Pixel summary for " << m_nEvts << " events :-" << endreq
           << "-------------------------------------------------------------------------------" << endreq;
    printRICH(Rich::Rich1);
    info() << "-------------------------------------------------------------------------------" << endreq;
    printRICH(Rich::Rich2);
    info() << "===============================================================================" << endreq;
  }

  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}

void RichRecPixelQC::printRICH( const Rich::DetectorType rich ) const
{
  const RichStatDivFunctor     occ  ("%8.2f +-%7.2f");
  const RichPoissonEffFunctor  pois ("%7.2f +-%6.2f");

  info() << "  " << rich << " : All pixels         : " << occ(m_pixels[rich],m_nEvts)
         << "   Eff. = " << pois(m_pixels[rich],m_pixelsRaw[rich]) << " %" << endreq;

  info() << "        : Cherenkov Signal   : " << occ(m_signal[rich],m_nEvts)
         << "   Eff. = " << pois(m_signal[rich],m_signalRaw[rich]) << " %" << endreq;

  if ( Rich::Rich1 == rich )
  {
    info() << "        :     Aerogel        : " << occ(m_radHitsRaw[Rich::Aerogel],m_nEvts)
           << "   Eff. = " << pois(m_radHits[Rich::Aerogel],m_radHitsRaw[Rich::Aerogel])
           << " %" << endreq;
    info() << "        :     Rich1Gas       : " << occ(m_radHitsRaw[Rich::Rich1Gas],m_nEvts)
           << "   Eff. = " << pois(m_radHits[Rich::Rich1Gas],m_radHitsRaw[Rich::Rich1Gas])
           << " %" << endreq;
  }
  else
  {
    info() << "        :     Rich2Gas       : " << occ(m_radHitsRaw[Rich::Rich2Gas],m_nEvts)
           << "   Eff. = " << pois(m_radHits[Rich::Rich2Gas],m_radHitsRaw[Rich::Rich2Gas])
           << " %" << endreq;
  }

  info() << "        : All Backgrounds    : " << occ(m_bkgs[rich],m_nEvts)
         << "   Eff. = " << pois(m_bkgs[rich],m_bkgsRaw[rich]) << " %" << endreq;

  info() << "        :   - HPD Quartz CK  : " << occ(m_npdqcks[rich],m_nEvts)
         << "   Eff. = " << pois(m_npdqcks[rich],m_npdqcksRaw[rich]) << " %" << endreq;
}
