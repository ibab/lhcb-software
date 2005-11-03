
//---------------------------------------------------------------------------------
/** @file RichRecoQC.cpp
 *
 *  Implementation file for RICH reconstruction monitoring algorithm : RichRecoQC
 *
 *  CVS Log :-
 *  $Id: RichRecoQC.cpp,v 1.19 2005-11-03 14:37:34 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//---------------------------------------------------------------------------------

// local
#include "RichRecoQC.h"

//---------------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichRecoQC>          s_factory ;
const        IAlgFactory& RichRecoQCFactory = s_factory ;

// Standard constructor, initializes variables
RichRecoQC::RichRecoQC( const std::string& name,
                        ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richPartProp      ( 0 ),
    m_ckAngle           ( 0 ),
    m_richRecMCTruth    ( 0 ),
    m_truePhotCount     ( Rich::NRadiatorTypes, 0 ),
    m_nSegs             ( Rich::NRadiatorTypes, 0 )
{
  // Declare job options
  // min beta
  declareProperty( "MinBeta",     m_minBeta   = 0.999 );
  // track selector
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );
  declareProperty( "TrackMomentumCuts", m_trSelector.setMomentumCuts() );
}

// Destructor
RichRecoQC::~RichRecoQC() {};

// Initialisation
StatusCode RichRecoQC::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // acquire tools
  acquireTool( "RichParticleProperties", m_richPartProp );
  acquireTool( "RichCherenkovAngle",      m_ckAngle     );
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  acquireTool( "RichCherenkovResolution", m_ckRes       );

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes() ) 
    return Error( "Problem configuring track selection" );
  m_trSelector.printTrackSelection( info() );

  return sc;
}

// Main execution
StatusCode RichRecoQC::execute()
{
  debug() << "Execute" << endreq;

  // Event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Rich Histo ID
  const RichHistoID hid;

  // Histo ranges               Aero   C4F10  CF4
  const double ckResRange[] = { 0.015, 0.01,  0.005 };

  // Make sure all tracks and segments have been formed
  if ( !trackCreator()->newTracks() ) 
    return Error( "Problem creating RichRecTracks" );

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    RichRecSegment * segment = *iSeg;

    // track selection
    if ( !m_trSelector.trackSelected(segment->richRecTrack()) ) continue;

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // True particle type
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
    if ( Rich::Unknown  == mcType ) continue; // skip tracks with unknown MC type
    if ( Rich::Electron == mcType ) continue; // skip electrons which are reconstructed badly..

    // segment momentum
    const double pTot = segment->trackSegment().bestMomentum().mag();

    // beta for true type
    const double beta = m_richPartProp->beta( pTot, mcType );
    if ( beta < m_minBeta ) continue; // skip non-saturated tracks

    // Expected Cherenkov theta angle for true particle type
    const double thetaExpTrue = m_ckAngle->avgCherenkovTheta( segment, mcType );

    // Cherenkov angle resolution for true type
    const double resExpTrue = m_ckRes->ckThetaResolution( segment, mcType );

    // loop over photons for this segment
    unsigned int truePhotons = 0;
    for ( RichRecSegment::Photons::iterator iPhot = segment->richRecPhotons().begin();
          iPhot != segment->richRecPhotons().end(); ++iPhot )
    {
      RichRecPhoton * photon = *iPhot;

      // reconstructed theta
      const double thetaRec = photon->geomPhoton().CherenkovTheta();

      // Is this a true photon ?
      const MCParticle * photonParent = m_richRecMCTruth->trueCherenkovPhoton( photon );
      if ( photonParent )
      {
        ++truePhotons;
        // resolution plot
        plot1D( thetaRec-thetaExpTrue,
                hid(rad,"ckRes"), "Rec-Exp Cktheta : beta=1", -ckResRange[rad], ckResRange[rad] );
        if ( resExpTrue>0 )
        {
          // pull plot
          const double ckPull = (thetaRec-thetaExpTrue)/resExpTrue;
          plot1D( ckPull, hid(rad,"ckPull"), "(Rec-Exp)/Res Cktheta : beta=1", -5, 5 );     
        }

      }

    } // photon loop

    // number of true photons
    if ( truePhotons > 0 )
    {
      plot1D( truePhotons, hid(rad,"nCKphots"), "True # p.e.s : beta=1", -0.5, 50, 51 );
      m_truePhotCount[rad] += truePhotons;
      ++m_nSegs[rad];
    }

  } // end loop over segments

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichRecoQC::finalize()
{

  // statistical tool
  const RichStatDivFunctor occ("%10.2f +-%7.2f");

  info() << "=============================================================================="
         << endreq;

  // track selection
  info() << "Track Selection : " << m_trSelector.selectedTracksAsString() << endreq;

  // print out of photon counts
  if ( m_truePhotCount[Rich::Aerogel]>0 )
  {
    info() << "Aerogel  Av. # CK photons = "
           << occ(m_truePhotCount[Rich::Aerogel],m_nSegs[Rich::Aerogel]) << " photons/segment" << endreq;
  }
  if ( m_truePhotCount[Rich::C4F10]>0 )
  {
    info() << "C4F10    Av. # CK photons = "
           << occ(m_truePhotCount[Rich::C4F10],m_nSegs[Rich::C4F10]) << " photons/segment" << endreq;
  }
  if ( m_truePhotCount[Rich::CF4]>0 )
  {
    info() << "CF4      Av. # CK photons = "
           << occ(m_truePhotCount[Rich::CF4],m_nSegs[Rich::CF4]) << " photons/segment" << endreq;
  }

  info() << "=============================================================================="
         << endreq;

  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
