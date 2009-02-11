// $Id: TupleToolRecoStats.cpp,v 1.1 2009-02-11 18:02:35 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TupleToolRecoStats.h"
#include "Event/ProtoParticle.h"
#include "Event/Track.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolRecoStats
//
// 2009-02-11 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolRecoStats ); 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolRecoStats::TupleToolRecoStats( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTupleTool ( type, name , parent )
{
  declareInterface<IEventTupleTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
TupleToolRecoStats::~TupleToolRecoStats() {} 

//=============================================================================
//=============================================================================
// Fill
//=============================================================================
StatusCode TupleToolRecoStats::fill( Tuples::Tuple& tup) {
  bool test = true;
  test &= tup->column("ChargedProtos",number<LHCb::ProtoParticles>(LHCb::ProtoParticleLocation::Charged));
  test &= tup->column("NeutralProtos",number<LHCb::ProtoParticles>(LHCb::ProtoParticleLocation::Neutrals));
  test &= tup->column("BestTracks",number<LHCb::Tracks>(LHCb::TrackLocation::Default));
  test &= tup->column("MuonTracks",number<LHCb::Tracks>(LHCb::TrackLocation::Muon));
  return StatusCode(test) ;
  
} 
