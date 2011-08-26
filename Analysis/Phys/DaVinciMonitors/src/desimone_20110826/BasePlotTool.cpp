// $Id: BasePlotTool.cpp,v 1.4 2009-02-15 17:37:38 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "BasePlotTool.h"

// boost
#include "boost/lexical_cast.hpp"

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
  // interface
  declareInterface<IPlotTool>(this);
  // JOs
  declareProperty( "DllCut", m_dllCut = 0 );
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

// PID DLL plots
void BasePlotTool::llPlots( const double var, 
                            const std::string & varName,
                            const LHCb::Particle* particle,
                            const LHCb::ParticleProperty* pp,
                            const std::string & trailer,
                            const double dllLimit ) const
{
  plot1D ( var,
           histoName(varName,pp,trailer),
           varName+" : "+pp->name()+" in "+trailer,
           -dllLimit, dllLimit, nBins() );
  profile1D ( particle->p()/Gaudi::Units::GeV, double(var>m_dllCut),
              histoName(varName+"vP",pp,trailer),
              varName+">"
              +boost::lexical_cast<std::string>(m_dllCut)+
              " vs Momentum : "+pp->name()+" in "+trailer,
              0., 100., nBins() );
}
