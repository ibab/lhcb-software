
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromSignalRichDigits.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPixelCreatorFromSignalRichDigits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromSignalRichDigits.cpp,v 1.7 2005-02-02 10:01:48 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */
//-----------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromSignalRichDigits.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromSignalRichDigits>          s_factory ;
const        IToolFactory& RichPixelCreatorFromSignalRichDigitsFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromSignalRichDigits::
RichPixelCreatorFromSignalRichDigits( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichRecToolBase       ( type, name, parent ),
    m_pixels              ( 0 ),
    m_mcTool              ( 0 ),
    m_pixMaker            ( 0 ),
    m_subPixelCreatorName ( "RichPixelCreatorReco" ),
    m_allDone             ( false ),
    m_trackFilter         ( false ),
    m_trackMCPsDone       ( false )
{

  declareInterface<IRichPixelCreator>(this);

  // Define job option parameters
  declareProperty( "RichRecPixelLocation",
                   m_richRecPixelLocation = RichRecPixelLocation::Default );
  declareProperty( "RecoDigitsLocation",
                   m_recoDigitsLocation = RichDigitLocation::Default );
  declareProperty( "DelegatedPixelCreator", m_subPixelCreatorName );
  declareProperty( "FilterTracklessDigits", m_trackFilter );

}

StatusCode RichPixelCreatorFromSignalRichDigits::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichMCTruthTool",     m_mcTool    );
  acquireTool( "RichRecMCTruthTool",  m_mcRecTool );
  acquireTool( m_subPixelCreatorName, m_pixMaker  );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  if (msgLevel(MSG::DEBUG)) incSvc()->addListener( this, IncidentType::EndEvent );

  // Make sure we are ready for a new event
  InitNewEvent();

  return sc;
}

StatusCode RichPixelCreatorFromSignalRichDigits::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPixelCreatorFromSignalRichDigits::handle ( const Incident& incident )
{
  // Update prior to start of event. Used to re-initialise data containers
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
  // Debug printout at the end of each event
  else if ( msgLevel(MSG::DEBUG) && IncidentType::EndEvent == incident.type() )
    {
      debug() << "Created " << richPixels()->size() << " RichRecPixels at "
              << m_richRecPixelLocation << endreq;
    }
}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel *
RichPixelCreatorFromSignalRichDigits::newPixel( const ContainedObject * obj ) const
{
  // Try to cast to RichDigit
  const RichDigit * digit = dynamic_cast<const RichDigit*>(obj);
  if ( !digit ) {
    Warning("Parent not of type RichDigit");
    return NULL;
  }

  // Get MCRichDigit
  const MCRichDigit * mcDigit = m_mcTool->mcRichDigit(digit);
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
          iHit != hits.end(); ++iHit ) {
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
  if ( !m_trackMCPsDone ) {
    m_trackMCPsDone = true;

    // Make sure all RichRecTracks have been formed
    if ( !trackCreator()->newTracks() ) Exception("Error whilst creating RichRecTracks");
    if ( richTracks()->empty() )        Warning( "RichRecTrack container empty !");

    // Loop over reconstructed tracks to form a list of tracked MCParticles
    debug() << "Found " <<  richTracks()->size() << " RichRecTracks" << endreq;
    for ( RichRecTracks::const_iterator iTk = richTracks()->begin();
          iTk != richTracks()->end(); ++iTk ) {
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

  // Loop over RichDigits and create working pixels
  richPixels()->reserve( digits->size() );
  for ( RichDigits::const_iterator digit = digits->begin();
        digit != digits->end(); ++digit ) { newPixel( *digit ); }

  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Located " << digits->size() << " RichDigits at "
            << m_recoDigitsLocation << endreq;
  }

  return StatusCode::SUCCESS;
}

RichRecPixels * RichPixelCreatorFromSignalRichDigits::richPixels() const
{
  if ( !m_pixels ) {
    SmartDataPtr<RichRecPixels> tdsPixels( evtSvc(),
                                           m_richRecPixelLocation );
    if ( !tdsPixels ) {

      // Reinitialise the Pixel Container
      m_pixels = new RichRecPixels();

      // Register new RichRecPhoton container to Gaudi data store
      put( m_pixels, m_richRecPixelLocation );

    } else {

      debug() << "Found " << tdsPixels->size() << " pre-existing RichRecPixels in TES at "
              << m_richRecPixelLocation << endreq;

      // Set smartref to TES pixel container
      m_pixels = tdsPixels;

    }
  }
  return m_pixels;
}
