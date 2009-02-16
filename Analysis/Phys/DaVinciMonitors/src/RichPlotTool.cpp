// $Id: RichPlotTool.cpp,v 1.5 2009-02-16 14:24:37 jonrob Exp $
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
DECLARE_TOOL_FACTORY( RichPlotTool ) ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichPlotTool::RichPlotTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : BasePlotTool ( type, name , parent ),
    m_jos        ( NULL )
{
  declareInterface<IPlotTool>(this);
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

  return sc;
}

//=============================================================================
// Daughter plots - just mass plots
//=============================================================================
StatusCode RichPlotTool::fillImpl( const LHCb::Particle* p,
                                   const std::string& trailer )
{
  // skip composite particles
  if ( !(p->isBasicParticle()) ) return StatusCode::SUCCESS; 

  // Get the info for this particle
  const LHCb::ParticleProperty * prop = particleProperty( p->particleID() );
  // If not available, just abort
  if ( !prop ) { return StatusCode::SUCCESS; }

  // Does this proto have RichPID info ?
  const LHCb::RichPID * rpid = p->proto()->richPID();
  if ( !rpid ) { return StatusCode::SUCCESS; }

  // stable RICH particle type
  const Rich::ParticleIDType pid = pidType(prop);
  
  // fill the plots 
  pidTool(trailer)->plots( rpid, pid );
  
  // return
  return StatusCode::SUCCESS;
}

const Rich::ParticleIDType 
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
    // turn on extra histos for each plots tool
    const std::string fullname = name()+"."+toolname;
    BooleanProperty bp( "ExtraHistos", m_extraHistos );
    StatusCode sc = joSvc()->addPropertyToCatalogue( fullname, bp );
    // refine the histogram path
    StringProperty sp1( "HistoTopDir", "" );
    StringProperty sp2( "HistoDir", '"'+name()+"/"+toolname+'"' );
    sc = sc && joSvc()->addPropertyToCatalogue( fullname, sp1 );
    sc = sc && joSvc()->addPropertyToCatalogue( fullname, sp2 );
    if ( sc.isFailure() ) { Exception( "Error setting properties" ); }
    // get and return the tool
    return m_pidTools[toolname] = tool<Rich::Rec::IPIDPlots>("Rich::Rec::PIDPlots",toolname,this);
  }
  return iT->second;
}
