
//---------------------------------------------------------------------------
/** @file RichPhotonSignalMonitor.cpp
 *
 *  Implementation file for algorithm class : PhotonSignalMonitor
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichPhotonSignalMonitor.h"

// namespace
using namespace Rich::Rec::MC;

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PhotonSignalMonitor )

// Standard constructor, initializes variables
PhotonSignalMonitor::PhotonSignalMonitor( const std::string& name,
                                          ISvcLocator* pSvcLocator )
  : HistoAlgBase        ( name, pSvcLocator ),
    m_richRecMCTruth    ( NULL ),
    m_tkSignal          ( NULL ),
    m_geomEffic         ( NULL ),
    m_refIndex          ( NULL ),
    m_trSelector        ( NULL )
{
  // job opts
}

// Destructor
PhotonSignalMonitor::~PhotonSignalMonitor() {}

//  Initialize
StatusCode PhotonSignalMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  acquireTool( "RichExpectedTrackSignal", m_tkSignal    );
  acquireTool( "RichGeomEff",          m_geomEffic      );
  acquireTool( "RichTrackEffectiveRefIndex", m_refIndex );
  acquireTool( "TrackSelector",      m_trSelector, this );

  // return
  return sc;
}

// Main execution
StatusCode PhotonSignalMonitor::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Histogramming
  const Rich::HistoID hid;
  //            Radiator          Aerogel  Rich1Gas  Rich2Gas
  const double minPhotEn[]    = { 1.5,     1.5,      1.5     };
  const double maxPhotEn[]    = { 4.5,     7,        7.5     };
  const double minRefInd[]    = { 1.0280,  1.00128,  1.00039 };
  const double maxRefInd[]    = { 1.0312,  1.00150,  1.00046 };

  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment * segment = *iSeg;

    // apply track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // MC type
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
    if ( mcType == Rich::Unknown ) continue;

    // Loop over all particle codes
    for ( Particles::const_iterator hypo = particles().begin();
          hypo != particles().end(); ++hypo )
    {
      // Expected number of emitted photons
      const double emitPhots = m_tkSignal->nEmittedPhotons( segment, *hypo );
      plot1D( emitPhots, hid(rad,*hypo,"nEmitPhots"), "Expected # emitted photons", 0, 100 );

      // Expected number of observable signal photons
      const double sigPhots = m_tkSignal->nObservableSignalPhotons( segment, *hypo );
      plot1D( sigPhots, hid(rad,*hypo,"nSigPhots"), "Expected # signal photons", 0,  100 );

      // Geometrical Efficiency
      const double geomEff = m_geomEffic->geomEfficiency(segment,*hypo);
      plot1D( geomEff, hid(rad,*hypo,"geomEff"), "Geom. Eff.", 0, 1 );

      // Scattered Geometrical Efficiency
      if ( Rich::Aerogel == rad )
      {
        const double geomEffScat = m_geomEffic->geomEfficiencyScat(segment,*hypo);
        plot1D( geomEffScat, hid(rad,*hypo,"geomEffScat"), "Scattered Geom. Eff.", 0, 1 );
      }

      // Plot the expected spectra (energy, refIndex etc.)
      const PhotonSpectra<LHCb::RichRecSegment::FloatType> & spectra 
        = segment->signalPhotonSpectra();
      // min and max ref index values
      double minRefIn = m_refIndex->refractiveIndex( segment, spectra.minEnergy() );
      double maxRefIn = m_refIndex->refractiveIndex( segment, spectra.maxEnergy() );
      if ( minRefIn>maxRefIn )
      {
        const double tmp = minRefIn;
        minRefIn = maxRefIn;
        maxRefIn = tmp;
      }
      for ( unsigned int iEnBin = 0; iEnBin < spectra.energyBins(); ++iEnBin )
      {
        // photon energy for this bin
        const double energy = spectra.binEnergy(iEnBin);
        // ref index for this energy bin
        const double refInd = m_refIndex->refractiveIndex( segment, energy );
        // energy spectra
        plot1D( energy, hid(rad,*hypo,"energySpectra"), "Photon energy spectra",
                spectra.minEnergy(), spectra.maxEnergy(), spectra.energyBins(), 
                (spectra.energyDist(*hypo))[iEnBin] );
        // ref index spectra
        plot1D( refInd, hid(rad,*hypo,"refIndexSpectra"), "Refractive index spectra",
                minRefIn, maxRefIn, spectra.energyBins(), 
                (spectra.energyDist(*hypo))[iEnBin] );
      }

    }

    // Average energy of emitted cherenkov photons for true type
    const double avgEnEmit = m_tkSignal->avgEmitPhotEnergy( segment, mcType );
    plot1D( avgEnEmit, hid(rad,mcType,"emitPhotEnTrue"),
            "Av. energy of emitted photons - True type", minPhotEn[rad], maxPhotEn[rad] );

    // Average energy of signal cherenkov photons for true type
    const double avgEnSig = m_tkSignal->avgSignalPhotEnergy( segment, mcType );
    plot1D( avgEnSig, hid(rad,mcType,"sigPhotEnTrue"),
            "Av. energy of signal photons - True type", minPhotEn[rad], maxPhotEn[rad] );

    // refractive index
    const double refInd = m_refIndex->refractiveIndex( segment, avgEnEmit );
    plot1D( refInd-1, hid(rad,mcType,"refIndM1"),
            "Refractive index n-1", minRefInd[rad]-1, maxRefInd[rad]-1 );

    // Number of signal photons for true type
    const double nSigTrue = m_tkSignal->nSignalPhotons( segment, mcType );
    plot1D( nSigTrue, hid(rad,mcType,"nSigPhotsTrue"),
            "Expected # signal photons - true type", 0,  100 );

  }

  return StatusCode::SUCCESS;
}
