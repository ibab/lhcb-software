// $Id: VertexBaseFromRecVertexClonerNoTracks.cpp,v 1.1 2010-08-11 12:48:36 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from LHCb
#include "Event/RecVertex.h"

// local
#include "VertexBaseFromRecVertexClonerNoTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VertexBaseFromRecVertexClonerNoTracks
//
// 2007-12-05 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VertexBaseFromRecVertexClonerNoTracks::
VertexBaseFromRecVertexClonerNoTracks( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
  : VertexBaseFromRecVertexCloner ( type, name, parent ) { }

//=============================================================================

LHCb::RecVertex* 
VertexBaseFromRecVertexClonerNoTracks::clone( const LHCb::RecVertex* vertex )
{
  LHCb::RecVertex* vertexClone = VertexBaseFromRecVertexCloner::clone(vertex);
  if ( vertexClone ) { vertexClone->clearTracks(); }
  return vertexClone;
}

//=============================================================================
// Destructor
//=============================================================================
VertexBaseFromRecVertexClonerNoTracks::~VertexBaseFromRecVertexClonerNoTracks() {}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VertexBaseFromRecVertexClonerNoTracks )

//=============================================================================
