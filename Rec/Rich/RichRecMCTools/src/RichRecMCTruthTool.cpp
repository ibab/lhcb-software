// $Id: RichRecMCTruthTool.cpp,v 1.7 2004-06-29 19:45:38 jonesc Exp $

// local
#include "RichRecMCTruthTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecMCTruthTool
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRecMCTruthTool>          s_factory ;
const        IToolFactory& RichRecMCTruthToolFactory = s_factory ;

// Standard constructor
RichRecMCTruthTool::RichRecMCTruthTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_truth         ( 0 )
{
  declareInterface<IRichRecMCTruthTool>(this);
}


StatusCode RichRecMCTruthTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Acquire instances of tools
  acquireTool( "RichMCTruthTool", m_truth );

  return StatusCode::SUCCESS;
}

StatusCode RichRecMCTruthTool::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

const MCParticle *
RichRecMCTruthTool::mcParticle( const RichRecTrack * richTrack ) const
{
  const ContainedObject * obj = richTrack->parentTrack();
  if ( !obj ) return NULL;

  // Try TrStoredTrack
  const TrStoredTrack * track = dynamic_cast<const TrStoredTrack*>(obj);
  if ( track ) return m_truth->mcParticle( track );

  // else Try MCParticle
  const MCParticle * mcPart = dynamic_cast<const MCParticle*>(obj);
  if ( mcPart ) return mcPart;

  return NULL;
}

const SmartRefVector<MCRichHit> &
RichRecMCTruthTool::mcRichHits( const RichRecPixel * richPixel ) const
{
  const MCRichDigit * mcDigit = mcRichDigit(richPixel);
  return ( mcDigit ? mcDigit->hits() : m_emptyContainer );
}

const MCRichDigit *
RichRecMCTruthTool::mcRichDigit( const RichRecPixel * richPixel ) const
{
  const RichDigit * digit =
    dynamic_cast<const RichDigit*>( richPixel->parentPixel() );
  if ( !digit ) {
    Warning("RichRecPixel has no associated RichDigit");
    return NULL;
  }

  return m_truth->mcRichDigit( digit );
}

bool RichRecMCTruthTool::mcParticle( const RichRecPixel * richPixel,
                                     std::vector<const MCParticle*> & mcParts ) const
{

  // Clean vector
  mcParts.clear();

  // Loop over all MCRichHits associated to the pixel
  const SmartRefVector<MCRichHit> & hits = mcRichHits( richPixel );
  if ( hits.empty() ) return false;
  for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
        iHit != hits.end(); ++iHit ) {
    if ( !(*iHit) ) continue; // protect against bad hits
    const MCParticle * mcPart = (*iHit)->mcParticle();
    if ( !mcPart ) continue;
    std::vector<const MCParticle*>::const_iterator iFind =
      std::find( mcParts.begin(), mcParts.end(), mcPart );
    if ( mcParts.end() == iFind ) mcParts.push_back( mcPart );
  }

  return true;
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
  const RichRecTrack * track = segment->richRecTrack();
  const MCParticle * mcTrack = ( track ? mcParticle(track) : NULL );
  if ( !mcTrack ) return NULL;

  // Loop over all MCRichHits associated to the pixel
  const SmartRefVector<MCRichHit> & hits = mcRichHits( pixel );
  for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
        iHit != hits.end(); ++iHit ) {
    if ( (*iHit) && ( mcTrack == (*iHit)->mcParticle() ) ) return mcTrack;
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
  const MCParticle * mcPart = trueRecPhoton( segment, pixel );
  return ( !mcPart ? NULL :
           trueCherenkovRadiation( pixel, segment->trackSegment().radiator() ) );
}

const MCParticle *
RichRecMCTruthTool::trueCherenkovRadiation( const RichRecPixel * pixel,
                                            const Rich::RadiatorType rad ) const
{
  // Loop over all MCRichHits for this pixel
  const SmartRefVector<MCRichHit> & hits = mcRichHits( pixel );
  for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
        iHit != hits.end(); ++iHit ) {
    if ( *iHit && 
         rad == (*iHit)->radiator() && 
         !m_truth->isBackground(*iHit) ) return (*iHit)->mcParticle();
  }

  return NULL;
}

Rich::ParticleIDType
RichRecMCTruthTool::mcParticleType( const RichRecTrack * richTrack ) const
{
  return m_truth->mcParticleType( mcParticle(richTrack) );
}

Rich::ParticleIDType
RichRecMCTruthTool::mcParticleType( const RichRecSegment * richSegment ) const
{
  return ( richSegment ? mcParticleType(richSegment->richRecTrack()) : Rich::Unknown );
}

const MCParticle *
RichRecMCTruthTool::mcParticle( const RichRecSegment * richSegment ) const
{
  return ( richSegment ? mcParticle( richSegment->richRecTrack() ) : NULL );
}

bool
RichRecMCTruthTool::mcRichOpticalPhoton( const RichRecPixel * richPixel,
                                         SmartRefVector<MCRichOpticalPhoton> & phots ) const
{
  phots.clear();
  const SmartRefVector<MCRichHit> & hits = mcRichHits(richPixel);
  for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
        iHit != hits.end(); ++iHit ) {
    if ( !(*iHit) ) continue; // protect against bad hits
    const MCRichOpticalPhoton * phot = m_truth->mcOpticalPhoton( *iHit );
    if ( phot ) phots.push_back( phot );
  }
  if ( phots.empty() ) return false;
  return true;
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
  return m_truth->isBackground( mcRichDigit(pixel) );
}

const MCRichHit *
RichRecMCTruthTool::trueCherenkovHit( const RichRecPhoton * photon ) const
{
  // Track MCParticle
  const MCParticle * trackMCP = mcParticle( photon->richRecSegment() );
  if ( trackMCP ) {

    // Loop over all MCRichHits for the pixel associated to this photon
    const SmartRefVector<MCRichHit> & hits = mcRichHits( photon->richRecPixel() );
    for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
          iHit != hits.end(); ++iHit ) {
      if ( !(*iHit) ) continue;
      const MCParticle * pixelMCP = (*iHit)->mcParticle();
      if ( pixelMCP == trackMCP ) return *iHit;
    }

  }

  return NULL;
}

const MCRichOpticalPhoton *
RichRecMCTruthTool::trueOpticalPhoton( const RichRecPhoton * photon ) const
{
  // get true MCRichHit
  const MCRichHit * mchit = trueCherenkovHit( photon );
  if ( !mchit ) return NULL;
  // return associated MCRichOpticalPhoton
  return m_truth->mcOpticalPhoton( mchit );
}

const MCRichOpticalPhoton *
RichRecMCTruthTool::trueOpticalPhoton( const RichRecSegment * segment,
                                       const RichRecPixel * pixel ) const
{
  // Is this a true cherenkov combination
  const MCParticle * mcPart = trueCherenkovPhoton(segment,pixel);
  if ( mcPart ) {

    // Now find associated MCRichOpticalPhoton
    const SmartRefVector<MCRichHit> & hits = mcRichHits(pixel);
    for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
          iHit != hits.end(); ++iHit ) {
      if ( *iHit && 
           (*iHit)->mcParticle() == mcPart ) return m_truth->mcOpticalPhoton(*iHit);
    }

  }

  // Not a true combination...
  return NULL;
}

