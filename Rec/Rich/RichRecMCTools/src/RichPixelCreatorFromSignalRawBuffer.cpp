
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromSignalRawBuffer.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPixelCreatorFromSignalRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromSignalRawBuffer.cpp,v 1.7 2008-02-20 16:10:33 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichPixelCreatorFromSignalRawBuffer.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PixelCreatorFromSignalRawBuffer );

// Standard constructor
PixelCreatorFromSignalRawBuffer::
PixelCreatorFromSignalRawBuffer( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  : Rich::Rec::PixelCreatorBase ( type, name, parent ),
    m_mcTool              ( 0 ),
    m_trackFilter         ( false ),
    m_rejBackHits         ( true  ),
    m_trackMCPsDone       ( false )
{
  // Define job option parameters
  declareProperty( "FilterTracklessDigits", m_trackFilter );
  declareProperty( "RejectBackground", m_rejBackHits );
}

StatusCode PixelCreatorFromSignalRawBuffer::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::PixelCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichMCTruthTool",     m_mcTool    );
  acquireTool( "RichRecMCTruthTool",  m_mcRecTool );

  if ( m_trackFilter )
    info() << "Will remove hits that do not come from tracked particles" << endreq;

  if ( m_rejBackHits )
    info() << "Will reject non Cherenkoc signal hits" << endreq;

  return sc;
}

bool PixelCreatorFromSignalRawBuffer::rejectTrackless( const Rich::HPDPixelCluster& cluster ) const
{
  bool OK = true;
  // if requested, filter trackless hits
  if ( m_trackFilter )
  {
    OK = false;
    std::vector<const LHCb::MCParticle*> mcParts;
    m_mcTool->mcParticles( cluster, mcParts );
    for ( std::vector<const LHCb::MCParticle*>:: const_iterator iMP = mcParts.begin();
          iMP != mcParts.end(); ++iMP )
    {
      if ( *iMP != NULL && trackedMCPs()[*iMP] ) { OK = true; break; }
    }
  } 
  return OK;
}

// Forms a new RichRecPixel object from a RichDigit
LHCb::RichRecPixel *
PixelCreatorFromSignalRawBuffer::buildPixel( const LHCb::RichSmartID & id ) const
{
  // Test if this is a background cluster
  if ( m_rejBackHits && m_mcTool->isBackground(id) ) return NULL;

  // reject trackless hits
  if ( !rejectTrackless(Rich::HPDPixelCluster(id)) ) return NULL;

  // Finally, delegate work to pixel creator
  return RichPixelCreatorBase::buildPixel(id);
}

// Forms a new RichRecPixel object from a RichDigit
LHCb::RichRecPixel *
PixelCreatorFromSignalRawBuffer::buildPixel( const Rich::HPDPixelCluster& cluster ) const
{
  // Test if this is a background cluster
  if ( m_rejBackHits && m_mcTool->isBackground(cluster) ) return NULL;

  // reject trackless hits
  if ( !rejectTrackless(cluster) ) return NULL;

  // Finally, delegate work to pixel creator
  return RichPixelCreatorBase::buildPixel(cluster);
}

PixelCreatorFromSignalRawBuffer::TrackedMCPList &
PixelCreatorFromSignalRawBuffer::trackedMCPs() const
{
  if ( !m_trackMCPsDone )
  {
    m_trackMCPsDone = true;

    // Make sure all RichRecTracks have been formed
    if ( !trackCreator()->newTracks() ) Exception("Error whilst creating RichRecTracks");
    if ( richTracks()->empty() )        Warning( "RichRecTrack container empty !");

    // Loop over reconstructed tracks to form a list of tracked MCParticles
    debug() << "Found " << richTracks()->size() << " RichRecTracks" << endreq;
    for ( LHCb::RichRecTracks::const_iterator iTk = richTracks()->begin();
          iTk != richTracks()->end(); ++iTk )
    {
      if ( !(*iTk) ) continue;
      const LHCb::MCParticle * tkMCP = m_mcRecTool->mcParticle(*iTk);
      verbose() << "RichRecTrack " << (*iTk)->key() << " has MCParticle " << tkMCP << endreq;
      if ( tkMCP ) m_trackedMCPs[tkMCP] = true;
    }

    debug() << "Found " << m_trackedMCPs.size() << " tracked MCParticles" << endreq;

  }

  return m_trackedMCPs;
}

void PixelCreatorFromSignalRawBuffer::InitNewEvent()
{
  // Initialise data for new event
  RichPixelCreatorBase::InitNewEvent();
  m_trackedMCPs.clear();
  m_trackMCPsDone = false;
}
