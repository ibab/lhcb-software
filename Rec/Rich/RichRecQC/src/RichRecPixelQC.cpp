
//-----------------------------------------------------------------------------
/** @file RichRecPixelQC.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::MC::PixelQC
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecPixelQC.h"

// working namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PixelQC )

// Standard constructor, initializes variables
PixelQC::PixelQC( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : HistoAlgBase        ( name, pSvcLocator ),
    m_richRecMCTruth    ( NULL ),
    m_truth             ( NULL ),
    m_richSys           ( NULL ),
    m_nEvts             ( 0    ) 
{
  declareProperty( "MaxPixels", m_maxPixels = 10000 );
}

// Destructor
PixelQC::~PixelQC() { }

//  Initialize
StatusCode PixelQC::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth   );
  acquireTool( "RichMCTruthTool",    m_truth );
  acquireTool( "RichSmartIDDecoder", m_decoder, 0, true );

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  return sc;
}

StatusCode PixelQC::prebookHistograms()
{

  for ( Rich::Detectors::const_iterator rich = Rich::detectors().begin();
        rich != Rich::detectors().end(); ++rich )
  {
    richHisto1D( Rich::HistogramID("nTotalPixsPerHPD",*rich),
                 "Average overall HPD occupancy (nHits>0)",
                 0.5, 150.5, 150 );
    richHisto1D( Rich::HistogramID("nTotalPixs",*rich), 
                 "Overall occupancy (nHits>0)", 0, m_maxPixels, nBins1D() );
    richHisto1D( Rich::HistogramID("nActiveHPDs",*rich), 
                 "# Active HPDs (nHits>0)", -0.5, 300.5, 301 );
  }

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode PixelQC::execute()
{
  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all pixels have been formed
  if ( richPixels()->empty() )
  {
    const StatusCode sc = pixelCreator()->newPixels();
    if ( sc.isFailure() )
    { return Error( "Problem creating RichRecPixels", sc ); }
    debug() << "No Pixels found : Created "
            << richPixels()->size() << " RichRecPixels" << endmsg;
  }

  // count events
  ++m_nEvts;

  // Histogramming
  const Rich::HistoID hid;

  // Count pixels, signal pixels and active HPDs per RICH
  std::vector<unsigned int> pixels ( Rich::NRiches, 0 );
  std::vector<unsigned int> signal ( Rich::NRiches, 0 );
  std::vector<unsigned int> hpds   ( Rich::NRiches, 0 );

  // Obtain data from raw decoding
  const DAQ::L1Map & data = m_decoder->allRichSmartIDs();

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

        // HPD ID
        const LHCb::RichSmartID hpd = (*iHPD).second.hpdID();
        if ( hpd.isValid() && !(*iHPD).second.header().inhibit() )
        {

          // Vector of SmartIDs
          const LHCb::RichSmartID::Vector & rawIDs = (*iHPD).second.smartIDs();
          // RICH
          const Rich::DetectorType rich = hpd.rich();

          // Count active HPDs (at least 1 hit)
          if ( !rawIDs.empty() ) { ++hpds[rich]; }

          // Loop over raw RichSmartIDs
          for ( LHCb::RichSmartID::Vector::const_iterator iR = rawIDs.begin();
                iR != rawIDs.end(); ++iR )
          {
            // flags
            const MCFlags flags = getHistories( *iR );
            // count
            ++m_rawTally.pixels[rich];
            if ( flags.isBkg           ) { ++m_rawTally.bkgs[rich]; }
            if ( flags.isHPDQCK        ) { ++m_rawTally.npdqcks[rich]; }
            if ( flags.isGasCK         ) { ++m_rawTally.ngasck[rich]; }
            if ( flags.isN2CK          ) { ++m_rawTally.n2ck[rich]; }
            if ( flags.isChargedTk     ) { ++m_rawTally.ntrack[rich]; }
            if ( flags.isChargeShare   ) { ++m_rawTally.nchargeshare[rich]; }
            if ( flags.isAeroFiltCK    ) { ++m_rawTally.naerofilter[rich]; }
            if ( flags.isSiBackScatter ) { ++m_rawTally.nbackscatter[rich]; }
            if ( flags.isHPDIntReflect ) { ++m_rawTally.nhpdintreflect[rich]; }
            if ( flags.isRadScint      ) { ++m_rawTally.radScint[rich];}
            if ( flags.isSignal        ) { ++m_rawTally.signal[rich]; }
            if ( flags.isAerogelCK     ) { ++m_rawTally.radHits[Rich::Aerogel]; }
            if ( flags.isRich1GasCK    ) { ++m_rawTally.radHits[Rich::Rich1Gas]; }
            if ( flags.isRich2GasCK    ) { ++m_rawTally.radHits[Rich::Rich2Gas]; }
          } // raw channel ids

          // Get the reconstructed pixels for this HPD
          IPixelCreator::PixelRange range = pixelCreator()->range(hpd);
          unsigned int nHPDHits(0), nHPDSignalHits(0);
          // loop over reconstructed pixels
          for ( IPixelCreator::PixelRange::const_iterator iPixel = range.begin();
                iPixel != range.end(); ++iPixel )
          {
            // for each pixel loop over the smartIDs
            for ( LHCb::RichSmartID::Vector::const_iterator iS = 
                    (*iPixel)->hpdPixelCluster().smartIDs().begin();
                  iS != (*iPixel)->hpdPixelCluster().smartIDs().end(); ++iS )
            {
              // flags
              const MCFlags flags = getHistories( *iS );
              // count
              ++nHPDHits;
              ++pixels[rich];
              ++m_recoTally.pixels[rich];
              if ( flags.isBkg           ) { ++m_recoTally.bkgs[rich]; }
              if ( flags.isHPDQCK        ) { ++m_recoTally.npdqcks[rich]; }
              if ( flags.isGasCK         ) { ++m_recoTally.ngasck[rich]; }
              if ( flags.isN2CK          ) { ++m_recoTally.n2ck[rich]; }
              if ( flags.isChargedTk     ) { ++m_recoTally.ntrack[rich]; }
              if ( flags.isChargeShare   ) { ++m_recoTally.nchargeshare[rich]; }
              if ( flags.isAeroFiltCK    ) { ++m_recoTally.naerofilter[rich]; }
              if ( flags.isSiBackScatter ) { ++m_recoTally.nbackscatter[rich]; }
              if ( flags.isHPDIntReflect ) { ++m_recoTally.nhpdintreflect[rich]; }
              if ( flags.isRadScint      ) { ++m_recoTally.radScint[rich];}
              if ( flags.isSignal        ) { ++nHPDSignalHits; ++signal[rich]; ++m_recoTally.signal[rich]; }
              if ( flags.isAerogelCK     ) { ++m_recoTally.radHits[Rich::Aerogel]; }
              if ( flags.isRich1GasCK    ) { ++m_recoTally.radHits[Rich::Rich1Gas]; }
              if ( flags.isRich2GasCK    ) { ++m_recoTally.radHits[Rich::Rich2Gas]; }
            }
          }

          if ( nHPDHits>0 )
          {
            richHisto1D( Rich::HistogramID("nTotalPixsPerHPD",rich) ) -> fill( nHPDHits );
          }
          if ( nHPDSignalHits>0 )
          {
            richHisto1D( Rich::HistogramID("nSignalPixsPerHPD",rich),
                         "Average signal HPD occupancy (nHits>0)",
                         0, 150, 75 ) -> fill( nHPDSignalHits );
          }

        } // valid HPD ID

      } // loop over HPDs
    } // ingresses
  } // L1 boards

  richHisto1D( Rich::HistogramID("nActiveHPDs",Rich::Rich1) )->fill( hpds[Rich::Rich1] );
  richHisto1D( Rich::HistogramID("nActiveHPDs",Rich::Rich2) )->fill( hpds[Rich::Rich2] );

  if ( pixels[Rich::Rich1] > 0 )
    richHisto1D(Rich::HistogramID("nTotalPixs",Rich::Rich1)) -> fill ( pixels[Rich::Rich1] );
  if ( pixels[Rich::Rich2] > 0 )
    richHisto1D(Rich::HistogramID("nTotalPixs",Rich::Rich2)) -> fill ( pixels[Rich::Rich2] );
  if ( signal[Rich::Rich1] > 0 )
    richHisto1D( Rich::HistogramID("nSignalPixs",Rich::Rich1),
                 "Signal occupancy (nHits>0)", 0, 5000, 100 ) -> fill ( signal[Rich::Rich1] );
  if ( signal[Rich::Rich2] > 0 )
    richHisto1D( Rich::HistogramID("nSignalPixs",Rich::Rich2),
                 "Signal occupancy (nHits>0)", 0, 5000, 100 ) -> fill ( signal[Rich::Rich2] );

  return StatusCode::SUCCESS;
}

PixelQC::MCFlags PixelQC::getHistories( const LHCb::RichSmartID id ) const
{
  // make a new flag object
  MCFlags flags;

  // get MC histories for this RichSmartID
  typedef std::vector<const LHCb::MCRichDigitSummary*> Summaries;
  Summaries summaries;
  m_truth->getMcHistories( id, summaries );
  // loop over summaries and set various flags
  for ( Summaries::const_iterator iS = summaries.begin();
        iS != summaries.end(); ++iS )
  {
    if ( (*iS)->history().isBackground() )
    {
      flags.isBkg = true;
      if ( (*iS)->history().hpdQuartzCK()      ) { flags.isHPDQCK        = true; }
      if ( (*iS)->history().gasQuartzCK()      ) { flags.isGasCK         = true; }
      if ( (*iS)->history().nitrogenCK()       ) { flags.isN2CK          = true; }
      if ( (*iS)->history().chargedTrack()     ) { flags.isChargedTk     = true; }
      if ( (*iS)->history().chargeShareHit()   ) { flags.isChargeShare   = true; }
      if ( (*iS)->history().aeroFilterCK()     ) { flags.isAeroFiltCK    = true; }
      if ( (*iS)->history().hpdSiBackscatter() ) { flags.isSiBackScatter = true; }
      if ( (*iS)->history().hpdReflection()    ) { flags.isHPDIntReflect = true; }
      if ( (*iS)->history().radScintillation() ) { flags.isRadScint      = true; }
    }
    else
    {
      flags.isSignal = true;
      if ( (*iS)->history().aerogelHit()   ) { flags.isAerogelCK  = true; }
      if ( (*iS)->history().c4f10Hit()     ) { flags.isRich1GasCK = true; }
      if ( (*iS)->history().cf4Hit()       ) { flags.isRich2GasCK = true; }
    }
  }

  // return the flags
  return flags;
}

//  Finalize
StatusCode PixelQC::finalize()
{

  if ( m_nEvts > 0 )
  {
    info() << "================================================================================" << endmsg
           << "                         Pixel summary for " << m_nEvts << " events :-" << endmsg
           << "--------------------------------------------------------------------------------" << endmsg;
    printRICH(Rich::Rich1);
    info() << "--------------------------------------------------------------------------------" << endmsg;
    printRICH(Rich::Rich2);
    info() << "================================================================================" << endmsg;
  }

  // Execute base class method
  return HistoAlgBase::finalize();
}

void PixelQC::printRICH( const Rich::DetectorType rich ) const
{
  const StatDivFunctor     occ  ("%8.2f +-%7.2f");
  const PoissonEffFunctor  pois ("%7.2f +-%6.2f");

  info() << "  " << rich << " : All pixels          : " << occ(m_recoTally.pixels[rich],m_nEvts)
         << "   Eff. = " << pois(m_recoTally.pixels[rich],m_rawTally.pixels[rich]) << " %" << endmsg;

  if ( m_recoTally.signal[rich] > 0 )
  {
    info() << "        : Cherenkov Signal    : " << occ(m_recoTally.signal[rich],m_nEvts)
           << "   Eff. = " << pois(m_recoTally.signal[rich],m_rawTally.signal[rich]) << " %" << endmsg;
  }

  if ( Rich::Rich1 == rich )
  {
    if ( m_recoTally.radHits[Rich::Aerogel] > 0 )
    {
      info() << "        :     Aerogel         : " << occ(m_recoTally.radHits[Rich::Aerogel],m_nEvts)
             << "   Eff. = " << pois(m_recoTally.radHits[Rich::Aerogel],m_rawTally.radHits[Rich::Aerogel])
             << " %" << endmsg;
    }
    if ( m_recoTally.radHits[Rich::Rich1Gas] > 0 )
    {
      info() << "        :     Rich1Gas        : " << occ(m_recoTally.radHits[Rich::Rich1Gas],m_nEvts)
             << "   Eff. = " << pois(m_recoTally.radHits[Rich::Rich1Gas],m_rawTally.radHits[Rich::Rich1Gas])
             << " %" << endmsg;
    }
  }
  else
  {
    if ( m_recoTally.radHits[Rich::Rich2Gas] > 0 )
    {
      info() << "        :     Rich2Gas        : " << occ(m_recoTally.radHits[Rich::Rich2Gas],m_nEvts)
             << "   Eff. = " << pois(m_recoTally.radHits[Rich::Rich2Gas],m_rawTally.radHits[Rich::Rich2Gas])
             << " %" << endmsg;
    }
  }

  if ( m_recoTally.bkgs[rich] > 0 )
  {

    info() << "        : All Backgrounds     : " << occ(m_recoTally.bkgs[rich],m_nEvts)
           << "   Eff. = " << pois(m_recoTally.bkgs[rich],m_rawTally.bkgs[rich]) << " %" << endmsg;

    if ( m_rawTally.npdqcks[rich] > 0 )
      info() << "        :   - HPD Quartz CK   : " << occ(m_recoTally.npdqcks[rich],m_nEvts)
             << "   Eff. = " << pois(m_recoTally.npdqcks[rich],m_rawTally.npdqcks[rich]) << " %" << endmsg;

    if ( m_rawTally.ngasck[rich] > 0 )
      info() << "        :   - Gas Quartz CK   : " << occ(m_recoTally.ngasck[rich],m_nEvts)
             << "   Eff. = " << pois(m_recoTally.ngasck[rich],m_rawTally.ngasck[rich]) << " %" << endmsg;

    if ( m_rawTally.n2ck[rich] > 0 )
      info() << "        :   - N2 CK           : " << occ(m_recoTally.n2ck[rich],m_nEvts)
             << "   Eff. = " << pois(m_recoTally.n2ck[rich],m_rawTally.n2ck[rich]) << " %" << endmsg;

    if ( m_rawTally.naerofilter[rich] > 0 )
      info() << "        :   - Aero. filter CK : " << occ(m_recoTally.naerofilter[rich],m_nEvts)
             << "   Eff. = " << pois(m_recoTally.naerofilter[rich],m_rawTally.naerofilter[rich]) << " %" << endmsg;

    if ( m_rawTally.nbackscatter[rich] > 0 )
      info() << "        :   - Si Back-Scatter : " << occ(m_recoTally.nbackscatter[rich],m_nEvts)
             << "   Eff. = " << pois(m_recoTally.nbackscatter[rich],m_rawTally.nbackscatter[rich]) << " %" << endmsg;

    if ( m_rawTally.nhpdintreflect[rich] > 0 )
      info() << "        :   - HPD Reflections : " << occ(m_recoTally.nhpdintreflect[rich],m_nEvts)
             << "   Eff. = " << pois(m_recoTally.nhpdintreflect[rich],m_rawTally.nhpdintreflect[rich]) << " %" << endmsg;

    if ( m_rawTally.ntrack[rich] > 0 )
      info() << "        :   - Track On HPD    : " << occ(m_recoTally.ntrack[rich],m_nEvts)
             << "   Eff. = " << pois(m_recoTally.ntrack[rich],m_rawTally.ntrack[rich]) << " %" << endmsg;

    if ( m_rawTally.nchargeshare[rich] > 0 )
      info() << "        :   - Si Charge Share : " << occ(m_recoTally.nchargeshare[rich],m_nEvts)
             << "   Eff. = " << pois(m_recoTally.nchargeshare[rich],m_rawTally.nchargeshare[rich]) << " %" << endmsg;

    if ( m_rawTally.radScint[rich] > 0 )
      info() << "        :   - Scintillation   : " << occ(m_recoTally.radScint[rich],m_nEvts)
             << "   Eff. = " << pois(m_recoTally.radScint[rich],m_rawTally.radScint[rich]) << " %" << endmsg;

  }

}
