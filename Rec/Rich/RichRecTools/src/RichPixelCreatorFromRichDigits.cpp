// $Id: RichPixelCreatorFromRichDigits.cpp,v 1.2 2003-07-03 14:46:59 jonesc Exp $

// local
#include "RichPixelCreatorFromRichDigits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPixelCreatorFromRichDigits
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromRichDigits>          s_factory ;
const        IToolFactory& RichPixelCreatorFromRichDigitsFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromRichDigits::RichPixelCreatorFromRichDigits( const std::string& type,
                                                                const std::string& name,
                                                                const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichPixelCreator>(this);

  declareProperty( "RichRecPixelLocation",
                   m_richRecPixelLocation = RichRecPixelLocation::Default );
  declareProperty( "RecoDigitsLocation",
                   m_recoDigitsLocation = RichDigitLocation::Default );

}

StatusCode RichPixelCreatorFromRichDigits::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Get pointer to EDS
  if ( !serviceLocator()->service( "EventDataSvc", m_evtDataSvc, true ) ) {
    msg << MSG::ERROR << "EventDataSvc not found" << endreq;
    return StatusCode::FAILURE;
  }

  // Acquire instances of tools
  acquireTool("RichDetInterface", m_richDetInt);

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
      << " Using OO RichDigits" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichPixelCreatorFromRichDigits::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release services and tools
  if ( m_evtDataSvc ) { m_evtDataSvc->release(); m_evtDataSvc = 0; }
  releaseTool( m_richDetInt );

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPixelCreatorFromRichDigits::handle ( const Incident& incident ) {

  if ( "BeginEvent" == incident.type() ) {

    m_allDone = false;

    // Initialise navigation data
    m_pixelExists.clear();
    m_pixelDone.clear();

    SmartDataPtr<RichRecPixels> tdsPixels( m_evtDataSvc,
                                           m_richRecPixelLocation );
    if ( !tdsPixels ) {

      // Reinitialise the Pixel Container
      m_pixels = new RichRecPixels();

      // Register new RichRecPhoton container to Gaudi data store
      if ( !m_evtDataSvc->registerObject(m_richRecPixelLocation, m_pixels) ) {
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
        m_pixelExists[(int)(*iPixel)->smartID()] = *iPixel;
        m_pixelDone[(int)(*iPixel)->smartID()] = true;
      }

    }

  } // end begin event if

}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel * RichPixelCreatorFromRichDigits::newPixel( ContainedObject * obj ) {

  // Try to cast to RichDigit
  RichDigit * digit = dynamic_cast<RichDigit*>(obj);
  if ( !digit ) { 
    MsgStream msg( msgSvc(), name() );
    msg << MSG::WARNING << "Parent not of type RichDigit" << endreq;
    return NULL;
  }
  
  RichSmartID id = digit->key();

  // See if this RichRecPixel already exists
  if ( m_pixelDone[(int)id] ) {
    return m_pixelExists[(int)id];
  } else {

    RichRecPixel * newPixel = NULL;

    if ( id ) {

      // Make a new RichRecPixel
      newPixel = new RichRecPixel();
      m_pixels->insert( newPixel );

      // Positions
      HepPoint3D & gPosition = newPixel->globalPosition();
      m_richDetInt->globalPosition( id, gPosition );
      // no method for local position as yet !
      //HepPoint3D & lPosition = newPixel->localPosition();

      // Set smartID
      newPixel->setSmartID( id );

      // Set parent information
      newPixel->setParentPixel( digit );
      newPixel->setParentType( Rich::RecPixel::Digit );
    }

    // Add to reference map
    m_pixelExists[(int)(digit->key())] = newPixel;
    m_pixelDone[(int)(digit->key())] = true;

    return newPixel;
  }

}

StatusCode RichPixelCreatorFromRichDigits::newPixels() {

  if ( m_allDone ) return StatusCode::SUCCESS;
  m_allDone = true;

  MsgStream msg( msgSvc(), name() );

  // Obtain smart data pointer to RichDigits
  SmartDataPtr<RichDigits> digits( m_evtDataSvc, m_recoDigitsLocation );
  if ( !digits ) {
    msg << MSG::ERROR << "Failed to locate digits at "
        << m_recoDigitsLocation << endreq;
    return StatusCode::FAILURE;
  } else {
    msg << MSG::DEBUG << "located " << digits->size() << " RichDigits at "
        <<  m_recoDigitsLocation << endreq;
  }

  for ( RichDigits::iterator digit = digits->begin();
        digit != digits->end();
        ++digit ) { newPixel( *digit ); }

  return StatusCode::SUCCESS;
}

RichRecPixels *& RichPixelCreatorFromRichDigits::richPixels()
{
  return m_pixels;
}
