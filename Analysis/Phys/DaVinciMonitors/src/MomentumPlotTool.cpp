// $Id: MomentumPlotTool.cpp,v 1.3 2009-05-14 11:25:57 jonrob Exp $
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "MomentumPlotTool.h"

using namespace Gaudi::Units;
//-----------------------------------------------------------------------------
// Implementation file for class : MomentumPlotTool
//
// 2008-12-05 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MomentumPlotTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MomentumPlotTool::MomentumPlotTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : BasePlotTool ( type, name , parent ) { }

//=============================================================================
// Standard destructor
//=============================================================================
MomentumPlotTool::~MomentumPlotTool( ) {}

//=============================================================================
// Daughter plots - just mass plots
//=============================================================================
StatusCode MomentumPlotTool::fillImpl( const LHCb::Particle* p,
                                       const std::string& trailer )
{
  const LHCb::ParticleProperty* pp = particleProperty( p->particleID() );
  if ( !pp ) return StatusCode::SUCCESS;

  plot1D( p->p()/Gaudi::Units::GeV,
          histoName("P",pp,trailer),
          "Ptot of "+pp->name()+" in "+trailer,
          0, 100, nBins() );
  plot1D( p->pt()/Gaudi::Units::GeV,
          histoName("Pt",pp,trailer),
          "Pt of "+pp->name()+" in "+trailer,
          0, 10, nBins() );

  return StatusCode::SUCCESS;
}
