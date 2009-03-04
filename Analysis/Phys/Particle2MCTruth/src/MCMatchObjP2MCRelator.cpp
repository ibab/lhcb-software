// $Id: MCMatchObjP2MCRelator.cpp,v 1.1.1.1 2009-03-04 14:26:10 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "MCMatchObjP2MCRelator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCMatchObjP2MCRelator
//
// 2009-03-04 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCMatchObjP2MCRelator );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCMatchObjP2MCRelator::MCMatchObjP2MCRelator( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : 
  Particle2MCAssociatorBase( type, name , parent )
{
  declareInterface<IParticle2MCAssociator>(this);

}
//=============================================================================
StatusCode MCMatchObjP2MCRelator::initialize()
{
  return Particle2MCAssociatorBase::initialize();
}
//=============================================================================
StatusCode MCMatchObjP2MCRelator::finalize()
{
  return Particle2MCAssociatorBase::finalize();
}
//=============================================================================
bool MCMatchObjP2MCRelator::isMatched(const LHCb::Particle* particle, 
                                      const LHCb::MCParticle* mcParticle) const
{
  return false;
}
//=============================================================================
// Destructor
//=============================================================================
MCMatchObjP2MCRelator::~MCMatchObjP2MCRelator() {}
//=============================================================================
