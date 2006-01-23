
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorFromRawBuffer.cpp
 *
 *  Implementation file for tool : RichPixelCreatorFromRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRawBuffer.cpp,v 1.11 2006-01-23 14:20:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/10/2004
 */
//---------------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromRawBuffer.h"

// namespaces
using namespace LHCb;

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
    m_decoder            ( 0 ) { }

StatusCode RichPixelCreatorFromRawBuffer::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPixelCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool",    m_idTool,  0, true );
  acquireTool( "RichSmartIDDecoder", m_decoder, 0, true );

  return sc;
}

StatusCode RichPixelCreatorFromRawBuffer::finalize()
{
  // Execute base class method
  return RichPixelCreatorBase::finalize();
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
    const Gaudi::XYZPoint gPos = m_idTool->globalPosition( id );
    pixel = new RichRecPixel( id,                              // SmartID for pixel
                              gPos,                            // position in global coords
                              m_idTool->globalToPDPanel(gPos), // position in local coords
                              Rich::PixelParent::RawBuffer,    // parent type
                              0                                // pointer to parent (not available)
                              );

    // compute corrected local coordinates
    computeRadCorrLocalPositions( pixel );

    // save to TES container in tool
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
    const RichSmartID::Vector & smartIDs = m_decoder->allRichSmartIDs();

    // Reserve space
    richPixels()->reserve( smartIDs.size() );

    // Loop over RichSmartIDs and create working pixels
    for ( RichSmartID::Vector::const_iterator iID = smartIDs.begin();
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

