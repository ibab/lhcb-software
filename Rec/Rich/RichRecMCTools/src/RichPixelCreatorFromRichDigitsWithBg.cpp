
/** @file RichPixelCreatorFromRichDigitsWithBg.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPixelCreatorFromRichDigitsWithBg
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRichDigitsWithBg.cpp,v 1.1 2004-10-13 09:37:27 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Andy Buckley  buckley@hep.phy.cam.ac.uk
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/10/2004
 */

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
    m_usedRads    ( Rich::NRadiatorTypes, true )
{

  declareInterface<IRichPixelCreator>(this);

  // Define job option parameters
  declareProperty( "RichRecPixelLocation", m_richRecPixelLocation = RichRecPixelLocation::Default );
  declareProperty( "RecoDigitsLocation", m_recoDigitsLocation = RichDigitLocation::Default );
  declareProperty( "NumberBackgroundTracksToAdd",  m_noBgTracksToAdd = 10 );
  declareProperty( "UseRadiators", m_usedRads );

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

  // Make sure we are ready for a new event
  InitNewEvent();

  return StatusCode::SUCCESS;
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
    debug() << "Created " << richPixels()->size() << " RichRecPixels" << endreq;
  }
}


// Fill the stack of background tracks from the current event
// (effectively emptying this event)
StatusCode
RichPixelCreatorFromRichDigitsWithBg::fillBgTrackStack() const
{
  // Obtain smart data pointer to RichDigits
  RichDigits * digits = get<RichDigits>( m_recoDigitsLocation );
  const unsigned int originalNoTracksInStack(m_digitsForTrackBg.size());
  unsigned int noDigitsAddedToStack(0);

  // Loop over RichDigits and create working pixels
  for ( RichDigits::const_iterator digit = digits->begin(); digit != digits->end(); ++digit ) {

    // Make association to the MCRichDigit via IRichMCTruthTool tool:
    const MCRichDigit* mcDigit( m_mcTool->mcRichDigit(*digit) );
    if (mcDigit) {

      // Get MC hits from the MC digit (can be >1 photon producing one digit in general)
      const SmartRefVector<MCRichHit> mcHits( mcDigit->hits() );
      if ( !mcHits.empty() ) {

        // For each hit, retrieve the MCParticle and add it to the stack if valid
        for (SmartRefVector<MCRichHit>::const_iterator mcHit = mcHits.begin();
             mcHit != mcHits.end(); ++mcHit) {
          const MCParticle* mcParticle( (*mcHit)->mcParticle() );
          if (mcParticle) {
            //std::vector<RichDigit>& bgDigits = m_digitsForTrackBg[mcParticle];
            m_digitsForTrackBg[mcParticle].push_back( (*digit)->key() );
            ++noDigitsAddedToStack;
          }
        }

      }
    }
  }

  // Empty the current event's digit collection
  // (could also operate without emptying, provided the stack is only filled at the end?
  // (but then what if the event isn't big enough?!?))
  digits->clear();

  // Work out how many tracks were added to the stack and tell the (debug) world about it
  const unsigned int noTracksAddedToStack(m_digitsForTrackBg.size() - originalNoTracksInStack);
  debug() << "Added "
          << noTracksAddedToStack  << " tracks ("
          << noDigitsAddedToStack << " digits) to the background track stack ("
          << originalNoTracksInStack << "->" << m_digitsForTrackBg.size() << ")"
          << endreq;

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
  RichRecPixel* newPixel( newPixel( digit->key() ) );

  // Set parent information
  if ( newPixel ) {
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

    RichRecPixel * newPixel = NULL;

    // Check if we are using this radiator
    if ( m_usedRads[hit->radiator()] ) {

      if ( id.pixelDataAreValid() ) {

        // Make a new RichRecPixel
        newPixel = new RichRecPixel();
        richPixels()->insert( newPixel );

        // Positions
        HepPoint3D & gPosition = newPixel->globalPosition();
        m_smartIDTool->globalPosition( id, gPosition );
        newPixel->localPosition() = m_smartIDTool->globalToPDPanel(gPosition);

        // Set smartID
        newPixel->setSmartID( id );

      } else {
        Warning("RichSmartID does not contain valid pixel data !");
      }

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

  if ( m_noBgTracksToAdd > m_digitsForTrackBg.size() ) {
    debug() << "Not enough background tracks in the stack ("
            << m_digitsForTrackBg.size() << "/" << m_noBgTracksToAdd << "). "
            << "'Refilling' from this event" << endreq;
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
  size_t noBgTracksAdded(0);
  if ( m_noBgTracksToAdd <= m_digitsForTrackBg.size() > 0 ) {
    while ( noBgTracksAdded < m_noBgTracksToAdd ) {

      std::vector<RichSmartID> & bgdigits = m_digitsForTrackBg.begin()->second;

      // Add the new digits: do I have to catch a double-add exception?
      for ( std::vector<RichSmartID>::const_iterator digit = bgdigits.begin();
            digit != bgdigits.end(); ++digit ) {
        newPixel( *digit );
      }
      ++noBgTracksAdded;

      // Remove this particle's digits here, just to be safe
      m_digitsForTrackBg.erase(m_digitsForTrackBg.begin());
    }
  }

  // Report
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Located " << digits->size() << " RichDigits at "
            << m_recoDigitsLocation << endreq
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
