
//-----------------------------------------------------------------------------
/** @file RichRecPixelQC.cpp
 *
 *  Implementation file for algorithm class : RichRecPixelQC
 *
 *  $Id: RichRecPixelQC.cpp,v 1.1 2006-03-02 15:27:39 jonrob Exp $
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
    m_pixels            ( Rich::NRiches, 0 ),
    m_bkgs              ( Rich::NRiches, 0 ),
    m_npdqcks           ( Rich::NRiches, 0 ),
    m_signal            ( Rich::NRiches, 0 ),
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
    if ( pixelCreator()->newPixels().isFailure() )
      return Error( "Problem creating RichRecPixels" );
    debug() << "No Pixels found : Created "
            << richPixels()->size() << " RichRecPixels" << endreq;
  }

  // count events
  ++m_nEvts;

  // Histogramming
  const RichHistoID hid;
  //PD_GLOBAL_POSITIONS;
  //PD_LOCAL_POSITIONS;

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

    // Hardware ID
    //const RichDAQ::HPDHardwareID hardID = m_richSys->hardwareID(hpd);
    // HPD as a string
    //std::ostringstream HPD; HPD << hpd;

    // Loop over raw RichSmartIDs
    for ( RichSmartID::Vector::const_iterator iR = rawIDs.begin();
          iR != rawIDs.end(); ++iR )
    {
      // flags
      bool isBkg(false), isHPDQCK(false), isSignal(false);
      getHistories( *iR, isBkg, isHPDQCK, isSignal );

      // count
      ++m_pixelsRaw[rich];
      if ( isBkg    ) ++m_bkgsRaw[rich];
      if ( isHPDQCK ) ++m_npdqcksRaw[rich];
      if ( isSignal ) ++m_signalRaw[rich];

    } // raw channel ids

    // Get the reconstructed pixels for this HPD
    RichRecPixels::const_iterator iPixel = pixelCreator()->begin ( hpd );
    RichRecPixels::const_iterator endPix = pixelCreator()->end   ( hpd );
    unsigned int nHPDHits(0);
    // loop over reconstructed pixels
    for ( ; iPixel != endPix; ++iPixel )
    {
      // flags
      bool isBkg(false), isHPDQCK(false), isSignal(false);
      getHistories( (*iPixel)->smartID(), isBkg, isHPDQCK, isSignal );

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
                                   bool & isSignal ) const
{
  // get MC histories for this RichSmartID
  typedef std::vector<const LHCb::MCRichDigitSummary*> Summaries;
  Summaries summaries;
  m_truth->getMcHistories( id, summaries );
  // loop over summaries and see if this HPD has any background
  for ( Summaries::const_iterator iS = summaries.begin();
        iS != summaries.end(); ++iS )
  {
    if ( (*iS)->history().isBackground() ) { isBkg = true;    }
    if ( (*iS)->history().hpdQuartzCK()  ) { isHPDQCK = true; }
    if ( (*iS)->history().isSignal()     ) { isSignal = true; }
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

  info() << "        : All Backgrounds    : " << occ(m_bkgs[rich],m_nEvts) 
         << "   Eff. = " << pois(m_bkgs[rich],m_bkgsRaw[rich]) << " %" << endreq;

  info() << "        :   - HPD Quartz CK  : " << occ(m_npdqcks[rich],m_nEvts) 
         << "   Eff. = " << pois(m_npdqcks[rich],m_npdqcksRaw[rich]) << " %" << endreq;
}
