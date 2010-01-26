// $Id: TupleToolRecoStats.cpp,v 1.2 2010-01-26 15:39:26 rlambert Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TupleToolRecoStats.h"
#include "Event/ProtoParticle.h"
#include "Event/Track.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
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
  : TupleToolBase ( type, name , parent )
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
StatusCode TupleToolRecoStats::fill( Tuples::Tuple& tup) 
{
  const std::string prefix=fullName();
  
  bool test = true;
  test &= tup->column(prefix+"ChargedProtos",number<LHCb::ProtoParticles>(LHCb::ProtoParticleLocation::Charged));
  test &= tup->column(prefix+"NeutralProtos",number<LHCb::ProtoParticles>(LHCb::ProtoParticleLocation::Neutrals));
  test &= tup->column(prefix+"BestTracks",number<LHCb::Tracks>(LHCb::TrackLocation::Default));
  test &= tup->column(prefix+"MuonTracks",number<LHCb::Tracks>(LHCb::TrackLocation::Muon));
  return StatusCode(test) ;
  
} 
