
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromSignalRichDigits.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPixelCreatorFromSignalRichDigits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromSignalRichDigits.cpp,v 1.10 2006-01-23 14:09:59 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */
//-----------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromSignalRichDigits.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromSignalRichDigits>          s_factory ;
const        IToolFactory& RichPixelCreatorFromSignalRichDigitsFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromSignalRichDigits::
RichPixelCreatorFromSignalRichDigits( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichPixelCreatorBase  ( type, name, parent ),
    m_mcTool              ( 0 ),
    m_pixMaker            ( 0 ),
    m_subPixelCreatorName ( "RichPixelCreatorReco" ),
    m_trackFilter         ( false ),
    m_trackMCPsDone       ( false )
{

  // Define job option parameters
  declareProperty( "RecoDigitsLocation",
                   m_recoDigitsLocation = RichDigitLocation::Default );
  declareProperty( "DelegatedPixelCreator", m_subPixelCreatorName );
  declareProperty( "FilterTracklessDigits", m_trackFilter );

}

StatusCode RichPixelCreatorFromSignalRichDigits::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPixelCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichMCTruthTool",     m_mcTool    );
  acquireTool( "RichRecMCTruthTool",  m_mcRecTool );
  acquireTool( m_subPixelCreatorName, m_pixMaker  );

  return sc;
}

StatusCode RichPixelCreatorFromSignalRichDigits::finalize()
{
  // Execute base class method
  return RichPixelCreatorBase::finalize();
}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel *
RichPixelCreatorFromSignalRichDigits::newPixel( const ContainedObject * obj ) const
{
  // Try to cast to RichDigit
  const RichDigit * digit = dynamic_cast<const RichDigit*>(obj);
  if ( !digit )
  {
    Warning("Parent not of type RichDigit");
    return NULL;
  }

  // Get MCRichDigit
  const MCRichDigit * mcDigit = m_mcTool->mcRichDigit(digit->richSmartID());
  if ( !mcDigit ) return NULL;

  // Test if this is a background hit
  if ( m_mcTool->isBackground( mcDigit ) ) return NULL;

  // if requested, filter trackless hits
  if ( m_trackFilter ) {

    // MC parentage for pixel
    const SmartRefVector<MCRichHit> & hits = mcDigit->hits();

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
  return m_pixMaker->newPixel( obj );
}

RichPixelCreatorFromSignalRichDigits::TrackedMCPList &
RichPixelCreatorFromSignalRichDigits::trackedMCPs() const
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

StatusCode RichPixelCreatorFromSignalRichDigits::newPixels() const
{
  if ( m_allDone ) return StatusCode::SUCCESS;
  m_allDone = true;

  // Obtain smart data pointer to RichDigits
  const RichDigits * digits = get<RichDigits>( m_recoDigitsLocation );

  // reserve space
  richPixels()->reserve( digits->size() );

  // Loop over RichDigits and create working pixels
  for ( RichDigits::const_iterator digit = digits->begin();
        digit != digits->end(); ++digit )
  {
    newPixel( *digit );
  }

  // Make sure pixels are sorted
  // (Maybe not needed, but speed not so important anyway when using MC)
  sortPixels();

  // find iterators
  fillIterators();

  // Debug messages
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Located " << digits->size() << " RichDigits at "
            << m_recoDigitsLocation << endreq
            << "Created " << richPixels()->size() << " RichRecPixels at "
            << pixelLocation() << endreq;
  }

  return StatusCode::SUCCESS;
}

void RichPixelCreatorFromSignalRichDigits::InitNewEvent()
{
  // Initialise data for new event
  RichPixelCreatorBase::InitNewEvent();
  m_trackedMCPs.clear();
  m_trackMCPsDone = false;
}
