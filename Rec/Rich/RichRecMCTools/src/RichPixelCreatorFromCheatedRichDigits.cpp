// $Id: RichPixelCreatorFromCheatedRichDigits.cpp,v 1.4 2004-03-16 13:41:43 jonesc Exp $

// local
#include "RichPixelCreatorFromCheatedRichDigits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPixelCreatorFromCheatedRichDigits
//
// 15/09/2003 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromCheatedRichDigits>          s_factory ;
const        IToolFactory& RichPixelCreatorFromCheatedRichDigitsFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromCheatedRichDigits::RichPixelCreatorFromCheatedRichDigits( const std::string& type,
                                                                              const std::string& name,
                                                                              const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_pixels      ( 0 ),
    m_smartIDTool ( 0 ),
    m_mcTool      ( 0 ),
    m_allDone     ( false )
{

  declareInterface<IRichPixelCreator>(this);

  // Define job option parameters
  declareProperty( "RichRecPixelLocation",
                   m_richRecPixelLocation = RichRecPixelLocation::Default );
  declareProperty( "RecoDigitsLocation",
                   m_recoDigitsLocation = RichDigitLocation::Default );

}

StatusCode RichPixelCreatorFromCheatedRichDigits::initialize()
{
  debug() << "Initialize" << endreq;

  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool", m_smartIDTool );
  acquireTool( "RichRecMCTruthTool", m_mcTool     );

  // Setup incident services
  IIncidentSvc * incSvc = svc<IIncidentSvc>( "IncidentSvc", true );
  incSvc->addListener( this, "BeginEvent" ); // Informed of a new event

  // Make sure we are ready for a new event
  InitNewEvent();

  // Informational printout
  debug() << " Using MCRichHits asscociated to RichDigits" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichPixelCreatorFromCheatedRichDigits::finalize()
{
  debug() << "Finalize" << endreq;

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPixelCreatorFromCheatedRichDigits::handle ( const Incident& incident )
{
  if ( "BeginEvent" == incident.type() ) InitNewEvent();
}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel *
RichPixelCreatorFromCheatedRichDigits::newPixel( const ContainedObject * obj ) const {

  // Try to cast to RichDigit
  const RichDigit * digit = dynamic_cast<const RichDigit*>(obj);
  if ( !digit ) {
    warning() << "Parent not of type RichDigit" << endreq;
    return NULL;
  }

  // Find MCRichDigit
  const MCRichDigit * mcDigit = m_mcTool->mcRichDigit( digit );
  if ( !mcDigit ) return NULL;

  // Loop over all MCRichHits for this MCRichDigit and make RichRecPixels
  RichRecPixel * returnPix = NULL;
  for ( SmartRefVector<MCRichHit>::const_iterator hit = mcDigit->hits().begin();
        hit != mcDigit->hits().end(); ++hit ) { returnPix = newPixelFromHit( digit, *hit ); }

  // Return the RichRecPixel related to the last Hit
  return returnPix;
}

RichRecPixel *
RichPixelCreatorFromCheatedRichDigits::newPixelFromHit( const RichDigit * digit,
                                                        const MCRichHit * hit ) const {

  RichSmartID::KeyType hitKey = hit->key();

  // See if this RichRecPixel already exists
  if ( m_pixelDone[hitKey] ) {
    return m_pixelExists[hitKey];
  } else {

    RichRecPixel * newPixel = NULL;

    if ( 0 != hitKey ) {

      RichSmartID id(0);
      StatusCode sc = m_smartIDTool->smartID( hit->entry(), id );
      if ( sc.isSuccess() && id.isValid() ) {

        // Make a new RichRecPixel
        newPixel = new RichRecPixel();
        richPixels()->insert( newPixel );

        // Positions
        newPixel->setGlobalPosition( hit->entry() );
        // no method for local position as yet !
        //HepPoint3D & lPosition = newPixel->localPosition();

        // Set smartID
        newPixel->setSmartID( id );

        // Set parent information
        // Note - we are pretending to be RichDigits here...
        newPixel->setParentPixel( digit );
        newPixel->setParentType( Rich::RecPixel::Digit );

      }
    }

    // Add to reference map
    m_pixelExists[hitKey] = newPixel;
    m_pixelDone[hitKey] = true;

    return newPixel;
  }

}

StatusCode RichPixelCreatorFromCheatedRichDigits::newPixels() const {

  if ( m_allDone ) return StatusCode::SUCCESS;
  m_allDone = true;

  // Obtain smart data pointer to RichDigits
  SmartDataPtr<RichDigits> digits( evtSvc(), m_recoDigitsLocation );
  if ( !digits ) {
    return Error( "Failed to locate RichDigits at " + m_recoDigitsLocation );
  } else {
    debug() << "located " << digits->size() << " RichDigits at "
            <<  m_recoDigitsLocation << endreq;
  }

  for ( RichDigits::iterator digit = digits->begin();
        digit != digits->end();
        ++digit ) { newPixel( *digit ); }

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

      // Set smartref to TES pixel container
      m_pixels = tdsPixels;

      // Remake local pixel reference map
      for ( RichRecPixels::const_iterator iPixel = tdsPixels->begin();
            iPixel != tdsPixels->end();
            ++iPixel ) {
        m_pixelExists[(*iPixel)->smartID()] = *iPixel;
        m_pixelDone[(*iPixel)->smartID()] = true;
      }

    }
  }
  return m_pixels;
}
