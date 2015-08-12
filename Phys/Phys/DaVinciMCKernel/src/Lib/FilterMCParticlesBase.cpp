// $Id$
// Include files 

// local
#include "Kernel/FilterMCParticlesBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterMCParticlesBase
//
// 2007-07-20 : Juan Palacios
//-----------------------------------------------------------------------------

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
