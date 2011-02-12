// $Id: CombinedPidPlotTool.cpp,v 1.3 2009-05-14 11:25:57 jonrob Exp $
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "CombinedPidPlotTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CombinedPidPlotTool
//
// 2008-12-05 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CombinedPidPlotTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CombinedPidPlotTool::CombinedPidPlotTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : BasePlotTool ( type, name , parent ) { }

//=============================================================================
// Standard destructor
//=============================================================================
CombinedPidPlotTool::~CombinedPidPlotTool( ) {}

//=============================================================================
// Daughter plots - just mass plots
//=============================================================================
StatusCode CombinedPidPlotTool::fillImpl( const LHCb::Particle* p,
                                          const std::string& trailer )
{
  // skip non-stable particles
  if ( !(p->isBasicParticle()) ) return StatusCode::SUCCESS; 

  const LHCb::ParticleProperty* pp = particleProperty( p->particleID() );
  if ( !pp ) return StatusCode::SUCCESS;

  const LHCb::ProtoParticle * proto = p->proto() ;
  if ( !proto ) return StatusCode::SUCCESS;

  llPlots( proto->info(LHCb::ProtoParticle::CombDLLe,  -1000), "CombDLL(e-pi)", 
           p, pp, trailer, 100 );
  llPlots( proto->info(LHCb::ProtoParticle::CombDLLmu, -1000), "CombDLL(mu-pi)", 
           p, pp, trailer, 100 );
  llPlots( proto->info(LHCb::ProtoParticle::CombDLLk,  -1000), "CombDLL(K-pi)",  
           p, pp, trailer, 100 );
  llPlots( proto->info(LHCb::ProtoParticle::CombDLLp,  -1000), "CombDLL(P-pi)",  
           p, pp, trailer, 100 );

  return StatusCode::SUCCESS ;
}
