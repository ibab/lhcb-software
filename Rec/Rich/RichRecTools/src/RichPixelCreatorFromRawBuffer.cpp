
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorFromRawBuffer.cpp
 *
 *  Implementation file for tool : RichPixelCreatorFromRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRawBuffer.cpp,v 1.5 2005-02-02 10:08:00 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/10/2004
 */
//---------------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromRawBuffer.h"

//---------------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromRawBuffer>          s_factory ;
const        IToolFactory& RichPixelCreatorFromRawBufferFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromRawBuffer::RichPixelCreatorFromRawBuffer( const std::string& type,
                                                              const std::string& name,
                                                              const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_pixels        ( 0 ),
    m_idTool        ( 0 ),
    m_decoder       ( 0 ),
    m_allDone       ( false ),
    m_bookKeep      ( false ),
    m_usedDets      ( Rich::NRiches, true )
{

  declareInterface<IRichPixelCreator>(this);

  // Define job option parameters
  declareProperty( "RichRecPixelLocation",
                   m_richRecPixelLocation = RichRecPixelLocation::Default );
  declareProperty( "DoBookKeeping", m_bookKeep );
  declareProperty( "UseDetectors", m_usedDets );

}

StatusCode RichPixelCreatorFromRawBuffer::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool",    m_idTool  );
  acquireTool( "RichSmartIDDecoder", m_decoder );

  // Check which detectors to use
  if ( !m_usedDets[Rich::Rich1] ) Warning("Pixels for RICH1 are disabled",StatusCode::SUCCESS);
  if ( !m_usedDets[Rich::Rich2] ) Warning("Pixels for RICH2 are disabled",StatusCode::SUCCESS);

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  if (msgLevel(MSG::DEBUG)) incSvc()->addListener( this, IncidentType::EndEvent );

  // Make sure we are ready for a new event
  InitNewEvent();

  return sc;
}

StatusCode RichPixelCreatorFromRawBuffer::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPixelCreatorFromRawBuffer::handle ( const Incident& incident )
{
  // Update prior to start of event. Used to re-initialise data containers
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
  // Debug printout at the end of each event
  else if ( msgLevel(MSG::DEBUG) && IncidentType::EndEvent == incident.type() )
  {
    debug() << "Created " << richPixels()->size() << " RichRecPixels" << endreq;
  }
}

// Forms a new RichRecPixel object
RichRecPixel *
RichPixelCreatorFromRawBuffer::newPixel( const ContainedObject * /* obj */ ) const
{
  // This method is not valid for this implementation
  Exception( "Unimplemented method RichPixelCreatorFromRawBuffer::newPixel" );
  return 0;
}

RichRecPixel *
RichPixelCreatorFromRawBuffer::buildPixel( const RichSmartID id ) const
{

  // See if this RichRecPixel already exists
  RichRecPixel * pixel = ( m_bookKeep && m_pixelDone[id] ? m_pixelExists[id] : 0 );
  if ( pixel ) return pixel;

  if ( id.pixelDataAreValid() ) { // check it is valid

    // Check if we are using this detector
    if ( m_usedDets[id.rich()] ) {

      // Make a new RichRecPixel
      const HepPoint3D gPos = m_idTool->globalPosition( id );
      pixel = new RichRecPixel( id,                              // SmartID for pixel
                                gPos,                            // position in global coords
                                m_idTool->globalToPDPanel(gPos), // position in local coords
                                Rich::PixelParent::RawBuffer,    // parent type
                                0                                // pointer to parent (not available)
                                );
      richPixels()->insert( pixel );

    }

  } else {
    Warning("RichSmartID does not contain valid pixel data !");
  }

  // Add to reference map
  if ( m_bookKeep ) {
    m_pixelExists[ id ] = pixel;
    m_pixelDone  [ id ] = true;
  }

  return pixel;
}

StatusCode RichPixelCreatorFromRawBuffer::newPixels() const
{
  if ( !m_allDone ) {
    m_allDone = true;

    // Obtain RichSmartIDs
    const RichSmartID::Collection & smartIDs = m_decoder->allRichSmartIDs();

    // Loop over RichDigits and create working pixels
    richPixels()->reserve( smartIDs.size() );
    for ( RichSmartID::Collection::const_iterator iID = smartIDs.begin();
          iID != smartIDs.end(); ++iID ) { buildPixel(*iID); }

    if ( msgLevel(MSG::DEBUG) ) {
      debug() << "Created " << richPixels()->size() << " RichRecPixels at "
              << m_richRecPixelLocation << endreq;
    }

  }

  return StatusCode::SUCCESS;
}

RichRecPixels * RichPixelCreatorFromRawBuffer::richPixels() const
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

      if ( m_bookKeep ) {
        // Remake local pixel reference map
        for ( RichRecPixels::const_iterator iPixel = tdsPixels->begin();
              iPixel != tdsPixels->end(); ++iPixel ) {
          m_pixelExists [ (*iPixel)->smartID() ] = *iPixel;
          m_pixelDone   [ (*iPixel)->smartID() ] = true;
        }
      }

    }
  }

  return m_pixels;
}
