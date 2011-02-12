// $Id: CaloPlotTool.cpp,v 1.3 2009-05-14 11:25:57 jonrob Exp $
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "CaloPlotTool.h"

using namespace Gaudi::Units;
//-----------------------------------------------------------------------------
// Implementation file for class : CaloPlotTool
//
// 2008-12-05 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloPlotTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloPlotTool::CaloPlotTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : BasePlotTool ( type, name , parent ) { }

//=============================================================================
// Standard destructor
//=============================================================================
CaloPlotTool::~CaloPlotTool( ) {}

//=============================================================================
// Daughter plots - just mass plots
//=============================================================================
StatusCode CaloPlotTool::fillImpl( const LHCb::Particle* p,
                                   const std::string& trailer )
{
  // skip non-stable particles
  if ( !(p->isBasicParticle()) ) return StatusCode::SUCCESS;

  const LHCb::ParticleProperty* pp = particleProperty( p->particleID() );

  const LHCb::ProtoParticle * proto = p->proto() ;
  if ( !proto ) return StatusCode::SUCCESS;

  llPlots( proto->info(LHCb::ProtoParticle::EcalPIDe, -1000), "EcalPIDe",  p, pp, trailer );
  llPlots( proto->info(LHCb::ProtoParticle::EcalPIDmu,-1000), "EcalPIDmu", p, pp, trailer );

  llPlots( proto->info(LHCb::ProtoParticle::HcalPIDe, -1000), "HcalPIDe",  p, pp, trailer );
  llPlots( proto->info(LHCb::ProtoParticle::HcalPIDmu,-1000), "HcalPIDmu", p, pp, trailer );

  llPlots( proto->info(LHCb::ProtoParticle::PrsPIDe,  -1000), "PrsPIDe",   p, pp, trailer );

  llPlots( proto->info(LHCb::ProtoParticle::BremPIDe, -1000), "BremPIDe",  p, pp, trailer );

  return StatusCode::SUCCESS;
}

