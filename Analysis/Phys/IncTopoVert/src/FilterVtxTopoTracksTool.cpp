// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "FilterVtxTopoTracksTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterVtxTopoTracksTool
//
// 2012-10-24 : Julien Cogan and Mathieu Perrin-Terrin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FilterVtxTopoTracksTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterVtxTopoTracksTool::FilterVtxTopoTracksTool( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ), m_tracks(0)
{
  declareInterface<IFilterVtxTopoTracksTool>(this);

  declareProperty("InputTrackLocation",m_inputTrackLocation = "/Event/Rec/Track/Best" );

}
//=============================================================================
// Destructor
//=============================================================================
FilterVtxTopoTracksTool::~FilterVtxTopoTracksTool() {} 

//=============================================================================
std::vector<const LHCb::Track*> & FilterVtxTopoTracksTool::filteredTracks(Tuples::Tuple* tuple)
{
  m_tracks.clear();
  
  LHCb::Tracks* trackContainer = get<LHCb::Tracks*>( m_inputTrackLocation );
  m_tracks.reserve(trackContainer->size());
  for (LHCb::Tracks::iterator it_ptrk=trackContainer->begin(); it_ptrk<trackContainer->end(); ++it_ptrk){
    m_tracks.push_back(*it_ptrk);
  }

  return m_tracks;

}
