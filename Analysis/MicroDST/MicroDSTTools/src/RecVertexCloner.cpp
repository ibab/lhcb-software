// $Id: RecVertexCloner.cpp,v 1.5 2009-04-16 12:16:17 jpalac Exp $
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
  : MicroDSTTool ( type, name , parent )
{
  declareInterface<ICloneRecVertex>(this);
}
//=============================================================================
StatusCode RecVertexCloner::initialize() 
{

  debug() << "==> Initialize" << endmsg;

  return MicroDSTTool::initialize();

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
