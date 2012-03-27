// $Id: TrackCloner.cpp,v 1.4 2010-08-11 12:52:52 jpalac Exp $
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

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackCloner::TrackCloner( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : base_class ( type, name, parent )
{
  declareProperty("TESVetoList",m_tesVetoList);
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

  // Get the parent location of the Track
  const std::string & tkLoc = track->parent()->registry()->identifier();

  // Is this location in the veto list ?
  if ( !m_tesVetoList.empty() &&
       m_tesVetoList.end() != std::find( m_tesVetoList.begin(),
                                         m_tesVetoList.end(),
                                         tkLoc ) )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Track at " << tkLoc
              << " is VETO'ed from cloning. Returning original pointer" << endmsg;
    return const_cast<LHCb::Track*>(track);
  }

  LHCb::Track* cloneTrack = cloneKeyedContainerItem<BasicTrackCloner>(track);

//   if ( !track->ancestors().empty() )
//   {
//     info() << "Track at " << tkLoc << " has ancestors" << endmsg;
//     for ( SmartRefVector<LHCb::Track>::const_iterator iTk = track->ancestors().begin();
//           iTk != track->ancestors().end(); ++iTk )
//     {
//       if ( (*iTk) && (*iTk)->parent() )
//       {
//         info() << "  -> " << (*iTk)->key() << " " 
//                << (*iTk)->parent()->registry()->identifier() << endmsg;
//       }
//     }
//   }
  
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
