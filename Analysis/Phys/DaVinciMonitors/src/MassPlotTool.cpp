// $Id: MassPlotTool.cpp,v 1.6 2009-05-14 11:25:57 jonrob Exp $
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
DECLARE_TOOL_FACTORY( MassPlotTool ) 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MassPlotTool::MassPlotTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : BasePlotTool ( type, name , parent )
{
  // job options
  declareProperty( "RecoResolution", m_recoRes = 20.0*Gaudi::Units::MeV );
  declareProperty( "NSigmaMassRange", m_nSigma = 5 );
}

//=============================================================================
// Standard destructor
//=============================================================================
MassPlotTool::~MassPlotTool( ) {}

//=============================================================================
// Daughter plots - just mass plots
//=============================================================================
StatusCode MassPlotTool::fillImpl( const LHCb::Particle* p,
                                   const std::string& trailer )
{
  // skip stable particles
  if ( p->isBasicParticle() ) return StatusCode::SUCCESS; 

  const LHCb::ParticleProperty* pp = particleProperty( p->particleID() );
  if (!pp) { return StatusCode::SUCCESS; }

  const double mm = pp->mass() ;
  const double wm = pp->width() ;
  const double em = m_nSigma * std::sqrt( wm*wm + m_recoRes*m_recoRes );  // (width + resolution)

  plot( p->measuredMass(),
        histoName("M",pp,trailer),"Mass of "+pp->name()+"_"+trailer, 
        mm-em, mm+em, nBins() );
  
  return StatusCode::SUCCESS;
}
