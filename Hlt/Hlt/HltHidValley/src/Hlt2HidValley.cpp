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
// Implementation file for class : HidValley
//
// 2008-08-21 : Marcin Kucharczyk
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(Hlt2HidValley);

// Sorting
bool sortPVz(LHCb::RecVertex* first, LHCb::RecVertex* second) {
  return first->position().z() < second->position().z();
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt2HidValley::Hlt2HidValley(const std::string& name,
                             ISvcLocator* pSvcLocator)
  : DVAlgorithm(name,pSvcLocator)
{
  declareProperty("InputTracks", m_inputTracks = 
                                 LHCb::TrackLocation::HltForward);
  declareProperty("InputPrimaryVertices", m_inputPrimaryVertices = 
                                          "Hlt/Vertex/PV2D");
  declareProperty("InputDisplacedVertices", m_inputDisplacedVertices = 
                                            "Rec/Vertices/myVrtcsOff");
}

//=============================================================================
// Destructor
//=============================================================================
Hlt2HidValley::~Hlt2HidValley() {};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode Hlt2HidValley::initialize() {

  debug() << "Hlt2HidValley initialize" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Execution
//=============================================================================
StatusCode Hlt2HidValley::execute() {

  debug() << "Hlt2HidValley execute"<< endreq;
  setFilterPassed(false);
  LHCb::Tracks* tracks = get<LHCb::Tracks>(m_inputTracks);
  LHCb::RecVertices* primVrtcs =
                     get<LHCb::RecVertices>(m_inputPrimaryVertices);
  LHCb::RecVertices* displacedVrtcs = 
                     get<LHCb::RecVertices>(m_inputDisplacedVertices);
  // Real PV
  std::sort(primVrtcs->begin(),primVrtcs->end(),sortPVz);
  const LHCb::RecVertex* realPV = 0;
  int minzPV = 0;
  for(std::vector<LHCb::RecVertex*>::const_iterator itPV = primVrtcs->begin(); 
       itPV != primVrtcs->end(); itPV++) {
    minzPV++;
    const LHCb::RecVertex* pvtx = *itPV;
    if(minzPV == 1) {
      realPV = pvtx;
    }
  }
  //===========================================================================
  // Hlt2 Hidden Valley selection
  //===========================================================================
  // Require at least 1 PV
  if((primVrtcs->size() > 0)) {
    // Tracks
    double sumPtTracks = 0.0;
    double sumXYTrackfirstStates = 0.0;
    for(LHCb::Track::Container::const_iterator itr = tracks->begin(); 
        tracks->end() != itr; itr++) {
      const LHCb::Track* trk = *itr;
      double xyfState = sqrt(trk->firstState().x() * trk->firstState().x() +
                             trk->firstState().y() * trk->firstState().y());
      sumPtTracks += trk->pt() / 1000.0;
      sumXYTrackfirstStates += xyfState;
    }
    // Vertices
    int nrDisplacedOffVrtcs = 0;
    double sumSVxyDist = 0.0;
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
      if(distVtcs > 0.001) {
      	nrDisplacedOffVrtcs++;
      	sumSVxyDist += xyDist;
      }
    }    
    // Cuts
    if((sumPtTracks > 250) && (sumXYTrackfirstStates > 30.0) && 
       (nrDisplacedOffVrtcs > 1) && (sumSVxyDist > 0.7)) {
      setFilterPassed(true);
    }			 
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode Hlt2HidValley::finalize() {

  debug() << "==> Finalize" << endmsg;

  return StatusCode::SUCCESS;
}
