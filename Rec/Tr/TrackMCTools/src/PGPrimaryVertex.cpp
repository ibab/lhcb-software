// Include files:
// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"
#include "Linker/LinkedTo.h"
#include "Event/MCTrackInfo.h"
// Local
#include "PGPrimaryVertex.h"



//-----------------------------------------------------------------------------
// Implementation file for class : PGPrimaryVertex
//
// 2008-12-07 : Marcin Kucharczyk
// 2014-08-01 : Improved by Dominik Mitzel
//-----------------------------------------------------------------------------
DECLARE_ALGORITHM_FACTORY(PGPrimaryVertex)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PGPrimaryVertex::PGPrimaryVertex(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name,pSvcLocator)
  , m_outputVertices(NULL)
{
  declareProperty("OutputVerticesName", m_outputVerticesName = LHCb::RecVertexLocation::Primary);
  declareProperty("UsePVResFile", m_usePVResFile = false);
  declareProperty("InputFileName", m_inputFileName = "");
  declareProperty("HistogramPath", m_histoPath = "h3_resovxvyvz");
}

//=============================================================================
// Destructor
//=============================================================================
PGPrimaryVertex::~PGPrimaryVertex() {
}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PGPrimaryVertex::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc) return sc;
  if(msgLevel(MSG::DEBUG)) debug() << "Initialisation" << endmsg;

  if( m_usePVResFile ){
    // -- Open the file containing the PV resolution
    m_file = TFile::Open( m_inputFileName.c_str() );
    if( !m_file ){
      error() << "Could not find PV resolution template file!" << endmsg;
      return StatusCode::FAILURE;
    }
    m_hist = (TH3D*) m_file->Get( m_histoPath.c_str() );
    if( !m_hist ){
      error() << "Could not find PV resolution template histogram!" << endmsg;
      return StatusCode::FAILURE;
    }
  } else {
    // -- Initialize the random number generator
    sc = m_gaus.initialize(randSvc(), Rndm::Gauss(0., 1.));
    if (!sc) {
      error() << "Cannot initialize Gaussian random number generator." << endmsg;
      return sc;
    }
    sc = m_flat.initialize(randSvc(), Rndm::Flat(0., 1.));
    if (!sc) {
      error() << "Cannot initialize Uniform random number generator." << endmsg;
      return sc;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Execution
//=============================================================================
StatusCode PGPrimaryVertex::execute() {

  if(msgLevel(MSG::DEBUG)) debug() << "Execute" << endmsg;

  m_outputVertices = new LHCb::RecVertices();
  put(m_outputVertices,m_outputVerticesName);

  // find a visible MC primary vertex
  double x(0.), y(0.), z(0.);
  bool found(false);
  LHCb::MCVertices* mcVertices = get<LHCb::MCVertices>(LHCb::MCVertexLocation::Default);
  for(LHCb::MCVertices::const_iterator itMCV = mcVertices->begin();  mcVertices->end() != itMCV; itMCV++) {
     const LHCb::MCVertex* mcv = *itMCV ;
     if ( 0 == mcv || !mcv->isPrimary() ) continue;
     x = mcv->position().x();
     y = mcv->position().y();
     z = mcv->position().z();
     found = true;
     break;
  }
  if (!found) {
     error() << "Could not find any PV in the event" << endmsg;
     return StatusCode::FAILURE;
  }

  // calculate smearing
  double smearX(0.), smearY(0.), smearZ(0.);
  if( m_usePVResFile ) {
     // -- sample components of PV resolution according to template in histogram
     m_hist->GetRandom3(smearX, smearY, smearZ);
  } else {
     // -- use parameterization of PV resolution
     double rx = m_flat();
     if (rx < 0.4086)
        smearX = m_gaus()*2.14809e-02 - 1.64321e-04;
     else if (rx < 0.7969)
        smearX = m_gaus()*9.28222e-03 - 2.07447e-05;
     else
        smearX = m_gaus()*5.53627e-02 - 1.59372e-04;
     double ry = m_flat();
     if (ry < 0.4035)
        smearY = m_gaus()*2.12752e-02 - 3.54945e-05;
     else if (ry < 0.8039)
        smearY = m_gaus()*9.42666e-03 - 1.92725e-05;
     else
        smearY = m_gaus()*5.50516e-02 - 7.60356e-05;
     double rz = m_flat();
     if (rz < 0.1628)
        smearZ = m_gaus()*2.93170e-01 + 1.92948e-03;
     else if (rz < 0.5597)
        smearZ = m_gaus()*4.58140e-02 - 6.39393e-05;
     else
        smearZ = m_gaus()*1.10641e-01 + 4.43561e-04;
  }

  // smear the MC primary vertex
  LHCb::RecVertex* cheatedPV = new LHCb::RecVertex();
  cheatedPV->clearTracks();
  Gaudi::XYZPoint cheatedPVposition(x+smearX, y+smearY, z+smearZ);
  cheatedPV->setPosition(cheatedPVposition);
  Gaudi::SymMatrix3x3 cheatedPVcov;
  cheatedPVcov(0,0) = 4.36969844565514146e-04;
  cheatedPVcov(1,1) = 4.09792105425419665e-04;
  cheatedPVcov(2,2) = 1.85588016359800601e-02;
  cheatedPV->setCovMatrix(cheatedPVcov);
  int nDoF(1); double chi2Vtx(1.);
  cheatedPV->setChi2(chi2Vtx);
  cheatedPV->setNDoF(nDoF);
  cheatedPV->setTechnique(LHCb::RecVertex::Primary);

  // save it
  m_outputVertices->insert(cheatedPV);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PGPrimaryVertex::finalize() {

  debug() << "==> Finalize" << endmsg;

  if( m_usePVResFile ) m_file->Close();

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
