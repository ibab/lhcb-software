// $Id: RichPixelCreatorFromMCRichHits.cpp,v 1.3 2004-02-02 14:24:40 jonesc Exp $

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
    m_smartIDTool ( 0 )
{

  declareInterface<IRichPixelCreator>(this);

  // Define job option parameters
  declareProperty( "RichRecPixelLocation",
                   m_richRecPixelLocation = RichRecPixelLocation::Default );
  declareProperty( "MCRichHitLocation",
                   m_mcHitsLocation = MCRichHitLocation::Default );

}

StatusCode RichPixelCreatorFromMCRichHits::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool", m_smartIDTool );

  // Setup incident services
  IIncidentSvc * incSvc;
  if ( !serviceLocator()->service( "IncidentSvc", incSvc, true ) ) {
    msg << MSG::ERROR << "IncidentSvc not found" << endreq;
    return StatusCode::FAILURE;
  } else {
    incSvc->addListener( this, "BeginEvent" ); // Informed of a new event
    incSvc->release();
  }

  // Informational printout
  msg << MSG::DEBUG
      << " Using MCRichHits" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichPixelCreatorFromMCRichHits::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release services and tools
  releaseTool( m_smartIDTool );

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPixelCreatorFromMCRichHits::handle ( const Incident& incident ) {

  if ( "BeginEvent" == incident.type() ) {

    // Initialise navigation data
    m_pixelExists.clear();
    m_pixelDone.clear();
    m_allDone = false;

    SmartDataPtr<RichRecPixels> tdsPixels( eventSvc(),
                                           m_richRecPixelLocation );
    if ( !tdsPixels ) {

      // Reinitialise the Pixel Container
      m_pixels = new RichRecPixels();

      // Register new RichRecPhoton container to Gaudi data store
      if ( !eventSvc()->registerObject(m_richRecPixelLocation, m_pixels) ) {
        MsgStream msg( msgSvc(), name() );
        msg << MSG::ERROR << "Failed to register RichRecPixels at "
            << m_richRecPixelLocation << endreq;
      }

    } else {

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

  } // end begin event if

}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel * 
RichPixelCreatorFromMCRichHits::newPixel( const ContainedObject * obj ) const {

  // Try to cast to MCRichHit
  const MCRichHit * hit = dynamic_cast<const MCRichHit*>(obj);
  if ( !hit ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::WARNING << "Parent not of type MCRichHit" << endreq;
    return NULL;
  }

  const long int hitKey = hit->key();

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
        m_pixels->insert( newPixel );

        // Positions
        newPixel->setGlobalPosition( hit->entry() );
        // no method for local position as yet !
        //HepPoint3D & lPosition = newPixel->localPosition();

        // Set smartID
        newPixel->setSmartID( id );

        // Set parent information
        newPixel->setParentPixel( hit );
        newPixel->setParentType( Rich::RecPixel::MCHit );

      }
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
  SmartDataPtr<MCRichHits> hits( eventSvc(), m_mcHitsLocation );
  if ( !hits ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Failed to locate MCRichHits at "
        << m_mcHitsLocation << endreq;
    return StatusCode::FAILURE;
  } else {
    if ( msgLevel(MSG::DEBUG) ) {
      MsgStream msg( msgSvc(), name() );
      msg << MSG::DEBUG << "located " << hits->size() << " MCRichHits at "
          << m_mcHitsLocation << endreq;
    }
  }

  for ( MCRichHits::iterator hit = hits->begin();
        hit != hits->end();
        ++hit ) { newPixel( *hit ); }

  return StatusCode::SUCCESS;
}

RichRecPixels * RichPixelCreatorFromMCRichHits::richPixels() const
{
  return m_pixels;
}
