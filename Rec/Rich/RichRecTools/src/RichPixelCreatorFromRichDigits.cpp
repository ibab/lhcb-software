
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromRichDigits.cpp
 *
 *  Implementation file for tool : RichPixelCreatorFromRichDigits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRichDigits.cpp,v 1.19 2005-05-28 13:10:53 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

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
  : RichPixelCreatorBase ( type, name, parent ),
    m_idTool             ( 0 )
{

  // Define job option parameters
  declareProperty( "RecoDigitsLocation",
                   m_recoDigitsLocation = RichDigitLocation::Default );

}

StatusCode RichPixelCreatorFromRichDigits::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPixelCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool", m_idTool  );

  return sc;
}

StatusCode RichPixelCreatorFromRichDigits::finalize()
{
  // Execute base class method
  return RichPixelCreatorBase::finalize();
}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel *
RichPixelCreatorFromRichDigits::newPixel( const ContainedObject * obj ) const
{
  // Try to cast to RichDigit
  const RichDigit * digit = dynamic_cast<const RichDigit*>(obj);
  if ( !digit )
  {
    Warning("Parent not of type RichDigit");
    return NULL;
  }

  // See if this RichRecPixel already exists
  RichRecPixel * pixel = ( bookKeep() && m_pixelDone[digit->key()] ?
                           m_pixelExists[digit->key()] : buildPixel(digit) );

  // Add to reference map
  if ( bookKeep() )
  {
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

  // Check this hit is OK
  if ( pixelIsOK(id) )
  {

    // Make a new RichRecPixel
    const HepPoint3D gPos = m_idTool->globalPosition( id );
    newPixel = new RichRecPixel( id,                              // SmartID for pixel
                                 gPos,                            // position in global coords
                                 m_idTool->globalToPDPanel(gPos), // position in local coords
                                 Rich::PixelParent::Digit,        // parent type
                                 digit                            // pointer to parent
                                 );
    savePixel( newPixel );

  }

  return newPixel;
}

StatusCode RichPixelCreatorFromRichDigits::newPixels() const
{
  if ( !m_allDone )
  {
    m_allDone = true;

    // Obtain smart data pointer to RichDigits
    const RichDigits * digits = get<RichDigits>( m_recoDigitsLocation );

    // Reserve space
    richPixels()->reserve( digits->size() );

    // Loop over RichDigits and create working pixels
    for ( RichDigits::const_iterator digit = digits->begin();
          digit != digits->end(); ++digit )
    {
      // Make a Pixel for this RichSmartID
      newPixel( *digit );
    }

    // find iterators
    fillIterators();

    // Debug messages
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Located " << digits->size() << " RichDigits at "
              << m_recoDigitsLocation << endreq
              << "Created " << richPixels()->size() << " RichRecPixels at "
              << pixelLocation() << endreq;
    }

  }

  return StatusCode::SUCCESS;
}

