// $Id: RichPixelCreatorFromMCRichHits.cpp,v 1.9 2004-07-12 14:31:33 jonrob Exp $

// local
#include "RichPixelCreatorFromMCRichHits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPixelCreatorFromMCRichHits
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromMCRichHits>          s_factory ;
const        IToolFactory& RichPixelCreatorFromMCRichHitsFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromMCRichHits::RichPixelCreatorFromMCRichHits( const std::string& type,
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
  declareProperty( "MCRichHitLocation",
                   m_mcHitsLocation = MCRichHitLocation::Default );

}

StatusCode RichPixelCreatorFromMCRichHits::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool", m_smartIDTool );
  acquireTool( "RichMCTruthTool", m_mcTool      );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // Make sure we are ready for a new event
  InitNewEvent();

  return StatusCode::SUCCESS;
}

StatusCode RichPixelCreatorFromMCRichHits::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPixelCreatorFromMCRichHits::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) InitNewEvent();
}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel *
RichPixelCreatorFromMCRichHits::newPixel( const ContainedObject * obj ) const {

  // Try to cast to MCRichHit
  const MCRichHit * hit = dynamic_cast<const MCRichHit*>(obj);
  if ( !hit ) {
    Warning("Parent not of type MCRichHit");
    return NULL;
  }

  const long int hitKey = hit->key();

  // See if this RichRecPixel already exists
  if ( m_pixelDone[hitKey] ) {
    return m_pixelExists[hitKey];
  } else {

    RichRecPixel * newPixel = NULL;

    RichSmartID id(0);
    StatusCode sc = m_smartIDTool->smartID( hit->entry(), id );
    if ( sc.isSuccess() && id.pixelDataAreValid() ) {

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
        newPixel->setSmartID( id );

        // Set parent information
        newPixel->setParentPixel( hit );
        newPixel->setParentType( Rich::RecPixel::MCHit );

      }

    } else {
      Warning("RichSmartID does not contain valid pixel data !");
    }

    // Add to reference map
    m_pixelExists[hitKey] = newPixel;
    m_pixelDone[hitKey] = true;

    return newPixel;
  }

}

StatusCode RichPixelCreatorFromMCRichHits::newPixels() const {

  if ( m_allDone ) return StatusCode::SUCCESS;
  m_allDone = true;

  // Obtain smart data pointer to RichDigits
  const MCRichHits * hits = get<MCRichHits>( m_mcHitsLocation );

  // Loop over hits and create working pixels
  for ( MCRichHits::const_iterator hit = hits->begin();
        hit != hits->end();
        ++hit ) { newPixel( *hit ); }

  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Located " << hits->size() << " MCRichHits at "
            << m_mcHitsLocation << endreq
            << "Created " << richPixels()->size() << " RichRecPixels at "
            << m_richRecPixelLocation << endreq;
  }

  return StatusCode::SUCCESS;
}

RichRecPixels * RichPixelCreatorFromMCRichHits::richPixels() const
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
        m_pixelExists[(long int)(*iPixel)->smartID()] = *iPixel;
        m_pixelDone[(long int)(*iPixel)->smartID()] = true;
      }

    }
  }
  return m_pixels;
}
