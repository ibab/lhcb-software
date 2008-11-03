// $Id: HltTrackFit.cpp,v 1.1 2008-11-03 08:10:36 albrecht Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

//from Event
#include "Event/Track.h"

// local
#include "HltTrackFit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackFit
//
// 2008-10-31 : Johannes Albrecht
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltTrackFit );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackFit::HltTrackFit( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
, m_fit(0)
{
  declareInterface<ITracksFromTrack>(this);
  declareProperty("FitterName", m_fitName = "TrackMasterFitter/Fit");
  declareProperty("PID",m_pid=211);
}
//=============================================================================
// Destructor
//=============================================================================
HltTrackFit::~HltTrackFit() {} 

//=============================================================================
StatusCode HltTrackFit::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return sc;
  }
 debug() << " ===> initialize HltTrackFit" << endmsg;

 m_fit = tool<ITrackFitter>(m_fitName, this);

 return sc;
}

StatusCode HltTrackFit::tracksFromTrack(
		const LHCb::Track& seed,
		std::vector<LHCb::Track*>& tracks )
{
  
  LHCb::Track* tr = seed.clone();
  
  StatusCode sc = StatusCode::SUCCESS;
  //fit the track only if it hasn't been fitted before ..
  if(tr->checkFitHistory( LHCb::Track::FitUnknown) ){
    
    sc = m_fit->fit(*tr,LHCb::ParticleID(m_pid));
    if( sc.isFailure() ) return StatusCode::FAILURE;
    //FIXME this should be done inside the fit, but it isn't .. 
    tr->setFitHistory(LHCb::Track::StdKalman);
    
  }
  
  tracks.push_back(tr);
  
  
  return sc;
}

