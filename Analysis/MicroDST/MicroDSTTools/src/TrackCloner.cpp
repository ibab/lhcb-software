// $Id: TrackCloner.cpp,v 1.4 2010-08-11 12:52:52 jpalac Exp $

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

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackCloner::TrackCloner( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : base_class ( type, name, parent )
{
  declareProperty("CloneAncestors",m_cloneAncestors=true);
  //setProperty( "OutputLevel", 2 );
}

//=============================================================================
// Destructor
//=============================================================================
TrackCloner::~TrackCloner() {}

//=============================================================================

LHCb::Track* TrackCloner::operator() ( const LHCb::Track* track )
{
  return this->clone(track);
}

//=============================================================================

LHCb::Track* TrackCloner::clone( const LHCb::Track* track )
{
  if ( !track ) 
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Track pointer is NULL !" << endmsg;
    return NULL;
  }

  if ( !track->parent() )
  {
    this->Warning( "Cannot clone a Track with no parent!" ).ignore();
    return NULL;
  }

  // Is this location in the veto list ?
  if ( isVetoed(track) ) { return const_cast<LHCb::Track*>(track); }

  LHCb::Track* cloneTrack = cloneKeyedContainerItem<BasicTrackCloner>(track);
  
  if ( cloneTrack )
  {
    cloneTrack->clearAncestors();
    if ( m_cloneAncestors )
    {
      for ( SmartRefVector<LHCb::Track>::const_iterator iTk = track->ancestors().begin();
            iTk != track->ancestors().end(); ++iTk )
      {
        const LHCb::Track * cloneAnTk = this->clone(*iTk);
        if ( cloneAnTk ) { cloneTrack->addToAncestors(cloneAnTk); }
      }
    }
  }

  return cloneTrack;
}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackCloner )

//=============================================================================
