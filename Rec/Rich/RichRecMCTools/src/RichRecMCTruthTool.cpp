
//-----------------------------------------------------------------------------
/** @file RichRecMCTruthTool.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichRecMCTruthTool
 *
 *  CVS Log :-
 *  $Id: RichRecMCTruthTool.cpp,v 1.24 2006-12-01 16:18:24 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"

// local
#include "RichRecMCTruthTool.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( RichRecMCTruthTool );

// Standard constructor
RichRecMCTruthTool::RichRecMCTruthTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_truth         ( 0 ),
    m_trToMCPLinks  ( 0 ),
    m_trLoc         ( TrackLocation::Default )
{
  // interface
  declareInterface<IRichRecMCTruthTool>(this);
  // job options
  declareProperty( "TrackLocation", m_trLoc );
}

// destructor
RichRecMCTruthTool::~RichRecMCTruthTool() { cleanUpLinkers(); }

// initalization
StatusCode RichRecMCTruthTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Acquire instances of tools
  acquireTool( "RichMCTruthTool", m_truth );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  return sc;
}

StatusCode RichRecMCTruthTool::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichRecMCTruthTool::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
}

RichRecMCTruthTool::TrackToMCP *
RichRecMCTruthTool::trackToMCPLinks() const
{
  if ( !m_trToMCPLinks )
  {
    debug() << "Attempting to load TrackToMCP Linker for '" << m_trLoc << "'" << endreq;
    m_trToMCPLinks = new TrackToMCP( evtSvc(), m_trLoc );
    if ( !m_trToMCPLinks->direct() )
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Linker for Tracks to MCParticles not found for '"
                << m_trLoc << "'" << endreq;
      // delete object and set to NULL, to force retrying next time this method is called.
      delete m_trToMCPLinks;
      m_trToMCPLinks = NULL;
    }
    else
    {
      debug() << "Found " << m_trToMCPLinks->direct()->relations().size()
              << " Track to MCParticle associations" << endreq;
    }
  }
  return m_trToMCPLinks;
}

const MCParticle *
RichRecMCTruthTool::mcParticle( const Track * track,
                                const double minWeight ) const
{
  // Try with linkers
  if ( trackToMCPAvailable() )
  {
    TrToMCTable::Range range = trackToMCPLinks()->direct()->relations(track);
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Found " << range.size() << " association(s) for Track " << track->key()
              << " " << track->type() << " " << track->history() << endreq;
    }
    const MCParticle * mcp = NULL;
    double bestWeight = -1;
    for ( TrToMCTable::Range::iterator iMC = range.begin(); iMC != range.end(); ++iMC )
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << " Found " << iMC->to() << " weight=" << iMC->weight() << endreq;
      if ( iMC->weight() > minWeight && iMC->weight() > bestWeight )
      {
        bestWeight = iMC->weight();
        mcp = iMC->to();
      }
    }
    return mcp;
  }
  // If get here MC association failed
  if ( msgLevel(MSG::DEBUG) )
    debug() << "No MC association available for Tracks" << endreq;
  return NULL;
}

const MCParticle *
RichRecMCTruthTool::mcParticle( const RichRecTrack * richTrack,
                                const double minWeight ) const
{
  if ( !richTrack ) return NULL;
  const ContainedObject * obj = richTrack->parentTrack();
  if ( !obj )
  {
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "RichRecTrack " << richTrack->key() 
              <<  " has NULL pointer to parent" << endreq;
    }
    return NULL;
  }

  // What sort of track is this ...

  if ( const Track * offTrack = dynamic_cast<const Track*>(obj) )
  {
    // Track
    debug() << "RichRecTrack " << richTrack->key()
            << " has parent track Track " << offTrack->key() << endreq;
    return mcParticle( offTrack, minWeight );
  }
  else if ( const MCParticle * mcPart = dynamic_cast<const MCParticle*>(obj) )
  {
    // MCParticle
    debug() << "RichRecTrack " << richTrack->key()
            << " has parent track MCParticle " << mcPart->key() << endreq;
    return mcPart;
  }
  else
  {
    Warning( "Unknown RichRecTrack parent track type" );
    return NULL;
  }

}

const SmartRefVector<MCRichHit> &
RichRecMCTruthTool::mcRichHits( const RichRecPixel * richPixel ) const
{
  return m_truth->mcRichHits( richPixel->smartID() );
}

const MCRichDigit *
RichRecMCTruthTool::mcRichDigit( const RichRecPixel * richPixel ) const
{
  // protect against bad pixels
  if ( !richPixel ) return NULL;

  const MCRichDigit * mcDigit = 0;
  if ( Rich::PixelParent::RawBuffer == richPixel->parentType() )
  {

    // use RichSmartID to locate MC information
    mcDigit = m_truth->mcRichDigit( richPixel->smartID() );

  }
  else if ( Rich::PixelParent::Digit == richPixel->parentType() )
  {

    // try to get parent RichDigit
    const RichDigit * digit = dynamic_cast<const RichDigit*>( richPixel->parentPixel() );
    if ( !digit && msgLevel(MSG::DEBUG) ) 
    { 
      debug() << "RichRecPixel " << richPixel->key() 
              << " has no associated RichDigit" << endreq; 
    }

    // All OK, so find and return MCRichDigit for this RichDigit
    mcDigit = m_truth->mcRichDigit( digit->richSmartID() );

  }
  else if ( Rich::PixelParent::NoParent == richPixel->parentType() )
  {

    // Pixel has no parent, so MC association cannot be done
    if ( msgLevel(MSG::DEBUG) ) 
    {
      debug() << "Parentless RichRecPixel -> MC association impossible" << endreq;
    }
    return NULL;

  }
  else
  {      
      Warning( "Do not know how to access MC for RichRecPixel type " +
               Rich::text(richPixel->parentType()) );
  }

  if ( !mcDigit && msgLevel(MSG::DEBUG) ) 
  {
    debug() << "Failed to find MCRichDigit for RichRecPixel "
            << richPixel->key() << endreq;
  }
  return mcDigit;
}

bool
RichRecMCTruthTool::mcParticle( const RichRecPixel * richPixel,
                                std::vector<const MCParticle*> & mcParts ) const
{
  return m_truth->mcParticles( richPixel->smartID(), mcParts );
}

const MCParticle *
RichRecMCTruthTool::trueRecPhoton( const RichRecPhoton * photon ) const
{
  return ( !photon ? NULL :
           trueRecPhoton( photon->richRecSegment(), photon->richRecPixel() ) );
}

const MCParticle * RichRecMCTruthTool::trueRecPhoton( const RichRecSegment * segment,
                                                      const RichRecPixel * pixel ) const
{
  // protect against bad input data
  if ( !segment || !pixel ) { return NULL; }

  const RichRecTrack * track = segment->richRecTrack();
  const MCParticle * mcTrack = ( track ? mcParticle(track) : NULL );
  if ( !mcTrack ) return NULL;

  return trueRecPhoton( mcTrack, pixel->smartID() );
}

const MCParticle * RichRecMCTruthTool::trueRecPhoton( const MCParticle * mcPart,
                                                      const RichSmartID id ) const
{
  // Get MCParticles for the pixel
  std::vector<const MCParticle *> mcParts;
  m_truth->mcParticles(id,mcParts);

  // Loop over all MCParticles associated to the pixel
  for ( std::vector<const MCParticle *>::const_iterator iMCP = mcParts.begin();
        iMCP != mcParts.end(); ++iMCP )
  {
    if ( mcPart == (*iMCP) ) return mcPart;
  }

  return NULL;
}

const MCParticle *
RichRecMCTruthTool::trueCherenkovPhoton( const RichRecPhoton * photon ) const
{
  return ( !photon ? NULL :
           trueCherenkovPhoton( photon->richRecSegment(), photon->richRecPixel() ) );
}

const MCParticle *
RichRecMCTruthTool::trueCherenkovPhoton( const RichRecSegment * segment,
                                         const RichRecPixel * pixel ) const
{
  if ( !segment || !pixel ) return NULL;
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Testing RichRecSegment " << segment->key()
            << " and RichRecPixel " << pixel->key() << endreq;
  }

  const MCParticle * mcPart = trueRecPhoton( segment, pixel );
  return ( !mcPart ? NULL :
           trueCherenkovRadiation( pixel, segment->trackSegment().radiator() ) );
}

const MCParticle *
RichRecMCTruthTool::trueCherenkovPhoton( const LHCb::MCParticle * mcPart,
                                         const LHCb::RichSmartID id,
                                         const Rich::RadiatorType rad ) const
{
  const MCParticle * mcPart2 = trueRecPhoton( mcPart, id );
  return ( !mcPart2 ? NULL :
           trueCherenkovRadiation( id, rad ) );
}

const MCParticle *
RichRecMCTruthTool::trueCherenkovRadiation( const RichRecPixel * pixel,
                                            const Rich::RadiatorType rad ) const
{
  return trueCherenkovRadiation( pixel->smartID(), rad );
}

const MCParticle *
RichRecMCTruthTool::trueCherenkovRadiation( const RichSmartID id,
                                            const Rich::RadiatorType rad ) const
{
  // Test if hit is background
  if ( m_truth->isBackground(id) ) return NULL;

  // Test if hit is from correct radiator
  if ( !m_truth->isCherenkovRadiation(id,rad) ) return NULL;

  // All OK so find correct MCParticle
  std::vector<const MCParticle *> mcParts;
  m_truth->mcParticles(id,mcParts);

  // Should do something better than just return first, but what ??
  return ( mcParts.empty() ? NULL : mcParts.front() );
}

Rich::ParticleIDType
RichRecMCTruthTool::mcParticleType( const LHCb::Track * track,
                                    const double minWeight ) const
{
  return m_truth->mcParticleType( mcParticle(track,minWeight) );
}

Rich::ParticleIDType
RichRecMCTruthTool::mcParticleType( const RichRecTrack * richTrack,
                                    const double minWeight ) const
{
  return m_truth->mcParticleType( mcParticle(richTrack,minWeight) );
}

Rich::ParticleIDType
RichRecMCTruthTool::mcParticleType( const RichRecSegment * richSegment,
                                    const double minWeight ) const
{
  return ( richSegment ? mcParticleType(richSegment->richRecTrack(),minWeight) : Rich::Unknown );
}

const MCParticle *
RichRecMCTruthTool::mcParticle( const RichRecSegment * richSegment,
                                const double minWeight ) const
{
  return ( richSegment ? mcParticle( richSegment->richRecTrack(), minWeight ) : NULL );
}

// CRJ : Update to remove use of MCRichHits ?
bool
RichRecMCTruthTool::mcRichOpticalPhoton( const RichRecPixel * richPixel,
                                         SmartRefVector<MCRichOpticalPhoton> & phots ) const
{
  phots.clear();
  const SmartRefVector<MCRichHit> & hits = mcRichHits(richPixel);
  for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
        iHit != hits.end(); ++iHit )
  {
    // protect against bad hits
    if ( !(*iHit) ) continue;

    // Find MC photon
    const MCRichOpticalPhoton * phot = m_truth->mcOpticalPhoton( *iHit );
    if ( phot ) phots.push_back( phot );
  }

  // Return boolean indicating if any photons were found
  return !phots.empty();
}

const MCRichSegment *
RichRecMCTruthTool::mcRichSegment( const RichRecSegment * segment ) const
{
  if ( !segment ) return NULL;
  const MCRichTrack * mcTrack = mcRichTrack( segment );
  return ( mcTrack ?
           mcTrack->segmentInRad(segment->trackSegment().radiator()) : 0 );
}

const SmartRefVector<MCRichSegment> *
RichRecMCTruthTool::mcRichSegments( const RichRecTrack * track ) const
{
  const MCRichTrack * mcTrack = mcRichTrack( track );
  return ( mcTrack ? &(mcTrack->mcSegments()) : 0 );
}

const MCRichTrack *
RichRecMCTruthTool::mcRichTrack( const RichRecSegment * segment ) const
{
  return ( segment ? mcRichTrack( segment->richRecTrack() ) : 0 );
}

const MCRichTrack *
RichRecMCTruthTool::mcRichTrack( const RichRecTrack * track ) const
{
  return m_truth->mcRichTrack( mcParticle(track) );
}

bool RichRecMCTruthTool::isBackground( const RichRecPixel * pixel ) const
{
  return m_truth->isBackground( pixel->smartID() );
}

const MCRichHit *
RichRecMCTruthTool::trueCherenkovHit( const RichRecPhoton * photon ) const
{
  // Track MCParticle
  const MCParticle * trackMCP = mcParticle( photon->richRecSegment() );
  if ( trackMCP )
  {
    // Loop over all MCRichHits for the pixel associated to this photon
    const SmartRefVector<MCRichHit> & hits = mcRichHits( photon->richRecPixel() );
    for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
          iHit != hits.end(); ++iHit )
    {
      if ( !(*iHit) ) continue;
      const MCParticle * pixelMCP = (*iHit)->mcParticle();
      if ( pixelMCP == trackMCP ) return *iHit;
    }

  }

  // Not a true combination...
  return NULL;
}

const MCRichOpticalPhoton *
RichRecMCTruthTool::trueOpticalPhoton( const RichRecPhoton * photon ) const
{
  return ( !photon ? NULL :
           trueOpticalPhoton( photon->richRecSegment(), photon->richRecPixel() ) );
}

const MCRichOpticalPhoton *
RichRecMCTruthTool::trueOpticalPhoton( const RichRecSegment * segment,
                                       const RichRecPixel * pixel ) const
{
  // Is this a true cherenkov combination
  const MCParticle * mcPart = trueCherenkovPhoton(segment,pixel);
  if ( mcPart )
  {
    // Now find associated MCRichOpticalPhoton
    const SmartRefVector<MCRichHit> & hits = m_truth->mcRichHits(mcPart);
    for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
          iHit != hits.end(); ++iHit )
    {
      if ( *iHit )
      {
        // check pixel ID
        const RichSmartID hitid ( pixel->smartID().pixelSubRowDataIsValid() ?
                                  (*iHit)->sensDetID() :
                                  (*iHit)->sensDetID().pixelID() );
        if ( pixel->smartID() == hitid )
        {
          // check radiator type
          if ( segment->trackSegment().radiator() == (*iHit)->radiator() )
          {
            return m_truth->mcOpticalPhoton(*iHit);
          }
        }
      }
    }

  }

  // Not a true combination...
  return NULL;
}

bool RichRecMCTruthTool::trackToMCPAvailable() const
{
  return ( trackToMCPLinks() != NULL && trackToMCPLinks()->direct() );
}

bool RichRecMCTruthTool::pixelMCHistoryAvailable() const
{
  return m_truth->richMCHistoryAvailable();
}

bool RichRecMCTruthTool::photonMCAssocAvailable() const
{
  return ( trackToMCPAvailable() && pixelMCHistoryAvailable() );
}

bool RichRecMCTruthTool::extendedMCAvailable() const
{
  return m_truth->extendedMCAvailable();
}
