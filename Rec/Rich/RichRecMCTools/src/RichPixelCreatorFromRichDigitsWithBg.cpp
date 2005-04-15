
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromRichDigitsWithBg.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPixelCreatorFromRichDigitsWithBg
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRichDigitsWithBg.cpp,v 1.9 2005-04-15 16:32:30 jonrob Exp $
 *
 *  @author Andy Buckley  buckley@hep.phy.cam.ac.uk
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/10/2004
 */
//-----------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromRichDigitsWithBg.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromRichDigitsWithBg>          s_factory ;
const        IToolFactory& RichPixelCreatorFromRichDigitsWithBgFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromRichDigitsWithBg::
RichPixelCreatorFromRichDigitsWithBg( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_pixels      ( 0 ),
    m_smartIDTool ( 0 ),
    m_allDone     ( false ),
    m_numBgTracksToAdd ( Rich::NRiches, 0 ),
    m_usedDets    ( Rich::NRiches, true )
{

  declareInterface<IRichPixelCreator>(this);

  // Define job option parameters
  declareProperty( "RichRecPixelLocation", m_richRecPixelLocation = RichRecPixelLocation::Default );
  declareProperty( "RecoDigitsLocation", m_recoDigitsLocation = RichDigitLocation::Default );
  declareProperty( "NumberBackgroundTracksToAdd",  m_numBgTracksToAdd );
  declareProperty( "UseDetectors", m_usedDets );
}


// Initializer
StatusCode RichPixelCreatorFromRichDigitsWithBg::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool", m_smartIDTool );
  acquireTool( "RichMCTruthTool", m_mcTool      );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  if (msgLevel(MSG::DEBUG)) incSvc()->addListener( this, IncidentType::EndEvent );

  // warn that this background adding creator is being used
  warning() << "Using background adding pixel creator : Will add " << m_numBgTracksToAdd[Rich::Rich1]
            << "/" << m_numBgTracksToAdd[Rich::Rich2] << " traversing particles to RICH(1/2)" << endreq;

  return sc;
}

StatusCode RichPixelCreatorFromRichDigitsWithBg::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPixelCreatorFromRichDigitsWithBg::handle ( const Incident& incident )
{
  // Update prior to start of event. Used to re-initialise data containers
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
  // Debug printout at the end of each event
  else if ( msgLevel(MSG::DEBUG) && IncidentType::EndEvent == incident.type() )
  {
    debug() << "Event finished with " << richPixels()->size() << " RichRecPixels" << endreq;
  }
}


// Fill the stack of background tracks from the current event
// (effectively emptying this event)
StatusCode
RichPixelCreatorFromRichDigitsWithBg::fillBgTrackStack() const
{
  // Obtain smart data pointer to RichDigits
  RichDigits * digits = get<RichDigits>( m_recoDigitsLocation );
  RichMap<Rich::DetectorType, unsigned int> originalNumTracksInStack;
  originalNumTracksInStack[Rich::Rich1] = m_digitsForTrackBg[Rich::Rich1].size();
  originalNumTracksInStack[Rich::Rich2] = m_digitsForTrackBg[Rich::Rich2].size();
  RichMap<Rich::DetectorType, unsigned int> numDigitsAddedToStack;

  // Loop over RichDigits and create working pixels
  for ( RichDigits::const_iterator digit = digits->begin(); digit != digits->end(); ++digit ) {
    const Rich::DetectorType whichRich( (*digit)->key().rich() );

    // Make association to the MCRichDigit via IRichMCTruthTool tool:
    const MCRichDigit* mcDigit( m_mcTool->mcRichDigit(*digit) );
    if (mcDigit) {

      // Get MC hits from the MC digit (can be >1 photon producing one digit in general)
      if ( !mcDigit->hits().empty() ) {

        // For each hit, retrieve the MCParticle and add it to the stack if valid
        for (SmartRefVector<MCRichHit>::const_iterator mcHit = mcDigit->hits().begin();
             mcHit != mcDigit->hits().end(); ++mcHit) {
          const MCParticle* mcParticle( (*mcHit)->mcParticle() );
          if (mcParticle) {
            if ( m_numBgTracksToAdd[whichRich] > 0 ) {
              m_digitsForTrackBg[whichRich][mcParticle].push_back( (*digit)->key() );
              ++numDigitsAddedToStack[whichRich];
            }
          }
        }

      }
    }
  }

  // Empty the current event's digit collection
  digits->clear();

  // Work out how many tracks were added to the stack and tell the world about it
  if ( msgLevel(MSG::INFO) )
  {
    RichMap<Rich::DetectorType, unsigned int> numTracksAddedToStack;
    numTracksAddedToStack[Rich::Rich1] =
      m_digitsForTrackBg[Rich::Rich1].size() - originalNumTracksInStack[Rich::Rich1];
    numTracksAddedToStack[Rich::Rich2] =
      m_digitsForTrackBg[Rich::Rich2].size() - originalNumTracksInStack[Rich::Rich2];
    if (m_numBgTracksToAdd[Rich::Rich1] > 0) {
      info() << "Added "
             << numTracksAddedToStack[Rich::Rich1]  << " particles ("
             << numDigitsAddedToStack[Rich::Rich1] << " digits) to the RICH1 stack ("
             << originalNumTracksInStack[Rich::Rich1] << "->" << m_digitsForTrackBg[Rich::Rich1].size() << ")"
             << endreq;
    }
    if (m_numBgTracksToAdd[Rich::Rich2] > 0) {
      info() << "Added "
             << numTracksAddedToStack[Rich::Rich2]  << " particles ("
             << numDigitsAddedToStack[Rich::Rich2] << " digits) to the RICH2 stack ("
             << originalNumTracksInStack[Rich::Rich2] << "->" << m_digitsForTrackBg[Rich::Rich2].size() << ")"
             << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel *
RichPixelCreatorFromRichDigitsWithBg::newPixel( const ContainedObject * obj ) const
{
  // Try to cast to RichDigit
  const RichDigit * digit = dynamic_cast<const RichDigit*>(obj);
  if ( !digit ) {
    Warning("Parent not of type RichDigit");
    return NULL;
  }

  // Get the pixel via the smart ID
  RichRecPixel * newPixel( newPixel( digit->key() ) );

  // Set parent information
  if ( newPixel )
  {
    newPixel->setParentPixel( digit );
    newPixel->setParentType( Rich::PixelParent::Digit );
  }

  return newPixel;
}

RichRecPixel *
RichPixelCreatorFromRichDigitsWithBg::newPixel( const RichSmartID id ) const
{

  // See if this RichRecPixel already exists
  if ( m_pixelDone[id] ) {
    return m_pixelExists[id];
  } else {
    RichRecPixel* newPixel(0);

    // check smartid is valid
    if ( id.pixelDataAreValid() ) {

      // Check if we are using this detector
      if ( m_usedDets[id.rich()] ) {

        // Make a new RichRecPixel
        newPixel = new RichRecPixel();
        richPixels()->insert( newPixel );

        // Positions
        newPixel->globalPosition() = m_smartIDTool->globalPosition( id );
        newPixel->localPosition()  = m_smartIDTool->globalToPDPanel(newPixel->globalPosition());

        // Set smartID
        newPixel->setSmartID( id );

      }

    } else {
      Warning("RichSmartID does not contain valid pixel data!");
    }

    // Add to reference map
    m_pixelExists[ id ] = newPixel;
    m_pixelDone  [ id ] = true;

    return newPixel;
  }
}


StatusCode RichPixelCreatorFromRichDigitsWithBg::newPixels() const
{
  if ( m_allDone ) return StatusCode::SUCCESS;
  m_allDone = true;

  // Do we need to fill the background stacks ?
  if ( m_numBgTracksToAdd[Rich::Rich1] > m_digitsForTrackBg[Rich::Rich1].size() ||
       m_numBgTracksToAdd[Rich::Rich2] > m_digitsForTrackBg[Rich::Rich2].size() )
  {
    debug() << "Not enough background tracks in the stack ( R1="
            << m_digitsForTrackBg[Rich::Rich1].size() + m_digitsForTrackBg[Rich::Rich2].size()
            << " R2=" << m_numBgTracksToAdd[Rich::Rich1] + m_numBgTracksToAdd[Rich::Rich2] << " ). "
            << "Refilling from this event" << endreq;

    fillBgTrackStack();

    return StatusCode::SUCCESS; // no digits to run on
  }

  // Obtain smart data pointer to RichDigits
  const RichDigits * digits = get<RichDigits>( m_recoDigitsLocation );

  // Loop over RichDigits and create working pixels
  richPixels()->reserve( digits->size() );
  for ( RichDigits::const_iterator digit = digits->begin();
        digit != digits->end(); ++digit ) { newPixel( *digit ); }

  // Do the same thing for the bg track digits
  RichMap<Rich::DetectorType, size_t> numBgTracksAdded;
  size_t numBgPixelsAdded(0);
  // RICH1
  while ( numBgTracksAdded[Rich::Rich1] < m_numBgTracksToAdd[Rich::Rich1] )
  {

    // Add hits for this particle
    std::vector<RichSmartID> & bgdigits = m_digitsForTrackBg[Rich::Rich1].begin()->second;
    for ( std::vector<RichSmartID>::const_iterator digit = bgdigits.begin();
          digit != bgdigits.end(); ++digit )
    {
      newPixel( *digit );
      ++numBgPixelsAdded;
    }

    // count number of added particles
    ++numBgTracksAdded[Rich::Rich1];

    // Remove this particle's digits from the stack
    m_digitsForTrackBg[Rich::Rich1].erase(m_digitsForTrackBg[Rich::Rich1].begin());
  }
  // RICH2
  while ( numBgTracksAdded[Rich::Rich2] < m_numBgTracksToAdd[Rich::Rich2] )
  {

    // Add hits for this particle
    std::vector<RichSmartID> & bgdigits = m_digitsForTrackBg[Rich::Rich2].begin()->second;
    for ( std::vector<RichSmartID>::const_iterator digit = bgdigits.begin();
          digit != bgdigits.end(); ++digit )
    {
      newPixel( *digit );
      ++numBgPixelsAdded;
    }

    // count number of added particles
    ++numBgTracksAdded[Rich::Rich2];

    // Remove this particle's digits from the stack
    m_digitsForTrackBg[Rich::Rich2].erase(m_digitsForTrackBg[Rich::Rich2].begin());

  }

  // Report
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Located " << digits->size() << " RichDigits at "
            << m_recoDigitsLocation << endreq
            << "Added " << numBgTracksAdded << " additional background tracks with "
            << numBgPixelsAdded << " pixels" << endreq
            << "Created " << richPixels()->size() << " RichRecPixels at "
            << m_richRecPixelLocation << endreq;
  }

  return StatusCode::SUCCESS;
}



RichRecPixels * RichPixelCreatorFromRichDigitsWithBg::richPixels() const
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

      // Remake local pixel reference map
      for ( RichRecPixels::const_iterator iPixel = tdsPixels->begin();
            iPixel != tdsPixels->end(); ++iPixel ) {
        m_pixelExists [ (*iPixel)->smartID() ] = *iPixel;
        m_pixelDone   [ (*iPixel)->smartID() ] = true;
      }

    }
  }

  return m_pixels;
}
