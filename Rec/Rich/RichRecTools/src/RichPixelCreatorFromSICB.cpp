// $Id: RichPixelCreatorFromSICB.cpp,v 1.5 2003-11-25 14:06:40 jonrob Exp $

// local
#include "RichPixelCreatorFromSICB.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPixelCreatorFromSICB
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromSICB>          s_factory ;
const        IToolFactory& RichPixelCreatorFromSICBFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromSICB::RichPixelCreatorFromSICB( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichPixelCreator>(this);

  // Define job option parameters
  declareProperty( "RichRecPixelLocation",
                   m_richRecPixelLocation = RichRecPixelLocation::Default );
  declareProperty( "RecoDigitsLocation",
                   m_recoDigitsLocation = RichDigitLocation::Default );

}

StatusCode RichPixelCreatorFromSICB::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool("SICBPixelGeom", m_pixelFinder);

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
      << " Using SICB RichDigits" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichPixelCreatorFromSICB::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release services and tools
  releaseTool( m_pixelFinder );

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPixelCreatorFromSICB::handle ( const Incident& incident ) {

  if ( "BeginEvent" == incident.type() ) {

    m_allDone = false;

    // Initialise navigation data
    m_pixelExists.clear();
    m_pixelDone.clear();

    SmartDataPtr<RichRecPixels> tdsPixels( eventSvc(),
                                           m_richRecPixelLocation );
    if ( !tdsPixels ) {

      // Reinitialise the Pixel Container
      m_pixels = new RichRecPixels();

      // Register new RichRecPixel container to Gaudi data store
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
RichRecPixel * RichPixelCreatorFromSICB::newPixel( const ContainedObject * obj ) {

  // Try to cast to RichDigit
  const RichDigit * digit = dynamic_cast<const RichDigit*>(obj);
  if ( !digit ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::WARNING << "Parent not of type RichDigit" << endreq;
    return NULL;
  }

  RichSmartID id = digit->key();

  // See if this RichRecPixel already exists
  if ( m_pixelDone[(long int)id] ) {
    return m_pixelExists[(long int)id];
  } else {

    RichRecPixel * newPixel = NULL;

    if ( id ) {

      // Make a new RichRecPixel
      newPixel = new RichRecPixel();
      m_pixels->insert( newPixel );

      // position
      HepPoint3D & gPosition = newPixel->globalPosition();
      HepPoint3D & lPosition = newPixel->localPosition();
      gPosition = 10.0 * (m_pixelFinder->globalPosition(id));
      lPosition = 10.0 * (m_pixelFinder->localPosition(id));
      m_pixelFinder->convertSmartID( id ); // convert smartID to OO conventions

      // Set smartID
      newPixel->setSmartID( id );

      // Set parent information
      newPixel->setParentPixel( digit );
      newPixel->setParentType( Rich::RecPixel::Digit );
    }

    // Add to reference map
    m_pixelExists[ (long int)(digit->key()) ] = newPixel;
    m_pixelDone  [ (long int)(digit->key()) ] = true;

    return newPixel;
  }

}

StatusCode RichPixelCreatorFromSICB::newPixels() {

  if ( m_allDone ) return StatusCode::SUCCESS;
  m_allDone = true;

  // Obtain smart data pointer to RichDigits
  SmartDataPtr<RichDigits> digits( eventSvc(), m_recoDigitsLocation );
  if ( !digits ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Failed to locate digits at "
        << m_recoDigitsLocation << endreq;
    return StatusCode::FAILURE;
  }

  // Loop over RichDigits and create working pixels
  for ( RichDigits::iterator digit = digits->begin();
        digit != digits->end(); ++digit ) {
    newPixel( *digit );
  }

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::DEBUG
        << "Located " << digits->size() << " RichDigits at "
        << m_recoDigitsLocation << endreq
        << "Created " << m_pixels->size() << " RichRecPixels at "
        << m_richRecPixelLocation << endreq;
  }

  if ( digits->size() != m_pixels->size() ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::WARNING
        << "Created " << m_pixels->size() << " RichRecPixels from "
        << digits->size() << " RichDigits !!" << endreq;
  }

  return StatusCode::SUCCESS;
}

RichRecPixels *& RichPixelCreatorFromSICB::richPixels()
{
  return m_pixels;
}
