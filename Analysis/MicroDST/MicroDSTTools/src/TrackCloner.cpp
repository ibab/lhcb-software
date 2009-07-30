// $Id: TrackCloner.cpp,v 1.3 2009-07-30 10:03:01 jpalac Exp $
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
  base_class ( type, name , parent )
{
}
//=============================================================================
StatusCode TrackCloner::initialize() 
{

  debug() << "==> Initialize" << endmsg;

  StatusCode sc = base_class::initialize();
  
  if (! sc.isSuccess() ) return sc;
  
  return StatusCode::SUCCESS;
}
//=============================================================================
LHCb::Track* TrackCloner::operator() (const LHCb::Track* track)
{
  return this->clone(track);
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
