// $Id: MassPlotTool.cpp,v 1.1 2009-02-13 12:28:16 jonrob Exp $
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "MassPlotTool.h"

using namespace Gaudi::Units;
//-----------------------------------------------------------------------------
// Implementation file for class : MassPlotTool
//
// 2008-12-05 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MassPlotTool ) ;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MassPlotTool::MassPlotTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : BasePlotTool ( type, name , parent )
{
  declareInterface<IPlotTool>(this);
}

//=============================================================================
// Standard destructor
//=============================================================================
MassPlotTool::~MassPlotTool( ) {}

//=============================================================================
// Daughter plots - just mass plots
//=============================================================================
StatusCode MassPlotTool::fillImpl( const LHCb::Particle* p,
                                   const std::string trailer )
{
  // skip stable particles
  if ( p->isBasicParticle() ) return StatusCode::SUCCESS; 

  const LHCb::ParticleProperty* pp = ppSvc()->find( p->particleID() );
  if (0==pp)
  {
    err() << "Unknown PID " << p->particleID() << endmsg ;
    return StatusCode::SUCCESS ;
  }

  const double mm = pp->mass() ;
  const double em = mm*0.1; // CRJ : Arbitary I know. To be improved ...

  plot( p->measuredMass(),
        histoName("M",pp,trailer),"Mass of "+pp->name()+"_"+trailer, mm-em, mm+em);
  
  return StatusCode::SUCCESS;
}
