
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromSignalRawBuffer.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPixelCreatorFromSignalRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromSignalRawBuffer.cpp,v 1.1 2006-06-14 22:08:32 jonrob Exp $
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
    m_trackMCPsDone       ( false )
{

  // Define job option parameters
  declareProperty( "FilterTracklessDigits", m_trackFilter );

}

StatusCode RichPixelCreatorFromSignalRawBuffer::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPixelCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichMCTruthTool",     m_mcTool    );
  acquireTool( "RichRecMCTruthTool",  m_mcRecTool );

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
  if ( m_mcTool->isBackground(id) ) return NULL;

  // if requested, filter trackless hits
  if ( m_trackFilter )
  {

    // MC parentage for pixel
    const SmartRefVector<MCRichHit> & hits = m_mcTool->mcRichHits(id);

    // loop over hits and compare MC history to tracked MCParticles
    bool found = false;
    for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
          iHit != hits.end(); ++iHit )
    {
      if ( !(*iHit) ) continue;
      const MCParticle * mcP = (*iHit)->mcParticle();
      if ( mcP && trackedMCPs()[mcP] ) { found = true; break; }
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
    debug() << "Found " <<  richTracks()->size() << " RichRecTracks" << endreq;
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
