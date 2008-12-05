// $Id: PidPlotTool.cpp,v 1.1.1.1 2008-12-05 16:41:05 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "Kernel/ParticleProperty.h" 
#include "Kernel/IParticlePropertySvc.h" 

// local
#include "PidPlotTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PidPlotTool
//
// 2008-12-05 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PidPlotTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PidPlotTool::PidPlotTool( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiHistoTool ( type, name , parent )
, m_ppSvc(0)
{
  declareInterface<IPlotTool>(this);
  declareProperty("MassWindow", m_massWin = 100*Gaudi::Units::MeV);
}
//=============================================================================
// Destructor
//=============================================================================
PidPlotTool::~PidPlotTool() {} 

//=============================================================================
// Init
//=============================================================================
StatusCode PidPlotTool::initialize(){
  StatusCode sc = GaudiHistoTool::initialize();
  if (!sc) return sc;
  m_ppSvc = svc<LHCb::IParticlePropertySvc>( "ParticlePropertySvc", true );
  return sc;
}
//=============================================================================
// Fill plots using a single Particle
//=============================================================================
StatusCode PidPlotTool::fillPlots(const LHCb::Particle* p,const std::string trailer){
  if ( p->isBasicParticle ()) return fillFinal(p,trailer);
  else return fillMother(p,trailer);
}
//=============================================================================
// Mother plots - just mass plots
//=============================================================================
StatusCode PidPlotTool::fillMother(const LHCb::Particle* p,const std::string trailer){
  
  const LHCb::ParticleProperty* pp = m_ppSvc->find( p->particleID() );
  if (0==pp){
    err() << "Unknown PID " << p->particleID() << endmsg ;
    return StatusCode::SUCCESS ;
  }
  double mm = pp->mass() ;
  double em = m_massWin ;

  plot(p->measuredMass(), "Mass of "+pp->name()+"_"+trailer, mm-em, mm+em);

  return StatusCode::SUCCESS ;
}
//=============================================================================
// Daughter plots - just mass plots
//=============================================================================
StatusCode PidPlotTool::fillFinal(const LHCb::Particle* p,const std::string trailer){
  
  return StatusCode::SUCCESS ;
}
