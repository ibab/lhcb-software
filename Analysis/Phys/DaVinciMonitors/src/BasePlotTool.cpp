// $Id: BasePlotTool.cpp,v 1.2 2008-12-10 16:42:17 pkoppenb Exp $
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
  declareProperty("MassWindow", m_massWin = 100*Gaudi::Units::MeV);
}
//=============================================================================
// Destructor
//=============================================================================
BasePlotTool::~BasePlotTool() {} 

//=============================================================================
// Init
//=============================================================================
StatusCode BasePlotTool::initialize(){
  StatusCode sc = GaudiHistoTool::initialize();
  if (!sc) return sc;
  m_ppSvc = svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc", true );
  return sc;
}
//=============================================================================
// Mother plots - just mass plots
//=============================================================================
StatusCode BasePlotTool::fillMother(const LHCb::Particle* p,const std::string trailer){
  
  const LHCb::ParticleProperty* pp = ppSvc()->find( p->particleID() );
  if (0==pp){
    err() << "Unknown PID " << p->particleID() << endmsg ;
    return StatusCode::SUCCESS ;
  }
  double mm = pp->mass() ;
  double em = m_massWin ;

  plot(p->measuredMass(), 
       histoName("M",pp,trailer),"Mass of "+pp->name()+"_"+trailer, mm-em, mm+em);

  return StatusCode::SUCCESS ;
}
