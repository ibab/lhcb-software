// $Id: RichRecoQC.cpp,v 1.10 2004-07-12 14:32:43 jonrob Exp $

// local
#include "RichRecoQC.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecoQC
//
// 2002-07-02 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichRecoQC>          s_factory ;
const        IAlgFactory& RichRecoQCFactory = s_factory ;


// Standard constructor, initializes variables
RichRecoQC::RichRecoQC( const std::string& name,
                        ISvcLocator* pSvcLocator )
  : RichRecAlgBase ( name, pSvcLocator ) {

  // Declare job options
  declareProperty( "MCHistoPath", m_mcHistPth = "RICH/RECOQC/MC/" );
  declareProperty( "HistoPath", m_histPth = "RICH/RECOQC/" );
  declareProperty( "MinBeta", m_minBeta = 0.99 );

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
  acquireTool( "RichCherenkovAngle",     m_ckAngle      );
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );

  // Book histograms
  if ( !bookHistograms() || !bookMCHistograms() ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
};

StatusCode RichRecoQC::bookHistograms() {
  return StatusCode::SUCCESS;
};

StatusCode RichRecoQC::bookMCHistograms() {

  std::string title;
  int id;

  // Defines for various parameters
  RAD_HISTO_OFFSET;
  RADIATOR_NAMES;

  const double ckRange[] = { 0.015, 0.01, 0.005 };

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
};

// Main execution
StatusCode RichRecoQC::execute() {

  debug() << "Execute" << endreq;

  // Event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg ) {
    RichRecSegment * segment = *iSeg;

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // True particle type
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
    if ( Rich::Unknown == mcType ) continue; // skip tracks with unknown MC type

    // beta for true type
    const double beta = m_richPartProp->beta( segment, mcType );
    if ( beta < m_minBeta ) continue; // skip non-saturated tracks

    // Expected Cherenkov theta angle for true particle type
    const double thetaExpTrue = ( mcType != Rich::Unknown ?
                                  m_ckAngle->avgCherenkovTheta( segment, mcType ) : 0 );

    // loop over photons for this segment
    int truePhotons = 0;
    for ( RichRecSegment::Photons::iterator iPhot = segment->richRecPhotons().begin();
          iPhot != segment->richRecPhotons().end(); ++iPhot ) {
      RichRecPhoton * photon = *iPhot;

      const double thetaRec = photon->geomPhoton().CherenkovTheta();

      // Is this a true photon ?
      const MCParticle * photonParent = m_richRecMCTruth->trueCherenkovPhoton( photon );
      if ( photonParent ) {
        ++truePhotons;
        m_ckTrueDTheta[rad]->fill( thetaRec-thetaExpTrue );
      }

    } // photon loop

    // number of true photons
    if ( truePhotons > 0 ) m_trueSignalPhots[rad]->fill( truePhotons );

  } // end loop over segments

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichRecoQC::finalize() 
{
  // Execute base class method
  return RichRecAlgBase::finalize();
}



