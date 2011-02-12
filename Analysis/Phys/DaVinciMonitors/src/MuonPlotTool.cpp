// $Id: MuonPlotTool.cpp,v 1.4 2009-05-14 11:25:57 jonrob Exp $
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "MuonPlotTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonPlotTool
//
// 2008-12-05 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MuonPlotTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonPlotTool::MuonPlotTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : BasePlotTool ( type, name , parent ) { }

//=============================================================================
// Standard destructor
//=============================================================================
MuonPlotTool::~MuonPlotTool( ) {}

//=============================================================================
// Daughter plots - just mass plots
//=============================================================================
StatusCode MuonPlotTool::fillImpl( const LHCb::Particle* p,
                                   const std::string& trailer )
{
  // skip non-stable particles
  if ( !(p->isBasicParticle()) ) return StatusCode::SUCCESS;

  const LHCb::ParticleProperty* pp = particleProperty( p->particleID() );
  if ( !pp ) return StatusCode::SUCCESS;

  const LHCb::ProtoParticle * proto = p->proto();
  if ( !proto ) return StatusCode::SUCCESS;

  // HasMuon efficiency
  profile1D( p->p()/Gaudi::Units::GeV,
             double(proto->muonPID()!=NULL),
             histoName("MuonPID",pp,trailer),
             "Has MuonPID vs p of "+pp->name()+" in "+trailer,
             0., 100., nBins() );

  llPlots( proto->info(LHCb::ProtoParticle::MuonMuLL, -1000), "MuonMuLL",  p, pp, trailer );
  llPlots( proto->info(LHCb::ProtoParticle::MuonBkgLL,-1000), "MuonBkgLL", p, pp, trailer );

  return StatusCode::SUCCESS;
}
