// $Id: RichRecoQC.cpp,v 1.2 2003-07-01 18:44:49 jonrob Exp $
// Include files

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
  declareProperty( "HistoBins", m_bins = 50 );
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
  acquireTool("RichSegmentProperties", m_segProps);

  // Get pointer to MC Truth tool
  if ( !(toolSvc()->retrieveTool("RichRecMCTruthTool",m_richRecMCTruth) ) ) {
    msg << MSG::ERROR << "failed to acquire RichRecMCTruthTool" << endreq;
    return StatusCode::FAILURE;
  }

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

  const double ckRange[]      = { 0.015, 0.01, 0.005 };

  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; iRad++ ) {

    title = "TrueRec-TrueExp Cktheta beta=1 : " + radiator[iRad];
    id = radOffset*(iRad+1) + 1;
    m_ckTrueDTheta[iRad] = histoSvc()->book(m_mcHistPth,id,title,m_bins,
                                            -ckRange[iRad],ckRange[iRad]);

    title = "True # signal photons beta=1 : " + radiator[iRad];
    id = radOffset*(iRad+1) + 2;
    m_trueSignalPhots[iRad] = histoSvc()->book(m_mcHistPth,id,title,m_bins,0,100);

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
        iSeg != richSegments()->end();
        ++iSeg ) {
    RichRecSegment * segment = *iSeg;

    // Radiator info
    Rich::RadiatorType rad = segment->trackSegment().radiator();

    // True particle type
    Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );

    // beta for true type
    double beta = m_segProps->beta( segment, mcType );
    if ( beta < m_minBeta ) continue; // skip non-saturated tracks

    // Expected Cherenkov theta angle for true particle type
    double thetaExpTrue = ( mcType != Rich::Unknown ?
                            m_segProps->avgCherenkovTheta( segment, mcType ) : 0 );

    // loop over photons for this segment
    int truePhotons = 0;
    SmartRefVector<RichRecPhoton> & photons = segment->richRecPhotons();
    for ( SmartRefVector<RichRecPhoton>::iterator iPhot = photons.begin();
          iPhot != photons.end();
          iPhot++ ) {
      RichRecPhoton * photon = *iPhot;

      double thetaRec = photon->geomPhoton().CherenkovTheta();

      // Is this a true photon ?
      const MCParticle * photonParent = m_richRecMCTruth->trueCherenkovPhoton( photon );
      if ( photonParent ) {
        ++truePhotons;
        m_ckTrueDTheta[rad]->fill( thetaRec-thetaExpTrue, 1 );
      }

    } // photon loop

    // number of true photons
    m_trueSignalPhots[rad]->fill( truePhotons, 1 );

  } // end loop over segments

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichRecoQC::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release MC tool
  if ( m_richRecMCTruth ) { toolSvc()->releaseTool( m_richRecMCTruth ); m_richRecMCTruth=0; }

  // Execute base class method
  return RichRecAlgBase::finalize();
}



