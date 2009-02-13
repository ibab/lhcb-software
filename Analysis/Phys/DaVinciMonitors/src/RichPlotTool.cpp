// $Id: RichPlotTool.cpp,v 1.1 2009-02-13 12:28:16 jonrob Exp $
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
    m_jos        ( NULL ),
    m_plots      ( NULL )
{
  declareInterface<IPlotTool>(this);
  declareProperty( "ExtraHistos", m_extraHistos = true ); 
}

//=============================================================================
// Standard destructor
//=============================================================================
RichPlotTool::~RichPlotTool( ) {}

//=============================================================================
// Init
//=============================================================================
StatusCode RichPlotTool::initialize()
{
  StatusCode sc = BasePlotTool::initialize();
  if ( sc.isFailure() ) return sc;

  // turn on extra histos for the RICH plot tool
  const std::string toolname = "RichPlots";
  BooleanProperty p( "ExtraHistos", m_extraHistos );
  sc = joSvc()->addPropertyToCatalogue( name()+"."+toolname, p );
  if ( sc.isFailure() ) { this -> Error( "Error setting property" ); }
  m_plots = tool<Rich::Rec::IPIDPlots>("Rich::Rec::PIDPlots", toolname, this );

  return sc;
}

//=============================================================================
// Daughter plots - just mass plots
//=============================================================================
StatusCode RichPlotTool::fillImpl( const LHCb::Particle* p,
                                   const std::string /* trailer */ )
{
  // skip composite particles
  if ( !(p->isBasicParticle()) ) return StatusCode::SUCCESS; 

  // Get the info for this particle
  const LHCb::ParticleProperty * prop = particleProperty( p->particleID() );
  // If not available, just abort
  if ( !prop ) { return StatusCode::SUCCESS; }

  // stable RICH particle type
  const Rich::ParticleIDType pid = pidType(prop);
  if ( pid == Rich::Unknown )
  {
    std::ostringstream mess;
    mess << "Unknown RICH stable particle : " << prop->particle();
    Warning( mess.str() ).ignore();
  }
  
  // fill the plots
  m_plots->plots( p->proto()->richPID(), pid );
  
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
  return type;
}

IJobOptionsSvc* RichPlotTool::joSvc() const
{
  if (!m_jos)
  { m_jos = this -> svc<IJobOptionsSvc>( "JobOptionsSvc" ); }
  return m_jos;
}
