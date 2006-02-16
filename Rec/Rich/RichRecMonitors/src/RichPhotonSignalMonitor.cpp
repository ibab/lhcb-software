
//---------------------------------------------------------------------------
/** @file RichPhotonSignalMonitor.cpp
 *
 *  Implementation file for algorithm class : RichPhotonSignalMonitor
 *
 *  CVS Log :-
 *  $Id: RichPhotonSignalMonitor.cpp,v 1.3 2006-02-16 16:07:48 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichPhotonSignalMonitor.h"

// namespace
using namespace LHCb;

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichPhotonSignalMonitor>          s_factory ;
const        IAlgFactory& RichPhotonSignalMonitorFactory = s_factory ;

// Standard constructor, initializes variables
RichPhotonSignalMonitor::RichPhotonSignalMonitor( const std::string& name,
                                                  ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( 0 ),
    m_tkSignal          ( 0 ),
    m_geomEffic         ( 0 ),
    m_refIndex          ( 0 )
{
  // track selector
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );
}

// Destructor
RichPhotonSignalMonitor::~RichPhotonSignalMonitor() {};

//  Initialize
StatusCode RichPhotonSignalMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  acquireTool( "RichExpectedTrackSignal", m_tkSignal    );
  acquireTool( "RichGeomEff",          m_geomEffic      );
  acquireTool( "RichRefractiveIndex",     m_refIndex    );

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes(msg()) )
    return Error( "Problem configuring track selection" );
  m_trSelector.printTrackSelection( info() );

  return sc;
}

// Main execution
StatusCode RichPhotonSignalMonitor::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Histogramming
  const RichHistoID hid;
  //            Radiator          Aerogel  C4F10    CF4
  const double minPhotEn[]    = { 1.5,     1.5,     1.5     };
  const double maxPhotEn[]    = { 4,       7,       7.5     };
  const double minRefInd[]    = { 0.031,   0.0013,  0.00044 };
  const double maxRefInd[]    = { 0.034,   0.00155, 0.00052 };

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    RichRecSegment * segment = *iSeg;

    // apply track selection
    if ( !m_trSelector.trackSelected(segment->richRecTrack()) ) continue;

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // MC type
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
    if ( mcType == Rich::Unknown ) continue;

    // Loop over all particle codes
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo )
    {
      Rich::ParticleIDType hypo = static_cast<Rich::ParticleIDType>(iHypo);

      // Expected number of emitted photons
      const double emitPhots = m_tkSignal->nEmittedPhotons( segment, hypo );
      plot1D( emitPhots, hid(rad,hypo,"nEmitPhots"), "Expected # emitted photons", 0, 100 );

      // Expected number of observable signal photons
      const double sigPhots = m_tkSignal->nObservableSignalPhotons( segment, hypo );
      plot1D( sigPhots, hid(rad,hypo,"nSigPhots"), "Expected # signal photons", 0,  100 );

      // Geometrical Efficiency
      const double geomEff = m_geomEffic->geomEfficiency(segment,hypo);
      plot1D( geomEff, hid(rad,hypo,"geomEff"), "Geom. Eff.", 0, 1 );

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
    const double refInd = m_refIndex->refractiveIndex( rad, avgEnEmit );
    plot1D( refInd-1, hid(rad,mcType,"refIndM1"),
            "Refractive index n-1", minRefInd[rad], maxRefInd[rad] );

    // Number of signal photons for true type
    const double nSigTrue = m_tkSignal->nSignalPhotons( segment, mcType );
    plot1D( nSigTrue, hid(rad,mcType,"nSigPhotsTrue"),
            "Expected # signal photons - true type", 0,  100 );

  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichPhotonSignalMonitor::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
