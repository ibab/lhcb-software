// $Id: RichRecPixelTool.cpp,v 1.5 2003-06-27 15:14:12 cattanem Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RichRecPixelTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecPixelTool
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRecPixelTool>          s_factory ;
const        IToolFactory& RichRecPixelToolFactory = s_factory ;

// Standard constructor
RichRecPixelTool::RichRecPixelTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : AlgTool( type, name, parent ) {

  declareInterface<IRichRecPixelTool>(this);

  // Define job option parameters
  declareProperty( "RichRecPixelLocation",
                   m_richRecPixelLocation = RichRecPixelLocation::Default );
  declareProperty( "RecoDigitsLocation",
                   m_recoDigitsLocation = RichDigitLocation::Default );
  declareProperty( "SICBRichDigits", m_sicbDigits = true );

}

StatusCode RichRecPixelTool::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // Get pointer to EDS
  if ( !serviceLocator()->service( "EventDataSvc", m_evtDataSvc, true ) ) {
    msg << MSG::ERROR << "EventDataSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get Pointer to PixelFinder (temporary for SICB data)
  if ( !toolSvc()->retrieveTool( "PixelFinder", m_pixelFinder ) ) {
    msg << MSG::ERROR << "PixelFinder not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Retrieve particle property service
  if ( !toolSvc()->retrieveTool( "RichDetInterface", m_richDetInterface ) ) {
    msg << MSG::ERROR << "RichDetInterface not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Setup incident services
  IIncidentSvc * incSvc;
  if ( !serviceLocator()->service( "IncidentSvc", incSvc, true ) ) {
    msg << MSG::ERROR << "IncidentSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  } else {
    incSvc->addListener( this, "BeginEvent" ); // Informed of a new event
    //incSvc->addListener( this, "EndEvent"   ); // Informed at the end of event
    incSvc->release();
  }

  return sc;
}

StatusCode RichRecPixelTool::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  //Release the tools ans services
  if ( m_pixelFinder ) toolSvc()->releaseTool( m_pixelFinder );
  if ( m_richDetInterface ) toolSvc()->releaseTool( m_richDetInterface );

  if( 0 != m_evtDataSvc ) {
    m_evtDataSvc->release();
    m_evtDataSvc = 0;
  }
  
  return StatusCode::SUCCESS;
}

// Method that handles various Gaudi "software events"
void RichRecPixelTool::handle ( const Incident& incident ) {

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
RichRecPixel * RichRecPixelTool::newPixel( RichDigit * digit ) {

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

      // difference treatment if digits from SICB or OO
      HepPoint3D & gPosition = newPixel->globalPosition();
      HepPoint3D & lPosition = newPixel->localPosition();
      if ( m_sicbDigits ) {
        gPosition = 10.0 * (m_pixelFinder->globalPosition(id));
        lPosition = 10.0 * (m_pixelFinder->SICBlocalPosition(id));
        m_pixelFinder->convertSmartID( id ); //convert smartID to OO conventions
      } else { // the glorious future!!!
        m_richDetInterface->globalPosition( id, gPosition );
        // no method for local position as yet !
      }

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

StatusCode RichRecPixelTool::newPixels() {

  if ( m_allDone ) return StatusCode::SUCCESS;
  m_allDone = true;

  // Obtain smart data pointer to RichDigits
  SmartDataPtr<RichDigits> digits( m_evtDataSvc, m_recoDigitsLocation );
  if ( !digits ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Failed to locate digits at "
        << m_recoDigitsLocation << endreq;
    return StatusCode::FAILURE;
  }
  
  for ( RichDigits::iterator digit = digits->begin();
        digit != digits->end();
        ++digit ) { newPixel( *digit ); }
  
  return StatusCode::SUCCESS;
}

RichRecPixels * RichRecPixelTool::richPixels()
{ 
  return m_pixels;
}

RichDigit * RichRecPixelTool::parentRichDigit ( const RichRecPixel * pixel )
{
  return dynamic_cast<RichDigit*>( (ContainedObject*)pixel->parentPixel() );
}
