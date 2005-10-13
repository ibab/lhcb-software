
//---------------------------------------------------------------------------------
/** @file RichRecoQC.cpp
 *
 *  Implementation file for RICH reconstruction monitoring algorithm : RichRecoQC
 *
 *  CVS Log :-
 *  $Id: RichRecoQC.cpp,v 1.14 2005-10-13 15:52:48 jonrob Exp $
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
  : RichRecAlgBase   ( name, pSvcLocator ),
    m_richPartProp   ( 0 ),
    m_ckAngle        ( 0 ),
    m_richRecMCTruth ( 0 ),    
    m_truePhotCount  ( Rich::NRadiatorTypes, 0 ),
    m_nSegs          ( Rich::NRadiatorTypes, 0 )
{

  // Declare job options
  declareProperty( "MCHistoPath", m_mcHistPth = "RICH/RECOQC/MC/" );
  declareProperty( "HistoPath",   m_histPth = "RICH/RECOQC/" );
  declareProperty( "MinBeta",     m_minBeta = 0.999 );

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
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // acquire tools
  acquireTool( "RichParticleProperties", m_richPartProp );
  acquireTool( "RichCherenkovAngle",      m_ckAngle   );
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes() ) return StatusCode::FAILURE;
  m_trSelector.printTrackSelection( info() );

  // Book histograms
  if ( !bookHistograms() || !bookMCHistograms() ) return StatusCode::FAILURE;

  return sc;
}

StatusCode RichRecoQC::bookHistograms() 
{
  return StatusCode::SUCCESS;
}

StatusCode RichRecoQC::bookMCHistograms() 
{

  std::string title;
  int id;

  const double ckRange[] = { 0.015, 0.01, 0.005 };

  // Defines for various parameters
  RAD_HISTO_OFFSET;
  RADIATOR_NAMES;

  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) {

    title = "Rec-Exp Cktheta : beta=1 : " + radiator[iRad];
    id = radOffset*(iRad+1) + 1;
    m_ckTrueDTheta[iRad] = histoSvc()->book(m_mcHistPth,id,title,100,
                                            -ckRange[iRad],ckRange[iRad]);

    title = "True # p.e.s : beta=1 : " + radiator[iRad];
    id = radOffset*(iRad+1) + 2;
    m_trueSignalPhots[iRad] = histoSvc()->book(m_mcHistPth,id,title,51,-0.5,50.5);

  } // end rad loop

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode RichRecoQC::execute() 
{

  // Event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

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
    if ( Rich::Unknown == mcType ) continue; // skip tracks with unknown MC type

    // beta for true type
    const double beta = m_richPartProp->beta( segment->trackSegment().bestMomentum().mag(), mcType );
    if ( beta < m_minBeta ) continue; // skip non-saturated tracks

    // Expected Cherenkov theta angle for true particle type
    const double thetaExpTrue = ( mcType != Rich::Unknown ?
                                  m_ckAngle->avgCherenkovTheta( segment, mcType ) : 0 );

    // loop over photons for this segment
    int truePhotons = 0;
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
        m_ckTrueDTheta[rad]->fill( thetaRec-thetaExpTrue );
      }

    } // photon loop

    // number of true photons
    if ( truePhotons > 0 ) 
    {
      m_trueSignalPhots[rad]->fill( truePhotons );
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
  RichStatDivFunctor occ("%10.2f +-%7.2f");

  // print out of photon counts
  info() << "Aerogel  Av. # CK photons = " 
         << occ(m_truePhotCount[Rich::Aerogel],m_nSegs[Rich::Aerogel]) << " photons/segment" << endreq
         << "C4F10    Av. # CK photons = " 
         << occ(m_truePhotCount[Rich::C4F10],m_nSegs[Rich::C4F10]) << " photons/segment" << endreq
         << "CF4      Av. # CK photons = " 
         << occ(m_truePhotCount[Rich::CF4],m_nSegs[Rich::CF4]) << " photons/segment" << endreq;

  // Execute base class method
  return RichRecAlgBase::finalize();
}
