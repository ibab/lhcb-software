// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "Event/VertexBase.h"
#include "Event/RecVertex.h"
// local
#include "IpsFilterVtxTopoTracksTool.h"

//temporary to fill tuple from gaudi
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

//-----------------------------------------------------------------------------
// Implementation file for class : IpsFilterVtxTopoTracksTool
//
// 2012-11-27 : Julien Cogan and Mathieu Perrin-Terrin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( IpsFilterVtxTopoTracksTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
IpsFilterVtxTopoTracksTool::IpsFilterVtxTopoTracksTool( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent )
: GaudiHistoTool ( type, name , parent )
{
  declareInterface<IFilterVtxTopoTracksTool>(this);

  declareProperty("IpsCut"  ,m_ips = 5.0);

  declareProperty("DistanceCalculatorToolType",m_distanceCalculatorToolType="LoKi::TrgDistanceCalculator"); //LoKi::DistanceCalculator
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode IpsFilterVtxTopoTracksTool::initialize()
{
  const StatusCode sc = GaudiHistoTool::initialize();
  if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize" << endmsg;

  m_Geom = tool<IDistanceCalculator> (m_distanceCalculatorToolType, this);

  return sc;
}

//=============================================================================


//=============================================================================
std::vector<const LHCb::Track*> & IpsFilterVtxTopoTracksTool::filteredTracks(Tuples::Tuple* tuple)
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
std::vector<const LHCb::Track*> & IpsFilterVtxTopoTracksTool::filteredTracks(std::vector<const LHCb::Track*> input_tracks,Tuples::Tuple* tuple)
{
  m_tracks.clear();
  if (input_tracks.size()==0) {
    debug()<<"No input tracks"<<endmsg;
    return m_tracks;
  }

  LHCb::RecVertex::Container* primary_vertices = NULL;
  if(exist<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary)){
    primary_vertices = get<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary);
  }
  else {
    warning()<<"No primary vertex at location"<<endreq;
    Error("No primary vertex at location",StatusCode::FAILURE,50).ignore();
    return m_tracks;
  }

  double ipsall;
  bool keep_track = true;
  debug()<<"Tool "<<name()<<endmsg;
  debug()<<"Input tracks Size "<<input_tracks.size()<<endmsg;
  std::vector<double> v_ips_of_tracks;
  std::vector<bool> v_is_tracks_used;

  for (std::vector<const LHCb::Track*>::iterator it = input_tracks.begin(); it!=input_tracks.end();++it){
    keep_track = true;

    debug()<<"\tcheck IPS: "<<endmsg;
    //IPS filter
    ipsall= 999;
    double imp, impchi2;
    imp =m_ips +1.;
    impchi2=m_ips +1.;
    LHCb::RecVertex::Container::const_iterator iv;
    for ( iv = primary_vertices->begin(); iv != primary_vertices->end(); iv++) {
      StatusCode sc2 = m_Geom->distance(((*it)),(*iv),imp,impchi2);
      if (sc2.isFailure() ) {
        warning()<<"m_Geom problems"<<endreq;
        continue;
      }
      if (impchi2<ipsall) {
        ipsall = impchi2;
      }
    }

    //fill vector for n-tuple
    v_ips_of_tracks.push_back(ipsall);

    if (ipsall<m_ips) {
      keep_track = false;
    }
    if(keep_track == true) {
      m_tracks.push_back((*it));
      v_is_tracks_used.push_back(1);
    }
    else v_is_tracks_used.push_back(0);
  }

  if(tuple != NULL)
  {
    //fill ntuple
    std::string prefix=name();
    prefix += "_";
    (*tuple)->farray( prefix+"IPS"         , v_ips_of_tracks, prefix+"L_tracks" , 500 );
    (*tuple)->farray( prefix+"ZVTOP", v_is_tracks_used, prefix+"L_tracks" , 500 );
  }

  return m_tracks;
}


//=============================================================================

//=============================================================================
// Destructor
//=============================================================================
IpsFilterVtxTopoTracksTool::~IpsFilterVtxTopoTracksTool() {}

//=============================================================================
