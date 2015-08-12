// Include files:
// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"
#include "Linker/LinkedTo.h"
#include "Event/MCTrackInfo.h"
// from gsl
#include "gsl/gsl_math.h"
// Local
#include "CheatedPrimaryVertices.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheatedPrimaryVertices
//
// 2008-12-07 : Marcin Kucharczyk
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(CheatedPrimaryVertices)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheatedPrimaryVertices::CheatedPrimaryVertices(const std::string& name,
                                               ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name,pSvcLocator)
  , m_inputTracks(NULL)
  , m_outputVertices(NULL)
{
  declareProperty("InputTracksName", m_inputTracksName = "Rec/Track/Best");
  declareProperty("OutputVerticesName", m_outputVerticesName = 
                                        "Hlt/Vertex/PV2D");
}

//=============================================================================
// Destructor
//=============================================================================
CheatedPrimaryVertices::~CheatedPrimaryVertices() {
}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CheatedPrimaryVertices::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc) return sc;
  if(msgLevel(MSG::DEBUG)) debug() << "Initialisation" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execution
//=============================================================================
StatusCode CheatedPrimaryVertices::execute() {

  if(msgLevel(MSG::DEBUG)) debug() << "Execute" << endmsg;

  MCTrackInfo trInfo( evtSvc(), msgSvc() );
  LinkedTo<LHCb::MCParticle> trackMClink( evtSvc(), msgSvc(), 
                                          LHCb::TrackLocation::Default);
  m_inputTracks = get<LHCb::Tracks>(m_inputTracksName);
  m_outputVertices = new LHCb::RecVertices();
  put(m_outputVertices,m_outputVerticesName);
  // MC particles assigned to hasVelo tracks
  int nrTracks = 0;
  SmartRefVector<LHCb::Track> hasVeloTracks;
  std::vector<const LHCb::MCParticle*> mcParts;
  for(LHCb::Track::Container::const_iterator itr = m_inputTracks->begin();
      m_inputTracks->end() != itr; itr++) {
    nrTracks++;
    const LHCb::Track* trk = *itr;
    if(trk->hasVelo()) {
      hasVeloTracks.push_back(trk);
      LHCb::MCParticle* mcPart = trackMClink.first(trk->key());
      if(mcPart) {
        mcParts.push_back(mcPart);
      }
    }
  }
  // Visible MC PV's
  LHCb::MCVertices* mcVertices =
                    get<LHCb::MCVertices>(LHCb::MCVertexLocation::Default);
  std::vector<const LHCb::MCVertex*> vtcsMCPV;
  int nrMCPVs = 0;
  for(LHCb::MCVertices::const_iterator itMCV = mcVertices->begin();
      mcVertices->end() != itMCV; itMCV++) {
    nrMCPVs++;
    const LHCb::MCParticle* motherPart = (*itMCV)->mother();
    if(0 == motherPart) {
      if((*itMCV)->type() == LHCb::MCVertex::ppCollision) {
        const LHCb::MCVertex* mcVtx = *itMCV;
        int isfromMCPV = 0;
        for(std::vector<const LHCb::MCParticle*>::iterator
          itP = mcParts.begin(); itP != mcParts.end(); itP++) {
          const LHCb::MCParticle* mcPart = *itP;
          isfromMCPV = isfromMCPV + fromMCVertex(mcPart,mcVtx);
        }
        if(isfromMCPV > 4) {
          vtcsMCPV.push_back(*itMCV);
	}
      }
    }
  }
  // Cheated PV's
  double sigmax = 0.020 * Gaudi::Units::mm;
  double sigmay = 0.020 * Gaudi::Units::mm;
  double sigmaz = 0.080 * Gaudi::Units::mm;
  for(std::vector<const LHCb::MCVertex*>::iterator
      itPV = vtcsMCPV.begin(); itPV != vtcsMCPV.end(); itPV++) {
    const LHCb::MCVertex* mcVtx = *itPV;
    LHCb::RecVertex* cheatedPV = new LHCb::RecVertex();
    cheatedPV->clearTracks();
    Gaudi::XYZPoint cheatedPVposition(mcVtx->position().x(), 
                                      mcVtx->position().y(),
                                      mcVtx->position().z());
    cheatedPV->setPosition(cheatedPVposition);
    Gaudi::SymMatrix3x3 cheatedPVcov;
    cheatedPVcov(0,0) = gsl_pow_2(sigmax);
    cheatedPVcov(1,1) = gsl_pow_2(sigmay);
    cheatedPVcov(2,2) = gsl_pow_2(sigmaz);
    cheatedPV->setCovMatrix(cheatedPVcov);
    int nDoF = -3;
    double chi2Vtx = 0.0;
    SmartRefVector<LHCb::Track> cheatedPVTracks;
    SmartRefVector<LHCb::Track>::iterator iTr;
    for(iTr = hasVeloTracks.begin(); iTr != hasVeloTracks.end(); iTr++) {
      LHCb::Track* vTr = *iTr;
      LHCb::MCParticle* mcPart = trackMClink.first(vTr->key());
      if(mcPart) {
        int isfromPV = 0;
        isfromPV = fromMCVertex(mcPart,mcVtx);
        if(isfromPV) {
          chi2Vtx += chi2Tr(vTr,cheatedPV);
          nDoF += 2;
          cheatedPVTracks.push_back(vTr);
	}
      }
    }
    cheatedPV->setChi2(chi2Vtx);
    cheatedPV->setNDoF(nDoF);
    cheatedPV->setTracks(cheatedPVTracks);
    cheatedPV->setTechnique(LHCb::RecVertex::Primary);
    m_outputVertices->insert(cheatedPV);
  }

  return StatusCode::SUCCESS;
}


//=============================================================================
// MC particle from MC visible PV
//=============================================================================
int CheatedPrimaryVertices::fromMCVertex(const LHCb::MCParticle* mcParticle,
                                         const LHCb::MCVertex* mcVertex)
{
  int isDaugh = 0;
  const LHCb::MCVertex* mcVtx = 0;
  const LHCb::MCParticle* motherMC = mcParticle->mother();
  while(motherMC) {
    mcVtx = motherMC->originVertex();
    motherMC = motherMC->mother();
  }
  if(mcVertex == mcVtx) isDaugh = 1;
  return isDaugh;
}

//=============================================================================
// Track chi2
//=============================================================================
double CheatedPrimaryVertices::chi2Tr(const LHCb::Track* pvTr, 
                                      const LHCb::RecVertex* invt)
{
  // Compute impact parameter
  Gaudi::XYZVector unitVect;
  Gaudi::XYZVector vd0;
  Gaudi::XYZPoint trkPoint(pvTr->firstState().x(),
                           pvTr->firstState().y(),
                           pvTr->firstState().z());
  Gaudi::XYZVector diffVect = trkPoint - invt->position();
  unitVect = pvTr->firstState().slopes().Unit();
  vd0 = unitVect.Cross(diffVect.Cross(unitVect));
  double d02 = vd0.Mag2();
  // Compute the error on the track impact parameter
  Gaudi::XYZVector vd0Unit = vd0.Unit();
  ROOT::Math::SVector<double,2> xyVec;
  xyVec[0] = vd0Unit.x();
  xyVec[1] = vd0Unit.y();
  Gaudi::SymMatrix2x2 covMatrix =
         pvTr->firstState().covariance().Sub<Gaudi::SymMatrix2x2>(0,0);
  ROOT::Math::SVector<double,2> product;
  product = covMatrix * xyVec;
  double err2d0 = xyVec[0] * product[0] + xyVec[1] * product[1];
  double chi2 = d02 / err2d0;
  return chi2;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CheatedPrimaryVertices::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
