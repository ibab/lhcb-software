
//---------------------------------------------------------------------------
/** @file RichCherenkovResMoni.cpp
 *
 *  Implementation file for algorithm class : RichCherenkovResMoni
 *
 *  CVS Log :-
 *  $Id: RichCherenkovResMoni.cpp,v 1.2 2005-10-31 13:30:58 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichCherenkovResMoni.h"

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichCherenkovResMoni>          s_factory ;
const        IAlgFactory& RichCherenkovResMoniFactory = s_factory ;

// Standard constructor, initializes variables
RichCherenkovResMoni::RichCherenkovResMoni( const std::string& name,
                                            ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( 0 ),
    m_ckAngle           ( 0 ),
    m_ckAngleRes        ( 0 )
{
  // track selector
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );
}

// Destructor
RichCherenkovResMoni::~RichCherenkovResMoni() {};

//  Initialize
StatusCode RichCherenkovResMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  acquireTool( "RichCherenkovAngle",      m_ckAngle     );
  acquireTool( "RichCherenkovResolution", m_ckAngleRes  );

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes() ) return StatusCode::FAILURE;
  m_trSelector.printTrackSelection( info() );

  return sc;
}

// Main execution
StatusCode RichCherenkovResMoni::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Histogramming
  const RichHistoID hid;

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    RichRecSegment * segment = *iSeg;

    // apply track selection
    if ( !m_trSelector.trackSelected(segment->richRecTrack()) ) continue;

    // MC type
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
    if ( mcType == Rich::Unknown ) continue;

    // track segment
    const RichTrackSegment & trackSeg = segment->trackSegment();

    // radiator
    const Rich::RadiatorType rad = trackSeg.radiator();

    // Segment momentum
    const double ptot = segment->trackSegment().bestMomentum().mag() / GeV;

    // CK resolution and angle for true type
    const double trueCKres = m_ckAngleRes->ckThetaResolution(segment,mcType);
    const double trueCKang = m_ckAngle->avgCherenkovTheta(segment,mcType);

    // histo range
    const double ckMax[]    = { 0.3, 0.06, 0.04 };
    const double ckResMax[] = { 0.011, 0.011, 0.003 };

    // Loop over all particle codes
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo )
    {
      const Rich::ParticleIDType hypo = static_cast<Rich::ParticleIDType>(iHypo);

      // CK resolution
      const double ckres = m_ckAngleRes->ckThetaResolution(segment,hypo);
      // CK angle
      const double ckang = m_ckAngle->avgCherenkovTheta(segment,hypo);

      if ( ckang > 0 )
      {
        // histograms
        plot1D( ckres, hid(rad,"ckres"), "Calculated CKres", 0, ckResMax[rad] );
        plot2D( ckang, ckres, hid(rad,"ckresVcktheta"), "Calculated CKres V CKangle", 
                0, ckMax[rad], 0, ckResMax[rad] );
        plot2D( ptot, ckres, hid(rad,"ckresVptot"), "Calculated CKres V ptot", 
                0, 100, 0, ckResMax[rad] );
        profile1D( ckang, ckres, hid(rad,"ckresVckangP"), "Calculated CKres V CKangle", 0, ckMax[rad] );
        profile1D( ptot, ckres, hid(rad,"ckresVptotp"), "Calculated CKres V ptot", 0, 100 );
        profile2D( ptot, ckang, ckres, hid(rad,"ckresVptotVckang"), 
                   "Calculated CKres V ptot+ckang", 0, 100, 0, ckMax[rad] );
      }

    } // particle ID codes

    // loop over photons
    const RichRecSegment::Photons & photons = photonCreator()->reconstructPhotons( segment );
    for ( RichRecSegment::Photons::const_iterator iPhot = photons.begin();
          iPhot != photons.end();
          ++iPhot )
    {
      RichRecPhoton * photon = *iPhot;

      // MC photon
      const MCRichOpticalPhoton * mcPhot = m_richRecMCTruth->trueOpticalPhoton(photon);
      if ( mcPhot )
      {
        // Cherenkov angles
        const double thetaRec = photon->geomPhoton().CherenkovTheta();
        const double thetaMC  = mcPhot->cherenkovTheta();
        const double delCK    = thetaRec-thetaMC;

        plot2D( delCK, trueCKres, hid(rad,"trueVcalCKres"), "True V calculated CKres", 
                0, ckResMax[rad], 0, ckResMax[rad] );
        plot2D( trueCKang, delCK, hid(rad,"trueCKresVang"), "True CKres V true CK angle", 
                0, ckMax[rad], 0, ckResMax[rad] );
        profile1D( delCK, trueCKres, hid(rad,"trueVcalCKresP"), "True V calculated CKres", 
                   0, ckResMax[rad] );
        profile1D( trueCKang, delCK, hid(rad,"trueCKresVangP"), "True CKres V true CK angle", 
                   0, ckMax[rad] );
      }

    }

  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichCherenkovResMoni::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
