// $Id: FilterParticlesBase.cpp,v 1.2 2008-03-31 09:23:42 pkoppenb Exp $
// Include files 

// local
#include "Kernel/FilterParticlesBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterParticlesBase
//
// 2007-07-20 : Juan Palacios
//-----------------------------------------------------------------------------

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
