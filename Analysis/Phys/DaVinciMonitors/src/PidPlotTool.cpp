// $Id: PidPlotTool.cpp,v 1.2 2008-12-08 18:12:13 pkoppenb Exp $
// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "PidPlotTool.h"

using namespace Gaudi::Units;
//-----------------------------------------------------------------------------
// Implementation file for class : PidPlotTool
//
// 2008-12-05 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PidPlotTool ) ;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PidPlotTool::PidPlotTool( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : BasePlotTool ( type, name , parent )
{
  declareInterface<IPlotTool>(this);
}
//=============================================================================
// Standard destructor
//=============================================================================
PidPlotTool::~PidPlotTool( ){}; 
//=============================================================================
// Init
//=============================================================================
StatusCode PidPlotTool::initialize(){
  StatusCode sc = BasePlotTool::initialize();
  return sc;
}
//=============================================================================
// Daughter plots - just mass plots
//=============================================================================
StatusCode PidPlotTool::fillFinal(const LHCb::Particle* p,const std::string trailer){
  
  const LHCb::ParticleProperty* pp = ppSvc()->find( p->particleID() );
  plot(p->p(), histoName("P",pp,trailer), 
       "Momentum of "+pp->name()+"_"+trailer, 0, 100*GeV);
  const LHCb::ProtoParticle* proto = p->proto() ;
  if ( 0==proto) return StatusCode::SUCCESS;
  
  fillPID(proto->info(LHCb::ProtoParticle::CombDLLe, -1000),p->p(),"e", pp,trailer);
  fillPID(proto->info(LHCb::ProtoParticle::CombDLLmu,-1000),p->p(),"mu",pp,trailer);
  fillPID(proto->info(LHCb::ProtoParticle::CombDLLk, -1000),p->p(),"K", pp,trailer);
  fillPID(proto->info(LHCb::ProtoParticle::CombDLLp, -1000),p->p(),"p", pp,trailer);

  profile1D(p->p(), (proto->muonPID()!=0), 
       histoName("MuonPID_p",pp,trailer),
       "has MuonPID vs p of "+pp->name()+"_"+trailer,-10., 10.);

  return StatusCode::SUCCESS ;
}

//=========================================================================
//  
//=========================================================================
void PidPlotTool::fillPID ( double val, double p, std::string part, 
                            const LHCb::ParticleProperty* pp, std::string trailer) {
  plot(val,histoName("DLL"+part,pp,trailer),
       part+" DLL of "+pp->name()+"_"+trailer,-10., 10.);
  profile1D(p, val>0, histoName("DLL"+part+"_p",pp,trailer),
       part+" DLL>0 vs p of "+pp->name()+"_"+trailer,-10., 10.);
  return ;
  
}
