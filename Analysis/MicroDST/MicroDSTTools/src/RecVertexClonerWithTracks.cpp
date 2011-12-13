// $Id: RecVertexClonerWithTracks.cpp,v 1.6 2010-08-11 12:52:52 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include <MicroDST/ICloneTrack.h>

// local
#include "RecVertexClonerWithTracks.h"
#include "RecVertexClonerFunctors.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RecVertexClonerWithTracks
//
// 2007-12-05 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RecVertexClonerWithTracks::RecVertexClonerWithTracks( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
  :
  base_class ( type, name , parent ),
  m_trackCloner(NULL),
  m_trackClonerType("TrackCloner")
{
  declareProperty("ICloneTrack", m_trackClonerType);
}

//=============================================================================

StatusCode RecVertexClonerWithTracks::initialize()
{
  const StatusCode sc = base_class::initialize();
  if ( sc.isFailure() ) return sc;

  debug() << "Going to get TrackCloner" << endmsg;

  m_trackCloner = tool<ICloneTrack>( m_trackClonerType,
                                     this->parent() );
  if (!m_trackCloner) 
  {
    return Error("Failed to find TrackCloner");
  }

  return sc;
}

//=============================================================================

LHCb::RecVertex* RecVertexClonerWithTracks::operator() (const LHCb::RecVertex* vertex)
{
  return this->clone(vertex);
}

//=============================================================================

LHCb::RecVertex* RecVertexClonerWithTracks::clone(const LHCb::RecVertex* vertex)
{
  if (!vertex) return NULL;

  LHCb::RecVertex* vertexClone =
    cloneKeyedContainerItem<MicroDST::BasicRecVertexCloner>(vertex);

  if (!vertexClone) return NULL;

  const SmartRefVector<LHCb::Track> & tracks = vertex->tracks();

  vertexClone->clearTracks();

  if (!tracks.empty()) 
  {
    for ( SmartRefVector<LHCb::Track>::const_iterator iTrack = tracks.begin(); 
          iTrack != tracks.end(); ++iTrack ) 
    {
      vertexClone->addToTracks( (*m_trackCloner)( iTrack->target() ) );
    }
  }
  
  return vertexClone;
}

//=============================================================================
// Destructor
//=============================================================================
RecVertexClonerWithTracks::~RecVertexClonerWithTracks() {}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RecVertexClonerWithTracks )
