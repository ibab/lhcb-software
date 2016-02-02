
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromSignalRawBuffer.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPixelCreatorFromSignalRawBuffer
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */
//-----------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromSignalRawBuffer.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PixelCreatorFromSignalRawBuffer )

// Standard constructor
PixelCreatorFromSignalRawBuffer::
PixelCreatorFromSignalRawBuffer( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  : Rich::Rec::PixelCreatorBase ( type, name, parent )
{
  // Define job option parameters
  declareProperty( "FilterTracklessDigits", m_trackFilter   = false );
  declareProperty( "RejectAllBackgrounds", m_rejAllBackHits = true  );
  declareProperty( "RejectHPDReflections", m_rejHPDReflHits = true  );
  declareProperty( "RejectBackScatter", m_rejHPDBackScatter = true  );
}

StatusCode PixelCreatorFromSignalRawBuffer::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::PixelCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichMCTruthTool",     m_mcTool, nullptr, true );
  acquireTool( "RichRecMCTruthTool",  m_mcRecTool );

  if ( m_trackFilter )
    info() << "Will remove hits that do not come from tracked particles" << endmsg;

  if ( m_rejAllBackHits )
  {
    info() << "Will reject ALL non Cherenkov signal hits" << endmsg;
  }
  else
  {
    if ( m_rejHPDReflHits )
      info() << "Will reject HPD reflections" << endmsg;
    if ( m_rejHPDBackScatter )
      info() << "Will reject back-scattered hits" << endmsg;
  }

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
    for ( const auto * MP : mcParts )
    {
      if ( MP && trackedMCPs()[MP] ) { OK = true; break; }
    }
  }
  return OK;
}

// Forms a new RichRecPixel object from a RichDigit
LHCb::RichRecPixel *
PixelCreatorFromSignalRawBuffer::buildPixel( const LHCb::RichSmartID & id ) const
{
  // Test if this is a background cluster
  if ( m_rejAllBackHits && m_mcTool->isBackground(id) ) return nullptr;

  // HPD reflection ?
  if ( m_rejHPDReflHits && m_mcTool->isHPDReflection(id) ) return nullptr;

  // Si back-scatter ?
  if ( m_rejHPDBackScatter && m_mcTool->isSiBackScatter(id) ) return nullptr;

  // reject trackless hits
  if ( !rejectTrackless(Rich::HPDPixelCluster(id)) ) return nullptr;

  // Finally, delegate work to pixel creator
  return PixelCreatorBase::buildPixel(id);
}

// Forms a new RichRecPixel object from a RichDigit
LHCb::RichRecPixel *
PixelCreatorFromSignalRawBuffer::buildPixel( const Rich::HPDPixelCluster& cluster ) const
{
  // Test if this is a background cluster
  if ( m_rejAllBackHits && m_mcTool->isBackground(cluster) ) return nullptr;

  // HPD reflection ?
  if ( m_rejHPDReflHits && m_mcTool->isHPDReflection(cluster) ) return nullptr;

  // Si back-scatter ?
  if ( m_rejHPDBackScatter && m_mcTool->isSiBackScatter(cluster) ) return nullptr;

  // reject trackless hits
  if ( !rejectTrackless(cluster) ) return nullptr;

  // Finally, delegate work to pixel creator
  return PixelCreatorBase::buildPixel(cluster);
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
    _ri_debug << "Found " << richTracks()->size() << " RichRecTracks" << endmsg;
    for ( const auto * tk : *richTracks() )
    {
      if ( !tk ) continue;
      const auto * tkMCP = m_mcRecTool->mcParticle(tk);
      _ri_verbo << "RichRecTrack " << tk->key() << " has MCParticle " << tkMCP << endmsg;
      if ( tkMCP ) m_trackedMCPs[tkMCP] = true;
    }

    _ri_debug << "Found " << m_trackedMCPs.size() << " tracked MCParticles" << endmsg;

  }

  return m_trackedMCPs;
}

void PixelCreatorFromSignalRawBuffer::InitNewEvent()
{
  // Initialise data for new event
  PixelCreatorBase::InitNewEvent();
  m_trackedMCPs.clear();
  m_trackMCPsDone = false;
}
