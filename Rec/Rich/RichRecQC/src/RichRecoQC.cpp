// $Id: RichRecoQC.cpp,v 1.7 2003-11-02 21:47:22 jonrob Exp $

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
StatusCode RichRecoQC::initialize() {

  MsgStream msg(msgSvc(), name());

  // Sets up various tools and services
  if ( !RichRecAlgBase::initialize() ) return StatusCode::FAILURE;

  // acquire tools
  acquireTool( "RichParticleProperties", m_richPartProp );
  acquireTool( "RichCherenkovAngle",     m_ckAngle      );
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );

  // Book histograms
  if ( !bookHistograms() || !bookMCHistograms() ) return StatusCode::FAILURE;

  msg << MSG::DEBUG << "Initialize" << endreq
      << " Histogram location     = " << m_histPth << endreq;
  msg << " MC Histogram location  = " << m_mcHistPth << endreq;

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

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Execute" << endreq;

  // Event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg ) {
    RichRecSegment * segment = *iSeg;

    // Radiator info
    Rich::RadiatorType rad = segment->trackSegment().radiator();

    // True particle type
    Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
    if ( Rich::Unknown == mcType ) continue; // skip tracks with unknown MC type

    // beta for true type
    double beta = m_richPartProp->beta( segment, mcType );
    if ( beta < m_minBeta ) continue; // skip non-saturated tracks

    // Expected Cherenkov theta angle for true particle type
    double thetaExpTrue = ( mcType != Rich::Unknown ?
                            m_ckAngle->avgCherenkovTheta( segment, mcType ) : 0 );

    // loop over photons for this segment
    int truePhotons = 0;
    for ( RichRecSegment::Photons::iterator iPhot = segment->richRecPhotons().begin();
          iPhot != segment->richRecPhotons().end(); ++iPhot ) {
      RichRecPhoton * photon = *iPhot;

      double thetaRec = photon->geomPhoton().CherenkovTheta();

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
StatusCode RichRecoQC::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  releaseTool( m_richRecMCTruth );
  releaseTool( m_richPartProp );
  releaseTool( m_ckAngle );

  // Execute base class method
  return RichRecAlgBase::finalize();
}



