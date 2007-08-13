
//-----------------------------------------------------------------------------
/** @file RichRecPixelQC.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::MC::PixelQC
 *
 *  $Id: RichRecPixelQC.cpp,v 1.15 2007-08-13 12:39:23 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#include <fstream>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichRecPixelQC.h"

// working namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PixelQC );

// Standard constructor, initializes variables
PixelQC::PixelQC( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : Rich::Rec::HistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( NULL ),
    m_truth             ( NULL ),
    m_richSys           ( NULL ),
    m_nEvts             ( 0    ) { }

// Destructor
PixelQC::~PixelQC() {};

//  Initialize
StatusCode PixelQC::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth   );
  acquireTool( "RichMCTruthTool",    m_truth );
  acquireTool( "RichSmartIDDecoder", m_decoder, 0, true );

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocation::RichSystem );

  return sc;
}

// Main execution
StatusCode PixelQC::execute()
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

  std::vector<unsigned int> pixels ( Rich::NRiches, 0 );
  std::vector<unsigned int> signal ( Rich::NRiches, 0 );

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
        const LHCb::RichSmartID hpd = (*iHPD).first;
        // Vector of SmartIDs
        const LHCb::RichSmartID::Vector & rawIDs = (*iHPD).second.smartIDs();
        // RICH
        const Rich::DetectorType rich = hpd.rich();

        // Loop over raw RichSmartIDs
        for ( LHCb::RichSmartID::Vector::const_iterator iR = rawIDs.begin();
              iR != rawIDs.end(); ++iR )
        {
          // flags
          const MCFlags flags = getHistories( *iR );
          // count
          ++m_rawTally.pixels[rich];
          if ( flags.isBkg        ) { ++m_rawTally.bkgs[rich]; }
          if ( flags.isHPDQCK     ) { ++m_rawTally.npdqcks[rich]; }
          if ( flags.isGasCK      ) { ++m_rawTally.ngasck[rich]; }
          if ( flags.isN2CK       ) { ++m_rawTally.n2ck[rich]; }
          if ( flags.isChargedTk  ) { ++m_rawTally.ntrack[rich]; }
          if ( flags.isChargeShare) { ++m_rawTally.nchargeshare[rich]; }
          if ( flags.isAeroFiltCK ) { ++m_rawTally.naerofilter[rich]; }
          if ( flags.isSignal     ) { ++m_rawTally.signal[rich]; }
          if ( flags.isAerogelCK  ) { ++m_rawTally.radHits[Rich::Aerogel]; }
          if ( flags.isRich1GasCK ) { ++m_rawTally.radHits[Rich::Rich1Gas]; }
          if ( flags.isRich2GasCK ) { ++m_rawTally.radHits[Rich::Rich2Gas]; }
        } // raw channel ids

        // Get the reconstructed pixels for this HPD
        IPixelCreator::PixelRange range = pixelCreator()->range(hpd);
        unsigned int nHPDHits(0), nHPDSignalHits(0);
        // loop over reconstructed pixels
        for ( IPixelCreator::PixelRange::const_iterator iPixel = range.begin();
              iPixel != range.end(); ++iPixel )
        {
          // for each pixel loop over the smartIDs
          for ( LHCb::RichSmartID::Vector::const_iterator iS = (*iPixel)->hpdPixelCluster().smartIDs().begin();
                iS != (*iPixel)->hpdPixelCluster().smartIDs().end(); ++iS )
          {
            // flags
            const MCFlags flags = getHistories( *iS );
            // count
            ++nHPDHits;
            ++pixels[rich];
            ++m_recoTally.pixels[rich];
            if ( flags.isBkg        ) { ++m_recoTally.bkgs[rich]; }
            if ( flags.isHPDQCK     ) { ++m_recoTally.npdqcks[rich]; }
            if ( flags.isGasCK      ) { ++m_recoTally.ngasck[rich]; }
            if ( flags.isN2CK       ) { ++m_recoTally.n2ck[rich]; }
            if ( flags.isChargedTk  ) { ++m_recoTally.ntrack[rich]; }
            if ( flags.isChargeShare) { ++m_recoTally.nchargeshare[rich]; }
            if ( flags.isAeroFiltCK ) { ++m_recoTally.naerofilter[rich]; }
            if ( flags.isSignal     ) { ++nHPDSignalHits; ++signal[rich]; ++m_recoTally.signal[rich]; }
            if ( flags.isAerogelCK  ) { ++m_recoTally.radHits[Rich::Aerogel]; }
            if ( flags.isRich1GasCK ) { ++m_recoTally.radHits[Rich::Rich1Gas]; }
            if ( flags.isRich2GasCK ) { ++m_recoTally.radHits[Rich::Rich2Gas]; }
          }
        }

        plot1D( nHPDHits,       hid(rich,"nTotalPixsPerHPD"),  "Average overall HPD occupancy (nHits>0)", 0, 150, 75 );
        plot1D( nHPDSignalHits, hid(rich,"nSignalPixsPerHPD"), "Average signal HPD occupancy (nHits>0)", 0, 150, 75 );

      } // loop over HPDs
    } // ingresses
  } // L1 boards

  plot1D( pixels[Rich::Rich1], hid(Rich::Rich1,"nTotalPixs"), "Overall occupancy (nHits>0)", 0, 5000, 100 );
  plot1D( pixels[Rich::Rich2], hid(Rich::Rich2,"nTotalPixs"), "Overall occupancy (nHits>0)", 0, 2000, 100 );
  plot1D( signal[Rich::Rich1], hid(Rich::Rich1,"nSignalPixs"), "Signal occupancy (nHits>0)", 0, 5000, 100 );
  plot1D( signal[Rich::Rich2], hid(Rich::Rich2,"nSignalPixs"), "Signal occupancy (nHits>0)", 0, 2000, 100 );

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
      flags.isBkg    = true;
      if ( (*iS)->history().hpdQuartzCK()    ) { flags.isHPDQCK = true; }
      if ( (*iS)->history().gasQuartzCK()    ) { flags.isGasCK  = true; }
      if ( (*iS)->history().nitrogenCK()     ) { flags.isN2CK   = true; }
      if ( (*iS)->history().chargedTrack()   ) { flags.isChargedTk   = true; }
      if ( (*iS)->history().chargeShareHit() ) { flags.isChargeShare = true; }
      if ( (*iS)->history().aeroFilterCK()   ) { flags.isAeroFiltCK = true; }
    }
    else
    {
      flags.isSignal = true;
      if ( (*iS)->history().aerogelHit()   ) { flags.isAerogelCK = true;  }
      if ( (*iS)->history().c4f10Hit()     ) { flags.isRich1GasCK = true; }
      if ( (*iS)->history().cf4Hit()       ) { flags.isRich2GasCK = true; }
      // requires new MCEvent
      //if ( (*iS)->history().rich1GasHit()  ) { flags.isRich1GasCK = true; }
      //if ( (*iS)->history().rich2GasHit()  ) { flags.isRich2GasCK = true; }
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
    info() << "================================================================================" << endreq
           << "                         Pixel summary for " << m_nEvts << " events :-" << endreq
           << "--------------------------------------------------------------------------------" << endreq;
    printRICH(Rich::Rich1);
    info() << "--------------------------------------------------------------------------------" << endreq;
    printRICH(Rich::Rich2);
    info() << "================================================================================" << endreq;
  }

  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}

void PixelQC::printRICH( const Rich::DetectorType rich ) const
{
  const StatDivFunctor     occ  ("%8.2f +-%7.2f");
  const PoissonEffFunctor  pois ("%7.2f +-%6.2f");

  info() << "  " << rich << " : All pixels          : " << occ(m_recoTally.pixels[rich],m_nEvts)
         << "   Eff. = " << pois(m_recoTally.pixels[rich],m_rawTally.pixels[rich]) << " %" << endreq;

  info() << "        : Cherenkov Signal    : " << occ(m_recoTally.signal[rich],m_nEvts)
         << "   Eff. = " << pois(m_recoTally.signal[rich],m_rawTally.signal[rich]) << " %" << endreq;

  if ( Rich::Rich1 == rich )
  {
    info() << "        :     Aerogel         : " << occ(m_recoTally.radHits[Rich::Aerogel],m_nEvts)
           << "   Eff. = " << pois(m_recoTally.radHits[Rich::Aerogel],m_rawTally.radHits[Rich::Aerogel])
           << " %" << endreq;
    info() << "        :     Rich1Gas        : " << occ(m_recoTally.radHits[Rich::Rich1Gas],m_nEvts)
           << "   Eff. = " << pois(m_recoTally.radHits[Rich::Rich1Gas],m_rawTally.radHits[Rich::Rich1Gas])
           << " %" << endreq;
  }
  else
  {
    info() << "        :     Rich2Gas        : " << occ(m_recoTally.radHits[Rich::Rich2Gas],m_nEvts)
           << "   Eff. = " << pois(m_recoTally.radHits[Rich::Rich2Gas],m_rawTally.radHits[Rich::Rich2Gas])
           << " %" << endreq;
  }

  info() << "        : All Backgrounds     : " << occ(m_recoTally.bkgs[rich],m_nEvts)
         << "   Eff. = " << pois(m_recoTally.bkgs[rich],m_rawTally.bkgs[rich]) << " %" << endreq;

  if ( m_rawTally.npdqcks[rich] > 0 )
    info() << "        :   - HPD Quartz CK   : " << occ(m_recoTally.npdqcks[rich],m_nEvts)
           << "   Eff. = " << pois(m_recoTally.npdqcks[rich],m_rawTally.npdqcks[rich]) << " %" << endreq;

  if ( m_rawTally.ngasck[rich] > 0 )
    info() << "        :   - Gas Quartz CK   : " << occ(m_recoTally.ngasck[rich],m_nEvts)
           << "   Eff. = " << pois(m_recoTally.ngasck[rich],m_rawTally.ngasck[rich]) << " %" << endreq;

  if ( m_rawTally.n2ck[rich] > 0 )
    info() << "        :   - N2 CK           : " << occ(m_recoTally.n2ck[rich],m_nEvts)
           << "   Eff. = " << pois(m_recoTally.n2ck[rich],m_rawTally.n2ck[rich]) << " %" << endreq;

  if ( m_rawTally.naerofilter[rich] > 0 )
    info() << "        :   - Aero. filter CK : " << occ(m_recoTally.naerofilter[rich],m_nEvts)
           << "   Eff. = " << pois(m_recoTally.naerofilter[rich],m_rawTally.naerofilter[rich]) << " %" << endreq;

  if ( m_rawTally.ntrack[rich] > 0 )
    info() << "        :   - Track On HPD    : " << occ(m_recoTally.ntrack[rich],m_nEvts)
           << "   Eff. = " << pois(m_recoTally.ntrack[rich],m_rawTally.ntrack[rich]) << " %" << endreq;

  if ( m_rawTally.nchargeshare[rich] > 0 )
    info() << "        :   - Si Charge Share : " << occ(m_recoTally.nchargeshare[rich],m_nEvts)
           << "   Eff. = " << pois(m_recoTally.nchargeshare[rich],m_rawTally.nchargeshare[rich]) << " %" << endreq;

}
