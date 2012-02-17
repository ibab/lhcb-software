// $Id: RecVertexCloner.cpp,v 1.8 2010-08-11 12:51:28 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from LHCb
#include "Event/RecVertex.h"

// local
#include "RecVertexCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RecVertexCloner
//
// 2007-12-05 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RecVertexCloner::RecVertexCloner( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : base_class ( type, name , parent ) { }

//=============================================================================

LHCb::RecVertex* RecVertexCloner::operator() ( const LHCb::RecVertex* vertex )
{
  return this->clone(vertex);
}

//=============================================================================

LHCb::RecVertex* RecVertexCloner::clone( const LHCb::RecVertex* vertex )
{
  return cloneKeyedContainerItem<PVCloner>(vertex);
}

//=============================================================================
// Destructor
//=============================================================================
RecVertexCloner::~RecVertexCloner() {}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RecVertexCloner )
