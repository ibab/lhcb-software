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
#include "Kernel/IJetMaker.h"
// from Event
#include "Event/Track.h"
#include "Event/RecVertex.h"
// From PhysEvent
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/MCParticle.h"
// Local
#include "HighPtJetSelection.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HighPtJetSelection
//
// 2010-04-03 : Marcin Kucharczyk
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(HighPtJetSelection);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HighPtJetSelection::HighPtJetSelection(const std::string& name,
                             ISvcLocator* pSvcLocator)
  : DVAlgorithm(name,pSvcLocator)
{
  declareProperty("JetMakerName",m_jetMakerName = "LoKi::FastJetMaker");
  declareProperty("InputPVs",m_inputPVs = LHCb::RecVertexLocation::Primary);
  declareProperty("MinPartPt", m_minChargedPartPt = 0.1); // [GeV]
  declareProperty("MinPartE", m_minNeutralPartE = 2.0);   // [GeV]
  declareProperty("Min1stJetPt", m_min1stJetPt = 10.0);   // [GeV]
  declareProperty("Min2ndJetPt", m_min2ndJetPt = 0.0);    // [GeV]
  declareProperty("JetMult", m_jetMult = 1);
}

//=============================================================================
// Destructor
//=============================================================================
HighPtJetSelection::~HighPtJetSelection() {};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode HighPtJetSelection::initialize() {
  StatusCode sc = DVAlgorithm::initialize();
  if (!sc) return sc;
  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
  m_jetMaker = tool<IJetMaker>(m_jetMakerName,this);
  return StatusCode::SUCCESS;
};

//=============================================================================
// Execution
//=============================================================================
StatusCode HighPtJetSelection::execute() {
  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  setFilterPassed(false);
  // Check if there is reconstructed PV
  if(exist<LHCb::RecVertices>(m_inputPVs)) {
    // Select particles for fastJet
    Particle::ConstVector inputParts = desktop()->particles();
    Particle::ConstVector jetParts;
    for(Particle::ConstVector::const_iterator ip = inputParts.begin();
        ip != inputParts.end(); ip++) {
      bool takePart = false;
      if((*ip)->charge()) {
        if((*ip)->momentum().pt() / Gaudi::Units::GeV > m_minChargedPartPt) {
          takePart = true;
        }
      } else {
        if((*ip)->momentum().e() / Gaudi::Units::GeV > m_minNeutralPartE) {
          takePart = true;
        }  
      }
      if(takePart) jetParts.push_back((*ip));
    }
    // Find jets
    IJetMaker::Input jetInputs(jetParts.begin(),jetParts.end());
    IJetMaker::Jets jets;
    StatusCode sc = m_jetMaker->makeJets(jetInputs,jets);
    if(sc.isFailure()) {
      setFilterPassed(false);
    } else {
      if(jets.size() >= m_jetMult) {
        bool highPtJet = false;
        unsigned int twoHighPtJets = 0;
        for(Particles::iterator ij = jets.begin(); jets.end() != ij; ij++) {
          if((*ij)->pt() / Gaudi::Units::GeV > m_min2ndJetPt) {
            twoHighPtJets++;
            if((*ij)->pt() / Gaudi::Units::GeV > m_min1stJetPt) {
              highPtJet = true;
            }
          }
        }
        if(highPtJet && (twoHighPtJets >= m_jetMult)) setFilterPassed(true);
      }
    }  
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode HighPtJetSelection::finalize() {
  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  return DVAlgorithm::finalize();
}
