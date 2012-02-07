// $Id: ANNPIDTool.cpp,v 1.3 2009-05-14 11:25:57 jonrob Exp $
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "ANNPIDTool.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ANNPIDTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ANNPIDTool::ANNPIDTool( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  : BasePlotTool ( type, name , parent ) { }

//=============================================================================
// Standard destructor
//=============================================================================
ANNPIDTool::~ANNPIDTool( ) {}

//=============================================================================
// Daughter plots - just mass plots
//=============================================================================
StatusCode ANNPIDTool::fillImpl( const LHCb::Particle* p,
                                 const std::string& trailer )
{
  // skip non-stable particles
  if ( !(p->isBasicParticle()) ) return StatusCode::SUCCESS;

  const LHCb::ParticleProperty * pp = particleProperty( p->particleID() );
  if ( !pp ) return StatusCode::SUCCESS;

  const LHCb::ProtoParticle * proto = p->proto();
  if ( !proto ) return StatusCode::SUCCESS;



  return StatusCode::SUCCESS ;
}
