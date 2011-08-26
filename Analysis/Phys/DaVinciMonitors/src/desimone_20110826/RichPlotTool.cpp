// $Id: RichPlotTool.cpp,v 1.9 2010-04-26 16:06:34 jonrob Exp $
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichPlotTool.h"

using namespace Gaudi::Units;
//-----------------------------------------------------------------------------
// Implementation file for class : RichPlotTool
//
// 2008-12-05 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichPlotTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichPlotTool::RichPlotTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : BasePlotTool ( type, name , parent ),
    m_jos        ( NULL )
{
  declareProperty( "ExtraHistos", m_extraHistos = true );
}

//=============================================================================
// Standard destructor
//=============================================================================
RichPlotTool::~RichPlotTool( ) {}

//=============================================================================
// initialize
//=============================================================================
StatusCode RichPlotTool::initialize()
{
  const StatusCode sc = BasePlotTool::initialize();
  if ( sc.isFailure() ) return sc;

  // pre-load RichPlot tools for common trailer names ...
  // avoids excessive cpu during first few events
  pidTool("peak");
  pidTool("sideband");

  // PID tool cuts (Should be a JO sometime ...)
  m_pidConfig.minP  =   2 * Gaudi::Units::GeV;
  m_pidConfig.maxP  = 100 * Gaudi::Units::GeV;
  m_pidConfig.minPt =   0 * Gaudi::Units::GeV;
  m_pidConfig.maxPt =  10 * Gaudi::Units::GeV;
  m_pidConfig.expertPlots = m_extraHistos;

  return sc;
}

//=============================================================================
// Daughter plots - just mass plots
//=============================================================================
StatusCode RichPlotTool::fillImpl( const LHCb::Particle* p,
                                   const std::string& trailer )
{
  // skip composite particles
  if ( p->isBasicParticle() )
  {

    // Get the info for this particle
    const LHCb::ParticleProperty * prop = particleProperty( p->particleID() );
    if ( prop )
    {
      // fill the plots
      pidTool(trailer)->plots( p->proto(), pidType(prop), m_pidConfig );
    }

  }

  // return
  return StatusCode::SUCCESS;
}

Rich::ParticleIDType
RichPlotTool::pidType( const LHCb::ParticleProperty * prop ) const
{
  Rich::ParticleIDType type = Rich::Unknown;
  if      ( abs(prop->pythiaID()) == 321  ) type = Rich::Kaon;
  else if ( abs(prop->pythiaID()) == 211  ) type = Rich::Pion;
  else if ( abs(prop->pythiaID()) == 2212 ) type = Rich::Proton;
  else if ( abs(prop->pythiaID()) == 11   ) type = Rich::Electron;
  else if ( abs(prop->pythiaID()) == 13   ) type = Rich::Muon;
  if ( type == Rich::Unknown )
  {
    std::ostringstream mess;
    mess << "Unknown RICH stable particle : " << prop->particle();
    Warning( mess.str() ).ignore();
  }
  return type;
}

IJobOptionsSvc* RichPlotTool::joSvc() const
{
  if (!m_jos)
  { m_jos = this -> svc<IJobOptionsSvc>( "JobOptionsSvc" ); }
  return m_jos;
}

const Rich::Rec::IPIDPlots *
RichPlotTool::pidTool( const std::string & toolname ) const
{
  PIDToolMap::const_iterator iT = m_pidTools.find(toolname);
  if ( iT == m_pidTools.end() )
  {
    const std::string fullname = name()+"."+toolname;
    // refine the histogram path
    StringProperty sp1( "HistoTopDir", "" );
    StringProperty sp2( "HistoDir", '"'+name()+"/"+toolname+'"' );
    const StatusCode sc = ( joSvc()->addPropertyToCatalogue( fullname, sp1 ) &&
                            joSvc()->addPropertyToCatalogue( fullname, sp2 ) );
    if ( sc.isFailure() ) { Exception( "Error setting properties" ); }
    // get and return the tool
    return m_pidTools[toolname] = tool<Rich::Rec::IPIDPlots>("Rich::Rec::PIDPlots",toolname,this);
  }
  return iT->second;
}
