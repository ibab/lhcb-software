// $Id: PidPlotTool.cpp,v 1.5 2009-02-15 13:00:14 jonrob Exp $
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
PidPlotTool::~PidPlotTool( ) {}

//=============================================================================
// Init
//=============================================================================
StatusCode PidPlotTool::initialize()
{
  const StatusCode sc = BasePlotTool::initialize();
  if ( sc.isFailure() ) return sc;

  // book tools etc ...

  return sc;
}

//=============================================================================
// Daughter plots - just mass plots
//=============================================================================
StatusCode PidPlotTool::fillImpl( const LHCb::Particle* p, 
                                  const std::string trailer )
{
  const LHCb::ParticleProperty* pp = particleProperty( p->particleID() );

  plot( p->p()/GeV, histoName("P",pp,trailer),
        "Momentum of "+pp->name()+"_"+trailer, 0, 100 );

  const LHCb::ProtoParticle * proto = p->proto() ;
  if ( !proto ) return StatusCode::SUCCESS;

  // Combined DLLs
  fillPID(proto->info(LHCb::ProtoParticle::CombDLLe, -1000),p->p()/GeV,"e", pp,trailer);
  fillPID(proto->info(LHCb::ProtoParticle::CombDLLmu,-1000),p->p()/GeV,"mu",pp,trailer);
  fillPID(proto->info(LHCb::ProtoParticle::CombDLLk, -1000),p->p()/GeV,"K", pp,trailer);
  fillPID(proto->info(LHCb::ProtoParticle::CombDLLp, -1000),p->p()/GeV,"p", pp,trailer);

  // Muon info
  profile1D( p->p()/GeV, double(proto->muonPID()!=0),
             histoName("MuonPID",pp,trailer),
             "has MuonPID vs p of "+pp->name()+" in "+trailer,0., 100. );
  
  return StatusCode::SUCCESS ;
}

//=========================================================================
//  fill PID
//=========================================================================
void PidPlotTool::fillPID ( double val, double p, 
                            const std::string & part,
                            const LHCb::ParticleProperty* pp, 
                            const std::string & trailer ) 
{
  plot ( val,
         histoName("CombDLL"+part,pp,trailer),
         "CombDLL("+part+"-pi)>0 : "+pp->name()+" in "+trailer,
         -100., 100., nBins() );
  profile1D ( p, double(val>0), 
              histoName("DLLvP"+part,pp,trailer),
              "CombDLL("+part+"-pi)>0 vs Momentum : "+pp->name()+" in "+trailer,
              0., 100., nBins() );
}
