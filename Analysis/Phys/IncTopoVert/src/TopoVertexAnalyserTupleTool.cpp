// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "Event/RecVertex.h"
#include "Event/RecVertex.h"
#include "Event/Track.h"
// local
#include "TopoVertexAnalyserTupleTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TopoVertexAnalyserTupleTool
//
// 2013-01-31 : Julien Cogan and Mathieu Perrin-Terrin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TopoVertexAnalyserTupleTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TopoVertexAnalyserTupleTool::TopoVertexAnalyserTupleTool( const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent )
: GaudiTool ( type, name , parent )
{
  declareInterface<ITopoVertexAnalyserTupleTool>(this);
}

//=============================================================================
// Analyser
//=============================================================================
StatusCode TopoVertexAnalyserTupleTool::analyseVertices(Tuples::Tuple* tuple, const std::string& tesLocation){


  //Defines variable
  int evt_type = 0; // 1=3+3pure; 2=6pure; 3=other;
  int nb_type1 = 0;
  int nb_type2 = 0;
  std::vector< int > nb_tracks;
  std::vector< int > nb_tauP_tracks;
  std::vector< int > nb_tauM_tracks;
  std::vector< int > nb_non_tau_tracks;
  int tmp_nb_tauP_tracks, tmp_nb_tauM_tracks, tmp_nb_non_tau_tracks;


  //load the vertices from the TES and loop if they exist
  LHCb::RecVertex::Container* vertices = NULL;
  if( exist<LHCb::RecVertex::Container>(tesLocation) ){
    vertices = get<LHCb::RecVertex::Container>(tesLocation);

    for(LHCb::RecVertex::Container::const_iterator i_vtx = vertices->begin() ; vertices->end()!=i_vtx ; ++i_vtx){
      tmp_nb_tauP_tracks    = 0;
      tmp_nb_tauM_tracks    = 0;
      tmp_nb_non_tau_tracks = 0;
      for (SmartRefVector< LHCb::Track >::const_iterator i_tr=(*i_vtx)->tracks().begin(); i_tr!=(*i_vtx)->tracks().end(); ++i_tr){
        int mother_pid = (*i_tr)->info(0,0.0);
        if      (mother_pid ==  15 )  tmp_nb_tauM_tracks+=1;
        else if (mother_pid == -15 )  tmp_nb_tauP_tracks+=1;
        else                          tmp_nb_non_tau_tracks +=1;
      }
      nb_tracks.push_back((*i_vtx)->tracks().size());
      nb_tauP_tracks.push_back(tmp_nb_tauP_tracks);
      nb_tauM_tracks.push_back(tmp_nb_tauM_tracks);
      nb_non_tau_tracks.push_back(tmp_nb_non_tau_tracks);

      //type of vertex

      if      (tmp_nb_tauP_tracks == 3 && tmp_nb_tauM_tracks == 0 && tmp_nb_non_tau_tracks == 0)  {nb_type1+=1;} //3pure
      else if (tmp_nb_tauP_tracks == 0 && tmp_nb_tauM_tracks == 3 && tmp_nb_non_tau_tracks == 0)  {nb_type1+=1;} //3pure
      else if (tmp_nb_tauP_tracks == 3 && tmp_nb_tauM_tracks== 3 && tmp_nb_non_tau_tracks == 0 )   {nb_type2+=1;} //6pure

    }
  }

  //type of event
  if     ( nb_type1 == 2 && nb_type2 == 0  ) evt_type=1; //3+3pure
  else if( nb_type1 == 0 && nb_type2 == 1  ) evt_type=2; //6pure
  else evt_type=3;

  //fill n-tuple
  //std::string prefix=name();
  //prefix += "_";
  (*tuple)->column( "ZVTOPcat", evt_type);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Destructor
//=============================================================================
TopoVertexAnalyserTupleTool::~TopoVertexAnalyserTupleTool() {}

//=============================================================================
