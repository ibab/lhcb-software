
/** @file RichPixelCreatorFromRichDigits.cpp
 *
 *  Implementation file for tool : RichPixelCreatorFromRichDigits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRichDigits.cpp,v 1.14 2004-10-13 09:52:41 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.13  2004/07/27 20:15:32  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

// local
#include "RichPixelCreatorFromRichDigits.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromRichDigits>          s_factory ;
const        IToolFactory& RichPixelCreatorFromRichDigitsFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromRichDigits::RichPixelCreatorFromRichDigits( const std::string& type,
                                                                const std::string& name,
                                                                const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_pixels      ( 0 ),
    m_smartIDTool ( 0 ),
    m_allDone     ( false ),
    m_bookKeep    ( true  ),
    m_usedDets    ( Rich::NRiches, true )
{

  declareInterface<IRichPixelCreator>(this);

  // Define job option parameters
  declareProperty( "RichRecPixelLocation",
                   m_richRecPixelLocation = RichRecPixelLocation::Default );
  declareProperty( "RecoDigitsLocation",
                   m_recoDigitsLocation = RichDigitLocation::Default );
  declareProperty( "DoBookKeeping", m_bookKeep );
  declareProperty( "UseDetectors", m_usedDets );

}

StatusCode RichPixelCreatorFromRichDigits::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool", m_smartIDTool );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  if (msgLevel(MSG::DEBUG)) incSvc()->addListener( this, IncidentType::EndEvent );

  // Make sure we are ready for a new event
  InitNewEvent();

  return StatusCode::SUCCESS;
}

StatusCode RichPixelCreatorFromRichDigits::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPixelCreatorFromRichDigits::handle ( const Incident& incident )
{
  // Update prior to start of event. Used to re-initialise data containers
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
  // Debug printout at the end of each event
  else if ( msgLevel(MSG::DEBUG) && IncidentType::EndEvent == incident.type() )
  {
    debug() << "Created " << richPixels()->size() << " RichRecPixels" << endreq;
  }
}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel *
RichPixelCreatorFromRichDigits::newPixel( const ContainedObject * obj ) const
{
  // Try to cast to RichDigit
  const RichDigit * digit = dynamic_cast<const RichDigit*>(obj);
  if ( !digit ) {
    Warning("Parent not of type RichDigit");
    return NULL;
  }

  // See if this RichRecPixel already exists
  RichRecPixel * pixel = ( m_bookKeep && m_pixelDone[digit->key()] ?
                           m_pixelExists[digit->key()] : buildPixel(digit) );

  // Add to reference map
  if ( m_bookKeep ) {
    m_pixelExists[ digit->key() ] = pixel;
    m_pixelDone  [ digit->key() ] = true;
  }

  return pixel;
}

RichRecPixel *
RichPixelCreatorFromRichDigits::buildPixel( const RichDigit * digit ) const
{

  RichRecPixel * newPixel = NULL;

  // RichDigit key
  const RichSmartID id = digit->key();
  if ( id.pixelDataAreValid() ) { // check it is valid

    // Check if we are using this detector
    if ( m_usedDets[id.rich()] ) {

      // Make a new RichRecPixel
      newPixel = new RichRecPixel();
      richPixels()->insert( newPixel );

      // Positions
      HepPoint3D & gPosition = newPixel->globalPosition();
      m_smartIDTool->globalPosition( id, gPosition );
      newPixel->localPosition() = m_smartIDTool->globalToPDPanel(gPosition);

      // Set smartID
      newPixel->setSmartID( id );

      // Set parent information
      newPixel->setParentPixel ( digit                    );
      newPixel->setParentType  ( Rich::PixelParent::Digit );

    }

  } else {
    Warning("RichSmartID does not contain valid pixel data !");
  }

  return newPixel;
}

StatusCode RichPixelCreatorFromRichDigits::newPixels() const
{
  if ( m_allDone ) return StatusCode::SUCCESS;
  m_allDone = true;

  // Obtain smart data pointer to RichDigits
  const RichDigits * digits = get<RichDigits>( m_recoDigitsLocation );

  // Loop over RichDigits and create working pixels
  richPixels()->reserve( digits->size() );
  for ( RichDigits::const_iterator digit = digits->begin();
        digit != digits->end(); ++digit ) { newPixel( *digit ); }

  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Located " << digits->size() << " RichDigits at "
            << m_recoDigitsLocation << endreq
            << "Created " << richPixels()->size() << " RichRecPixels at "
            << m_richRecPixelLocation << endreq;
  }

  return StatusCode::SUCCESS;
}

RichRecPixels * RichPixelCreatorFromRichDigits::richPixels() const
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
