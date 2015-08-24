
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from LHCb
#include "Event/RecVertex.h"

// MicroDST
#include <MicroDST/ICloneTrack.h>

// local
#include "VertexBaseFromRecVertexClonerWithTracks.h"

// STL
#include <sstream>

//-----------------------------------------------------------------------------
// Implementation file for class : VertexBaseFromRecVertexClonerWithTracks
//
// 2007-12-05 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VertexBaseFromRecVertexClonerWithTracks::
VertexBaseFromRecVertexClonerWithTracks( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
  : VertexBaseFromRecVertexCloner ( type, name , parent ),
    m_trackCloner(NULL)
 { 
   declareProperty("ICloneTrack", m_trackClonerType = "TrackCloner" );
 }

//=============================================================================

StatusCode VertexBaseFromRecVertexClonerWithTracks::initialize()
{
  const StatusCode sc = VertexBaseFromRecVertexCloner::initialize();
  if ( sc.isFailure() ) return sc;

  m_trackCloner = tool<ICloneTrack>( m_trackClonerType,
                                     this->parent() );

  return sc;
}

//=============================================================================

LHCb::RecVertex*
VertexBaseFromRecVertexClonerWithTracks::clone( const LHCb::RecVertex* vertex )
{
  LHCb::RecVertex* vClone = VertexBaseFromRecVertexCloner::clone(vertex);
  if ( !vClone ) return NULL;

  // get the list of tracks with weights from the original
  const LHCb::RecVertex::TrackWithWeightVector tracks = vertex->tracksWithWeights();

  // clear the tracks in the clone
  vClone->clearTracks();

  // Clone tracks and add them, with the correct weight, to the clone
  for ( const auto& tw : tracks )
  {
    if ( tw.first )
    {
      vClone->addToTracks( (*m_trackCloner)(tw.first), tw.second );
    }
    else
    {
      std::ostringstream mess;
      mess << "Null Track SmartRef found in PV in '"
           << tesLocation(vertex) << "'";
      Warning( mess.str(), StatusCode::SUCCESS ).ignore();
    }
  }

  // return
  return vClone;
}

//=============================================================================
// Destructor
//=============================================================================
VertexBaseFromRecVertexClonerWithTracks::~VertexBaseFromRecVertexClonerWithTracks() {}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VertexBaseFromRecVertexClonerWithTracks )

//=============================================================================
