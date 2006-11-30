
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromSignalRawBuffer.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPixelCreatorFromSignalRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromSignalRawBuffer.cpp,v 1.2 2006-11-30 15:29:26 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */
//-----------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromSignalRawBuffer.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromSignalRawBuffer>          s_factory ;
const        IToolFactory& RichPixelCreatorFromSignalRawBufferFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromSignalRawBuffer::
RichPixelCreatorFromSignalRawBuffer( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent )
  : RichPixelCreatorBase  ( type, name, parent ),
    m_mcTool              ( 0 ),
    m_trackFilter         ( false ),
    m_rejBackHits         ( true  ),
    m_trackMCPsDone       ( false )
{

  // Define job option parameters
  declareProperty( "FilterTracklessDigits", m_trackFilter );
  declareProperty( "RejectBackground", m_rejBackHits );

}

StatusCode RichPixelCreatorFromSignalRawBuffer::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPixelCreatorBase::initialize();
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

StatusCode RichPixelCreatorFromSignalRawBuffer::finalize()
{
  // Execute base class method
  return RichPixelCreatorBase::finalize();
}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel *
RichPixelCreatorFromSignalRawBuffer::buildPixel( const RichSmartID id ) const
{

  // Test if this is a background hit
  if ( m_rejBackHits && m_mcTool->isBackground(id) ) return NULL;

  // if requested, filter trackless hits
  if ( m_trackFilter )
  {
    std::vector<const LHCb::MCParticle*> mcParts;
    m_mcTool->mcParticles( id, mcParts );
    bool found = false;
    for ( std::vector<const LHCb::MCParticle*>:: const_iterator iMP = mcParts.begin(); 
          iMP != mcParts.end(); ++iMP )
    {
      if ( *iMP != NULL && trackedMCPs()[*iMP] ) { found = true; break; }
    } 
    // If no associated tracked MCParticle found, return NULL
    if ( !found ) return NULL;
  }

  // Finally, delegate work to pixel creator
  return RichPixelCreatorBase::buildPixel(id);
}

RichPixelCreatorFromSignalRawBuffer::TrackedMCPList &
RichPixelCreatorFromSignalRawBuffer::trackedMCPs() const
{
  if ( !m_trackMCPsDone )
  {
    m_trackMCPsDone = true;

    // Make sure all RichRecTracks have been formed
    if ( !trackCreator()->newTracks() ) Exception("Error whilst creating RichRecTracks");
    if ( richTracks()->empty() )        Warning( "RichRecTrack container empty !");

    // Loop over reconstructed tracks to form a list of tracked MCParticles
    debug() << "Found " << richTracks()->size() << " RichRecTracks" << endreq;
    for ( RichRecTracks::const_iterator iTk = richTracks()->begin();
          iTk != richTracks()->end(); ++iTk )
    {
      if ( !(*iTk) ) continue;
      const MCParticle * tkMCP = m_mcRecTool->mcParticle(*iTk);
      verbose() << "RichRecTrack " << (*iTk)->key() << " has MCParticle " << tkMCP << endreq;
      if ( tkMCP ) m_trackedMCPs[tkMCP] = true;
    }

    debug() << "Found " << m_trackedMCPs.size() << " tracked MCParticles" << endreq;

  }

  return m_trackedMCPs;
}

void RichPixelCreatorFromSignalRawBuffer::InitNewEvent()
{
  // Initialise data for new event
  RichPixelCreatorBase::InitNewEvent();
  m_trackedMCPs.clear();
  m_trackMCPsDone = false;
}
