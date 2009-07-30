// $Id: RecVertexCloner.cpp,v 1.7 2009-07-30 10:03:01 jpalac Exp $
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

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RecVertexCloner );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RecVertexCloner::RecVertexCloner( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : base_class ( type, name , parent )
{
}
//=============================================================================
StatusCode RecVertexCloner::initialize() 
{

  debug() << "==> Initialize" << endmsg;

  return base_class::initialize();

}
//=============================================================================
LHCb::VertexBase* RecVertexCloner::operator() (const LHCb::VertexBase* vertex)
{
  const LHCb::RecVertex* recVertex = dynamic_cast<const LHCb::RecVertex*>(vertex);
  return (0 != recVertex) ? this->clone(recVertex) : 0;
}
//=============================================================================
LHCb::RecVertex* RecVertexCloner::operator() (const LHCb::RecVertex* vertex)
{
  return this->clone(vertex);
}
//=============================================================================
LHCb::RecVertex* RecVertexCloner::clone(const LHCb::RecVertex* vertex)
{
  
  LHCb::RecVertex* vertexClone = 
    cloneKeyedContainerItem<LHCb::RecVertex, PVCloner>(vertex);

  return vertexClone;
  
}
//=============================================================================
// Destructor
//=============================================================================
RecVertexCloner::~RecVertexCloner() {} 

//=============================================================================
