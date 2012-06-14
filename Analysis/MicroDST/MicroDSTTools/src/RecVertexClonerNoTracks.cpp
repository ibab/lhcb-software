
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from LHCb
#include "Event/RecVertex.h"

// local
#include "RecVertexClonerNoTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RecVertexClonerNoTracks
//
// 2007-12-05 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RecVertexClonerNoTracks::RecVertexClonerNoTracks( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : RecVertexCloner ( type, name , parent ) { }

//=============================================================================

LHCb::RecVertex* RecVertexClonerNoTracks::clone(const LHCb::RecVertex* vertex)
{
  LHCb::RecVertex* new_vert = RecVertexCloner::clone(vertex);
  if ( new_vert ) { new_vert->clearTracks(); }
  return new_vert;
}

//=============================================================================
// Destructor
//=============================================================================
RecVertexClonerNoTracks::~RecVertexClonerNoTracks() {}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RecVertexClonerNoTracks )
