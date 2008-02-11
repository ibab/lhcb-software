// $Id: MCVertexCloner.cpp,v 1.2 2008-02-11 22:40:07 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// From LHCb
#include <Event/MCVertex.h>
// local
#include "MCVertexCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCVertexCloner
//
// 2007-11-30 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCVertexCloner );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCVertexCloner::MCVertexCloner( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : 
  MicroDSTTool ( type, name , parent )
{
  declareInterface<ICloneMCVertex>(this);
}
//=============================================================================
LHCb::MCVertex* MCVertexCloner::operator() (const LHCb::MCVertex* vertex)
{
  return this->clone(vertex);
}
//=============================================================================
//=============================================================================
LHCb::MCVertex* MCVertexCloner::clone(const LHCb::MCVertex* vertex)
{
  return cloneKeyedContainerItem<LHCb::MCVertex, BasicMCVtxCloner>(vertex);
}
//=============================================================================
StatusCode MCVertexCloner::initialize()
{

  StatusCode sc = MicroDSTTool::initialize();
  
  if (! sc.isSuccess() ) return sc;

  m_particleCloner = tool<ICloneMCParticle>( "MCParticleCloner",
                                             this->parent()       );

  return StatusCode::SUCCESS;
  
}
//=============================================================================
//=============================================================================
StatusCode MCVertexCloner::finalize() 
{
  return MicroDSTTool::finalize();
}
//=============================================================================
// Destructor
//=============================================================================
MCVertexCloner::~MCVertexCloner() {} 

//=============================================================================
