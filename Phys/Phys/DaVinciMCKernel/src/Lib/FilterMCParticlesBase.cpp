// $Id: FilterMCParticlesBase.cpp,v 1.1 2007-07-24 12:09:22 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "Kernel/FilterMCParticlesBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterMCParticlesBase
//
// 2007-07-20 : Juan Palacios
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FilterMCParticlesBase );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterMCParticlesBase::FilterMCParticlesBase( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IFilterMCParticles>(this);

}
//=============================================================================
bool FilterMCParticlesBase::operator()( const LHCb::MCParticle::ConstVector& parts) const
{
  return isSatisfied(parts);
}
//=============================================================================
bool FilterMCParticlesBase::isSatisfied( const LHCb::MCParticle::ConstVector& /*parts*/) const
{
  return true;
}
//=============================================================================
// Destructor
//=============================================================================
FilterMCParticlesBase::~FilterMCParticlesBase() {} 

//=============================================================================
