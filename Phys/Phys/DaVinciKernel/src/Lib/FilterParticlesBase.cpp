// $Id: FilterParticlesBase.cpp,v 1.1 2007-07-20 14:55:14 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "Kernel/FilterParticlesBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterParticlesBase
//
// 2007-07-20 : Juan Palacios
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FilterParticlesBase );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterParticlesBase::FilterParticlesBase( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IFilterParticles>(this);

}
//=============================================================================
bool FilterParticlesBase::operator()( const LHCb::Particle::ConstVector& parts) const
{
  return isSatisfied(parts);
}
//=============================================================================
bool FilterParticlesBase::isSatisfied( const LHCb::Particle::ConstVector& /*parts*/) const
{
  return true;
}
//=============================================================================
// Destructor
//=============================================================================
FilterParticlesBase::~FilterParticlesBase() {} 

//=============================================================================
