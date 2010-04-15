// $Id: HltGuidedForward.cpp,v 1.2 2010-04-15 09:30:00 albrecht Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltGuidedForward.h"
#include "Event/Track.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltGuidedForward
//
// 2008-04-08 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltGuidedForward );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltGuidedForward::HltGuidedForward( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITracksFromTrack>(this);

  declareProperty("NSigma",m_nSigma = 5.);
}

StatusCode HltGuidedForward::initialize() {
  StatusCode sc = GaudiTool::initialize();

  m_l0ConfExtrapolator  = tool<IL0ConfExtrapolator>("L0ConfExtrapolator");
  m_hitMan = 
    tool<Tf::TStationHitManager <PatForwardHit> >("PatTStationHitManager");
  m_forwardTool = tool<ITracksFromTrack>("PatForwardTool", this ); 

  return sc;
}


StatusCode HltGuidedForward::tracksFromTrack(const LHCb::Track& track,
                                             std::vector<LHCb::Track*>& tracks) {
  
  StatusCode sc = StatusCode::SUCCESS;

  const LHCb::Track& velo = *(track.ancestors().front());
  const LHCb::Track& calo = *(track.ancestors().back());

  FwdHypothesis tp = m_l0ConfExtrapolator->getFwdHypothesis(velo,calo,
                                                            m_nSigma);
  m_hitMan->prepareHitsInWindow(tp);
  
  sc = m_forwardTool->tracksFromTrack(velo,tracks);
  for (std::vector<LHCb::Track*>::iterator it = tracks.begin();
       it != tracks.end(); ++it)
    (*it)->addToAncestors(&calo);
  
  m_hitMan->clearHits();
  
  return sc;
}



//=============================================================================
// Destructor
//=============================================================================
HltGuidedForward::~HltGuidedForward() {} 

//=============================================================================
