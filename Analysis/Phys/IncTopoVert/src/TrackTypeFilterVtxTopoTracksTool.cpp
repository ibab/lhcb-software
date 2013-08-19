// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "Event/VertexBase.h"
#include "Event/RecVertex.h"
// local
#include "TrackTypeFilterVtxTopoTracksTool.h"

//temporary to fill tuple from gaudi
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackTypeFilterVtxTopoTracksTool
//
// 2012-11-27 : Julien Cogan and Mathieu Perrin-Terrin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackTypeFilterVtxTopoTracksTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackTypeFilterVtxTopoTracksTool::TrackTypeFilterVtxTopoTracksTool( const std::string& type,
                                                                          const std::string& name,
                                                                          const IInterface* parent )
  : GaudiHistoTool ( type, name , parent )
{
  declareInterface<IFilterVtxTopoTracksTool>(this);


  vector<string>                   m_TrackTypeAllowed_default;
  m_TrackTypeAllowed_default.push_back("Long");
  declareProperty("TrackType"  ,m_TrackTypeAllowed = m_TrackTypeAllowed_default); 

  m_TrackTypeMap["TypeUnknown"] = LHCb::Track::TypeUnknown;
  m_TrackTypeMap["Velo"]        = LHCb::Track::Velo;
  m_TrackTypeMap["VeloR"]       = LHCb::Track::VeloR;
  m_TrackTypeMap["Long"]        = LHCb::Track::Long;
  m_TrackTypeMap["Upstream"]    = LHCb::Track::Upstream;
  m_TrackTypeMap["Downstream"]  = LHCb::Track::Downstream;
  m_TrackTypeMap["Ttrack"]      = LHCb::Track::Ttrack;
  m_TrackTypeMap["Muon"]        = LHCb::Track::Muon;
  m_TrackTypeMap["Calo"]        = LHCb::Track::Calo;
  m_TrackTypeMap["TT"]          = LHCb::Track::TT;

  m_TrackTypeMapRev[LHCb::Track::TypeUnknown] = "TypeUnknown";
  m_TrackTypeMapRev[LHCb::Track::Velo]        = "Velo";
  m_TrackTypeMapRev[LHCb::Track::VeloR]       = "VeloR";
  m_TrackTypeMapRev[LHCb::Track::Long]        = "Long";
  m_TrackTypeMapRev[LHCb::Track::Upstream]    = "Upstream";
  m_TrackTypeMapRev[LHCb::Track::Downstream]  = "Downstream";
  m_TrackTypeMapRev[LHCb::Track::Ttrack]      = "Ttrack";
  m_TrackTypeMapRev[LHCb::Track::Muon]        = "Muon";
  m_TrackTypeMapRev[LHCb::Track::Calo]        = "Calo";
  m_TrackTypeMapRev[LHCb::Track::TT]          = "TT";

}


//============================================================================= 
// Initialization
//=============================================================================
StatusCode TrackTypeFilterVtxTopoTracksTool::initialize() {
  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================




//=============================================================================
std::vector<const LHCb::Track*> & TrackTypeFilterVtxTopoTracksTool::filteredTracks(Tuples::Tuple* tuple)
{
  LHCb::Tracks* input_tracks =  get<LHCb::Tracks>("Rec/Track/Best");
  std::vector<const LHCb::Track*> vec_input_tracks;
  for ( LHCb::Tracks::const_iterator itt = input_tracks->begin(); itt != input_tracks->end() ; ++itt) {
    vec_input_tracks.push_back(*itt);
  }
  debug()<<"Filter tracks from TES :"<<vec_input_tracks.size()<<" tracks"<<endmsg;
  return filteredTracks(vec_input_tracks, tuple);
  
}
//=============================================================================
std::vector<const LHCb::Track*> & TrackTypeFilterVtxTopoTracksTool::filteredTracks(std::vector<const LHCb::Track*> input_tracks,Tuples::Tuple* tuple)
{
  m_tracks.clear();
  tuple =0;
  if (tuple) {
  }
  
  if (input_tracks.size()==0) {
    debug()<<"No input tracks"<<endmsg;
    return m_tracks;
  }


  for (std::vector<const LHCb::Track*>::iterator it = input_tracks.begin(); it!=input_tracks.end();++it){
    unsigned int i_type=0;
    for (i_type=0; i_type<m_TrackTypeAllowed.size(); i_type++){
      if( (*it)->checkType( m_TrackTypeMap[m_TrackTypeAllowed[i_type]]) ){
        break;
      }
    }
    if(i_type<m_TrackTypeAllowed.size()) {
      m_tracks.push_back((*it));   
    }
  }
  
  return m_tracks;
}


//=============================================================================

//=============================================================================
// Destructor
//=============================================================================
TrackTypeFilterVtxTopoTracksTool::~TrackTypeFilterVtxTopoTracksTool() {} 

//=============================================================================
