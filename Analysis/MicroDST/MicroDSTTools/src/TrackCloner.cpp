// $Id: TrackCloner.cpp,v 1.1 2008-04-01 12:34:34 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from LHCb
#include "Event/Track.h"

// local
#include "TrackCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackCloner
//
// 2008-04-01 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackCloner );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackCloner::TrackCloner( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : 
  MicroDSTTool ( type, name , parent )
{

  declareInterface<ICloneTrack>(this);

}
//=============================================================================
StatusCode TrackCloner::initialize() 
{

  debug() << "==> Initialize" << endmsg;

  StatusCode sc = MicroDSTTool::initialize();
  
  if (! sc.isSuccess() ) return sc;
  
  return StatusCode::SUCCESS;
}
//=============================================================================
LHCb::Track* TrackCloner::operator() (const LHCb::Track* protoParticle)
{
  return this->clone(protoParticle);
}
//=============================================================================
LHCb::Track* TrackCloner::clone(const LHCb::Track* track)
{
  LHCb::Track* trackClone = 
    cloneKeyedContainerItem<LHCb::Track, BasicTrackCloner>(track);

  return trackClone;
  
}
//=============================================================================
// Destructor
//=============================================================================
TrackCloner::~TrackCloner() {} 

//=============================================================================
