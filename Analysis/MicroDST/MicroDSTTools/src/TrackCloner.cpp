
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
  : base_class  ( type, name, parent ),
    m_mcPcloner ( NULL )
{
  declareProperty( "ICloneMCParticle",
                   m_mcpClonerName = "MCParticleCloner" );
  declareProperty( "CloneAncestors", m_cloneAncestors = true  );
  declareProperty( "CloneMCLinks",   m_cloneMCLinks   = false );
  //setProperty( "OutputLevel", 1 );
}

//=============================================================================
// Destructor
//=============================================================================
TrackCloner::~TrackCloner() {}

//=============================================================================

StatusCode TrackCloner::initialize()
{
  const StatusCode sc = base_class::initialize();
  if ( sc.isFailure() ) return sc;

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // MC stuff
  if ( m_cloneMCLinks ) { debug() << "Will clone MC Links" << endmsg; }

  return sc;
}

//=============================================================================

// Method that handles various Gaudi "software events"
void TrackCloner::handle ( const Incident& /* incident */ )
{
  // Only one Incident type, so skip type check
  clonedTrackList().clear();
}

//=============================================================================

LHCb::Track* TrackCloner::operator() ( const LHCb::Track* track )
{
  return this->clone(track);
}

//=============================================================================

LHCb::Track* TrackCloner::clone ( const LHCb::Track* track )
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

  LHCb::Track * cloneTrack = cloneKeyedContainerItem<BasicTrackCloner>(track);

  // Did the cloning work ?
  if ( cloneTrack )
  {

    // If so clone ancestors if required
    cloneTrack->clearAncestors();
    if ( m_cloneAncestors )
    {
      for ( const SmartRef<LHCb::Track>& Tk : track->ancestors() )
      {
        const LHCb::Track * cloneAnTk = this->clone(Tk);
        if ( cloneAnTk ) { cloneTrack->addToAncestors(cloneAnTk); }
      }
    }

    // Clone MC links ?
    if ( m_cloneMCLinks ) { cloneMCLinks(track,cloneTrack); }

  }

  return cloneTrack;
}

//=============================================================================

void TrackCloner::cloneMCLinks( const LHCb::Track* track,
                                const LHCb::Track* cloneTrack )
{

  // Linker typedefs
  typedef LinkerWithKey<LHCb::MCParticle,LHCb::Track>  Linker;
  typedef LinkerTool<LHCb::Track,LHCb::MCParticle>     Asct;
  typedef Asct::DirectType                             Table;

  // has this clone already been done
  if ( std::find( clonedTrackList().begin(),
                  clonedTrackList().end(),
                  cloneTrack ) == clonedTrackList().end() )
  {

    // location in TES of original tracks
    const std::string tkLoc = objectLocation( track->parent() );

    // try and load the linker tool
    Asct linker = Asct( evtSvc(), tkLoc );
    const Table * table = linker.direct();

    // If we found a table, try and use it
    if ( table )
    {
      // location in TES of cloned tracks
      const std::string cloneLoc = objectLocation( cloneTrack->parent() );

      // Create a new linker for the cloned tracks
      Linker clonedLinks( evtSvc(), msgSvc(), cloneLoc );

      // Loop over relations for original track
      for ( const auto& Rentry : table->relations(track) )
      {
        // get cloned MCParticle
        const LHCb::MCParticle * clonedMCP = mcPCloner()( Rentry.to() );
        if ( clonedMCP )
        {
          // if cloning worked, fill relation in linker with original weight
          clonedLinks.link( cloneTrack, clonedMCP, Rentry.weight() );
        }
      }
    }

    // save in the list
    clonedTrackList().push_back(cloneTrack);

  }

}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackCloner )

//=============================================================================
