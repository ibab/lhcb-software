
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromCheatedRichDigits.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPixelCreatorFromCheatedRichDigits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromCheatedRichDigits.cpp,v 1.13 2005-02-02 10:01:48 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */
//-----------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromCheatedRichDigits.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromCheatedRichDigits>          s_factory ;
const        IToolFactory& RichPixelCreatorFromCheatedRichDigitsFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromCheatedRichDigits::
RichPixelCreatorFromCheatedRichDigits( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_pixels      ( 0 ),
    m_smartIDTool ( 0 ),
    m_mcTool      ( 0 ),
    m_allDone     ( false ),
    m_usedRads    ( Rich::NRadiatorTypes, true )
{

  declareInterface<IRichPixelCreator>(this);

  // Define job option parameters
  declareProperty( "RichRecPixelLocation",
                   m_richRecPixelLocation = RichRecPixelLocation::Default );
  declareProperty( "RecoDigitsLocation",
                   m_recoDigitsLocation = RichDigitLocation::Default );
  declareProperty( "UseRadiators", m_usedRads );

}

StatusCode RichPixelCreatorFromCheatedRichDigits::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool", m_smartIDTool );
  acquireTool( "RichMCTruthTool", m_mcTool      );

  // Check which radiators to use
  if ( !m_usedRads[Rich::Aerogel] ) Warning("Pixel data for Aerogel is disabled",StatusCode::SUCCESS);
  if ( !m_usedRads[Rich::C4F10]   ) Warning("Pixel data for C4F10 is disabled",StatusCode::SUCCESS);
  if ( !m_usedRads[Rich::CF4]     ) Warning("Pixel data for CF4 is disabled",StatusCode::SUCCESS);

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  if (msgLevel(MSG::DEBUG)) incSvc()->addListener( this, IncidentType::EndEvent );

  // Make sure we are ready for a new event
  InitNewEvent();

  return sc;
}

StatusCode RichPixelCreatorFromCheatedRichDigits::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPixelCreatorFromCheatedRichDigits::handle ( const Incident& incident )
{
  // Update prior to start of event. Used to re-initialise data containers
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
  // Debug printout at the end of each event
  else if ( msgLevel(MSG::DEBUG) && IncidentType::EndEvent == incident.type() )
  {
    debug() << "Created " << richPixels()->size() << " RichRecPixels" << endreq;
  }
}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel *
RichPixelCreatorFromCheatedRichDigits::newPixel( const ContainedObject * obj ) const
{
  // Try to cast to RichDigit
  const RichDigit * digit = dynamic_cast<const RichDigit*>(obj);
  if ( !digit ) {
    Warning("Parent not of type RichDigit");
    return NULL;
  }

  // Find the MCRichDigit for this RichDigit
  const MCRichDigit * mcDigit = m_mcTool->mcRichDigit( digit );
  if ( !mcDigit ) {
    Warning("Failed to find MCRichDigit for given RichDigit"); return NULL;
  }

  // Loop over all MCRichHits for this MCRichDigit and make RichRecPixels
  RichRecPixel * returnPix = NULL;
  for ( SmartRefVector<MCRichHit>::const_iterator hit = mcDigit->hits().begin();
        hit != mcDigit->hits().end(); ++hit ) {
    returnPix = newPixelFromHit( digit, *hit );
  }

  // Return the RichRecPixel related to the last Hit
  return returnPix;
}

RichRecPixel *
RichPixelCreatorFromCheatedRichDigits::newPixelFromHit( const RichDigit * digit,
                                                        const MCRichHit * hit ) const
{

  // See if this RichRecPixel already exists
  if ( m_pixelDone[hit->key()] ) {
    return m_pixelExists[hit->key()];
  } else {

    RichRecPixel * newPixel = NULL;

    // Check if we are using this radiator
    if ( m_usedRads[hit->radiator()] ) {

      if ( digit->key().pixelDataAreValid() ) {

        // Find associated MCRichOpticalPhoton
        const MCRichOpticalPhoton * mcPhot = m_mcTool->mcOpticalPhoton(hit);
        if ( mcPhot ) {

          // Make a new RichRecPixel
          newPixel = new RichRecPixel();
          richPixels()->insert( newPixel );

          // Positions
          newPixel->setGlobalPosition( mcPhot->pdIncidencePoint() );
          newPixel->localPosition() =
            m_smartIDTool->globalToPDPanel(newPixel->globalPosition());

          // Set smartID
          newPixel->setSmartID( digit->key() );

          // Set parent information
          // Note - we are pretending to be RichDigits here...
          newPixel->setParentPixel( digit );
          newPixel->setParentType( Rich::PixelParent::Digit );

        }

      } else {
        Warning("RichSmartID does not contain valid pixel data !");
      }

    }

    // Add to reference map
    m_pixelExists [ hit->key() ] = newPixel;
    m_pixelDone   [ hit->key() ] = true;

    return newPixel;
  }

}

StatusCode RichPixelCreatorFromCheatedRichDigits::newPixels() const
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
            << m_recoDigitsLocation << endreq
            << "Created " << richPixels()->size() << " RichRecPixels at "
            << m_richRecPixelLocation << endreq;
  }

  return StatusCode::SUCCESS;
}

RichRecPixels * RichPixelCreatorFromCheatedRichDigits::richPixels() const
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
            iPixel != tdsPixels->end();
            ++iPixel ) {
        m_pixelExists[(*iPixel)->key()] = *iPixel;
        m_pixelDone[(*iPixel)->key()]   = true;
      }

    }
  }
  return m_pixels;
}
