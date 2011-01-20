// Include files:
//from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IAlgTool.h"
//from Kernel
#include "Kernel/DVAlgorithm.h"
// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"
// Local
#include "Hlt2HidValley.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2HidValley
//
// 2008-08-21 : Marcin Kucharczyk
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(Hlt2HidValley);

bool sortPVz(const LHCb::RecVertex* first, const LHCb::RecVertex* second) {
  return first->position().z() < second->position().z();
}
bool sortPartPt(const LHCb::Particle* first, const LHCb::Particle* second) {
  return first->pt() > second->pt();
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt2HidValley::Hlt2HidValley(const std::string& name,
                             ISvcLocator* pSvcLocator)
  : DVAlgorithm(name,pSvcLocator)
{
  declareProperty("InputTracks", m_inputTracks = "");
  declareProperty("InputDisplacedVertices", m_inputDisplacedVertices = 
                  "Rec/Vertices/displacedOffVertices");
}

//=============================================================================
// Destructor
//=============================================================================
Hlt2HidValley::~Hlt2HidValley() {};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode Hlt2HidValley::initialize() {
  StatusCode sc = DVAlgorithm::initialize();
  if (!sc) return sc;
  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
  return StatusCode::SUCCESS;
};

//=============================================================================
// Execution
//=============================================================================
StatusCode Hlt2HidValley::execute() {

  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  setFilterPassed(false);

  LHCb::Tracks* inputTracks = get<LHCb::Tracks>(m_inputTracks);
  const LHCb::Particle::ConstVector& inputParts = this->i_particles();
  const LHCb::RecVertex::Range primVertices = this->primaryVertices();
  LHCb::RecVertices* displacedVrtcs = 
                     get<LHCb::RecVertices>(m_inputDisplacedVertices);
  if((primVertices.size() > 0) && inputParts.size() > 0) {
    std::vector<const LHCb::RecVertex*> primVrtcs;
    for(LHCb::RecVertex::Range::const_iterator 
        itPV = primVertices.begin(); primVertices.end() != itPV; ++itPV) {
      const LHCb::RecVertex* pvtx = *itPV;
      primVrtcs.push_back(pvtx);
    }
    std::sort(primVrtcs.begin(),primVrtcs.end(),sortPVz);
    const LHCb::RecVertex* realPV = *(primVrtcs.begin());
    std::vector<const LHCb::Particle*> parts;
    for(LHCb::Particle::ConstVector::const_iterator ip = inputParts.begin();
        inputParts.end() != ip; ++ip) {
      const LHCb::Particle* part = *ip;
      parts.push_back(part);
    }
    std::sort(parts.begin(),parts.end(),sortPartPt);
    const LHCb::Particle* highestPtPion = *(parts.begin());
    //=========================================================================
    // Hlt2 Hidden Valley selection
    //=========================================================================
    double sumPtTracks = 0.;
    double sumXYTrackfirstStates = 0.;
    for(LHCb::Track::Container::const_iterator itr = inputTracks->begin(); 
        inputTracks->end() != itr; itr++) {
      const LHCb::Track* trk = *itr;
      double xyfState = sqrt(trk->firstState().x() * trk->firstState().x() +
                             trk->firstState().y() * trk->firstState().y());
      sumPtTracks += trk->pt();
      sumXYTrackfirstStates += xyfState;
    }
    double sumSVxyDist = 0.;
    for(LHCb::RecVertices::const_iterator itRV = displacedVrtcs->begin();
        displacedVrtcs->end() != itRV; itRV++) {
      const LHCb::RecVertex* dVtx = *itRV;
      double distVtcs = sqrt((realPV->position().x() - dVtx->position().x()) * 
                             (realPV->position().x() - dVtx->position().x()) +
      	                     (realPV->position().y() - dVtx->position().y()) * 
                             (realPV->position().y() - dVtx->position().y()) +
                             (realPV->position().z() - dVtx->position().z()) * 
                             (realPV->position().z() - dVtx->position().z()));
      double xyDist = sqrt(dVtx->position().x() * dVtx->position().x() +
                           dVtx->position().y() * dVtx->position().y());
      if(distVtcs > .001) {
      	sumSVxyDist += xyDist;
      }
    }    
    // Cuts
    if((sumPtTracks > 250. * Gaudi::Units::GeV) && 
       (sumXYTrackfirstStates > 30. * Gaudi::Units::mm) && 
       (sumSVxyDist > 0.7 * Gaudi::Units::mm)) {
      setFilterPassed(true);
      this->cloneAndMarkTree(highestPtPion);
    }			 
  }

  return StatusCode::SUCCESS;

}

//=============================================================================
// Finalization
//=============================================================================
StatusCode Hlt2HidValley::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize();
}
