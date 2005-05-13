
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorFromRawBuffer.cpp
 *
 *  Implementation file for tool : RichPixelCreatorFromRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRawBuffer.cpp,v 1.7 2005-05-13 15:20:38 jonrob Exp $
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
RichPixelCreatorFromRawBuffer::
RichPixelCreatorFromRawBuffer( const std::string& type,
                               const std::string& name,
                               const IInterface* parent )
  : RichPixelCreatorBase ( type, name, parent ),
    m_idTool             ( 0 ),
    m_decoder            ( 0 )
{
}

StatusCode RichPixelCreatorFromRawBuffer::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPixelCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool",    m_idTool  );
  acquireTool( "RichSmartIDDecoder", m_decoder );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  incSvc()->addListener( this, IncidentType::EndEvent );

  return sc;
}

StatusCode RichPixelCreatorFromRawBuffer::finalize()
{
  // Execute base class method
  return RichPixelCreatorBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPixelCreatorFromRawBuffer::handle ( const Incident& incident )
{
  // Update prior to start of event. Used to re-initialise data containers
  if      ( IncidentType::BeginEvent == incident.type() )
  {
    InitNewEvent();
  }
  // Debug printout at the end of each event
  else if ( IncidentType::EndEvent == incident.type() )
  {
    FinishEvent();
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
  RichRecPixel * pixel = ( bookKeep() && m_pixelDone[id] ? m_pixelExists[id] : 0 );
  if ( pixel ) return pixel;

  // Check this hit is OK
  if ( pixelIsOK(id) )
  {

    // Make a new RichRecPixel
    const HepPoint3D gPos = m_idTool->globalPosition( id );
    pixel = new RichRecPixel( id,                              // SmartID for pixel
                              gPos,                            // position in global coords
                              m_idTool->globalToPDPanel(gPos), // position in local coords
                              Rich::PixelParent::RawBuffer,    // parent type
                              0                                // pointer to parent (not available)
                              );
    savePixel( pixel );

  }

  // Add to reference map
  if ( bookKeep() )
  {
    m_pixelExists[ id ] = pixel;
    m_pixelDone  [ id ] = true;
  }

  return pixel;
}

StatusCode RichPixelCreatorFromRawBuffer::newPixels() const
{
  if ( !m_allDone )
  {
    m_allDone = true; // only once per event

    // Obtain RichSmartIDs
    const RichSmartID::Collection & smartIDs = m_decoder->allRichSmartIDs();

    // Reserve space
    richPixels()->reserve( smartIDs.size() );

    // Loop over RichSmartIDs and create working pixels
    for ( RichSmartID::Collection::const_iterator iID = smartIDs.begin();
          iID != smartIDs.end(); ++iID )
    {
      // Make a Pixel for this RichSmartID
      buildPixel(*iID);
    }

    // find iterators
    // note : we are relying on the sorting of the input RichSmartIDs here, so we
    // don't manually sort the RichRecPixels for speed unlike in other tool
    // implementations
    fillIterators();

    // Debug messages
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Created " << richPixels()->size() << " RichRecPixels at "
              << pixelLocation() << endreq;
    }

  }

  return StatusCode::SUCCESS;
}

