// $Id: RecVertexClonerWithTracks.cpp,v 1.1.1.1 2007-12-10 09:32:24 jpalac Exp $
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

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RecVertexClonerWithTracks );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RecVertexClonerWithTracks::RecVertexClonerWithTracks( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
  : MicroDSTTool ( type, name , parent )
{
  declareInterface<ICloneRecVertex>(this);

}
//=============================================================================
StatusCode RecVertexClonerWithTracks::initialize() 
{
  
  StatusCode sc = MicroDSTTool::initialize();

  debug() << "Going to get TrackCloner" << endmsg;

  m_trackCloner = tool<ICloneTrack>("TrackCloner", 
                                    this->parent() );

  if (m_trackCloner) {
    debug() << "Found TrackCloner " << m_trackCloner->name() << endmsg;
  } else {
    error() << "Failed to find TrackCloner" << endmsg;
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
  
  LHCb::RecVertex* vertexClone = 
    cloneKeyedContainerItem<LHCb::RecVertex, MicroDST::BasicRecVertexCloner>(vertex);

  // and now do something about the tracks!

  return vertexClone;
  
}
//=============================================================================
StatusCode RecVertexClonerWithTracks::finalize() 
{
  return MicroDSTTool::finalize();
}
//=============================================================================
// Destructor
//=============================================================================
RecVertexClonerWithTracks::~RecVertexClonerWithTracks() {} 

//=============================================================================
