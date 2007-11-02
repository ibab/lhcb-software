// $Id: CopyRelatedMCParticles.cpp,v 1.1 2007-11-02 16:28:24 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
// from LHCb
#include "Event/Particle.h"
#include "Event/MCParticle.h"
// local
#include "CopyRelatedMCParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyRelatedMCParticles
//
// 2007-11-02 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CopyRelatedMCParticles );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyRelatedMCParticles::CopyRelatedMCParticles( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IDoSomethingToParticle>(this);

}
//=============================================================================
StatusCode CopyRelatedMCParticles::operator()  (LHCb::Particle* particle)
{
  return this->doSomething(particle);
}
//=============================================================================
StatusCode CopyRelatedMCParticles::doSomething (LHCb::Particle* particle)
{
  return StatusCode::SUCCESS;
}
//=============================================================================
// Destructor
//=============================================================================
CopyRelatedMCParticles::~CopyRelatedMCParticles() {} 

//=============================================================================
