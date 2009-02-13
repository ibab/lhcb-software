// $Id: BasePlotTool.cpp,v 1.3 2009-02-13 12:28:16 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "BasePlotTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BasePlotTool
//
// 2008-12-05 : Patrick Koppenburg
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BasePlotTool::BasePlotTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiHistoTool ( type, name , parent )
  , m_ppSvc(0)
{
  declareInterface<IPlotTool>(this);
  declareProperty( "HistoBins", m_bins = 50 );
}

//=============================================================================
// Destructor
//=============================================================================
BasePlotTool::~BasePlotTool() {}

//=============================================================================
// Init
//=============================================================================
StatusCode BasePlotTool::initialize()
{
  StatusCode sc = GaudiHistoTool::initialize();
  if (!sc) return sc;
  m_ppSvc = svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc", true );
  return sc;
}

//=============================================================================
// Fill plots using a LHCb::Particle::ConstVector
//=============================================================================
StatusCode BasePlotTool::fillPlots( const LHCb::Particle::ConstVector& pv,
                                    const std::string where )
{
  for ( LHCb::Particle::ConstVector::const_iterator p = pv.begin();
        p != pv.end() ; ++p )
  {
    if ( !fillPlots(*p,where) ) return StatusCode::FAILURE ;
  }
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Fill plots using a single Particle
//=============================================================================
StatusCode BasePlotTool::fillPlots(const LHCb::Particle* p, const std::string where )
{
  return ( p->isBasicParticle() ? 
           fillImpl(p,where) : 
           StatusCode(fillImpl(p,where) && fillPlots(p->daughtersVector(),where)) );
}

//=============================================================================
// Returns the ParticleProperty object for a given ParticleID
//=============================================================================
const LHCb::ParticleProperty *
BasePlotTool::particleProperty( const LHCb::ParticleID& id ) const
{
  const LHCb::ParticleProperty * prop = ppSvc()->find ( id );
  if ( NULL == prop )
  {
    std::ostringstream mess;
    mess << "LHCb::ParticleProperty missing for ParticleID " << id;
    Warning( mess.str(), StatusCode::SUCCESS, 1 );
  }
  return prop;
}
