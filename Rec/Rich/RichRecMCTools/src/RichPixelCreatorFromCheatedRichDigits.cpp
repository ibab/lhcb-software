// $Id: RichPixelCreatorFromCheatedRichDigits.cpp,v 1.2 2003-11-25 14:01:50 jonesc Exp $

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
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichPixelCreator>(this);

  // Define job option parameters
  declareProperty( "RichRecPixelLocation",
                   m_richRecPixelLocation = RichRecPixelLocation::Default );
  declareProperty( "RecoDigitsLocation",
                   m_recoDigitsLocation = RichDigitLocation::Default );

}

StatusCode RichPixelCreatorFromCheatedRichDigits::initialize() {

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
  acquireTool( "RichDetInterface",   m_richDetInt );
  acquireTool( "RichRecMCTruthTool", m_mcTool     );

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
      << " Using MCRichHits asscociated to RichDigits" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichPixelCreatorFromCheatedRichDigits::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release services and tools
  if ( m_evtDataSvc ) { m_evtDataSvc->release(); m_evtDataSvc = 0; }
  releaseTool( m_richDetInt );
  releaseTool( m_mcTool     );

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPixelCreatorFromCheatedRichDigits::handle ( const Incident& incident ) {

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
        m_pixelExists[(long int)(*iPixel)->smartID()] = *iPixel;
        m_pixelDone[(long int)(*iPixel)->smartID()] = true;
      }

    }

  } // end begin event if

}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel * 
RichPixelCreatorFromCheatedRichDigits::newPixel( const ContainedObject * obj ) {

  // Try to cast to RichDigit
  const RichDigit * digit = dynamic_cast<const RichDigit*>(obj);
  if ( !digit ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::WARNING << "Parent not of type RichDigit" << endreq;
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
                                                        const MCRichHit * hit ) {

  long int hitKey = hit->key();

  // See if this RichRecPixel already exists
  if ( m_pixelDone[hitKey] ) {
    return m_pixelExists[hitKey];
  } else {

    RichRecPixel * newPixel = NULL;

    if ( 0 != hitKey ) {

      RichSmartID id(0);
      StatusCode sc = m_richDetInt->smartID( hit->entry(), id );
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

StatusCode RichPixelCreatorFromCheatedRichDigits::newPixels() {

  if ( m_allDone ) return StatusCode::SUCCESS;
  m_allDone = true;

  // Obtain smart data pointer to RichDigits
  SmartDataPtr<RichDigits> digits( m_evtDataSvc, m_recoDigitsLocation );
  if ( !digits ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Failed to locate RichDigits at "
        << m_recoDigitsLocation << endreq;
    return StatusCode::FAILURE;
  } else {
    if ( msgLevel(MSG::DEBUG) ) {
      MsgStream msg( msgSvc(), name() );
      msg << MSG::DEBUG << "located " << digits->size() << " RichDigits at "
          <<  m_recoDigitsLocation << endreq;
    }
  }

  for ( RichDigits::iterator digit = digits->begin();
        digit != digits->end();
        ++digit ) { newPixel( *digit ); }

  return StatusCode::SUCCESS;
}

RichRecPixels *& RichPixelCreatorFromCheatedRichDigits::richPixels()
{
  return m_pixels;
}
