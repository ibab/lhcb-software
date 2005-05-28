
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromCheatedRichDigits.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPixelCreatorFromCheatedRichDigits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromCheatedRichDigits.cpp,v 1.15 2005-05-28 16:45:48 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */
//-----------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromCheatedRichDigits.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromCheatedRichDigits>          s_factory ;
const        IToolFactory& RichPixelCreatorFromCheatedRichDigitsFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromCheatedRichDigits::
RichPixelCreatorFromCheatedRichDigits( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
  : RichPixelCreatorBase( type, name, parent ),
    m_smartIDTool ( 0 ),
    m_mcTool      ( 0 ),
    m_usedRads    ( Rich::NRadiatorTypes, true )
{

  // Define job option parameters
  declareProperty( "RecoDigitsLocation",
                   m_recoDigitsLocation = RichDigitLocation::Default );
  declareProperty( "UseRadiators", m_usedRads );

}

StatusCode RichPixelCreatorFromCheatedRichDigits::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPixelCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool", m_smartIDTool );
  acquireTool( "RichMCTruthTool", m_mcTool      );

  // Check which radiators to use
  if ( !m_usedRads[Rich::Aerogel] ) Warning("Pixel data for Aerogel is disabled",StatusCode::SUCCESS);
  if ( !m_usedRads[Rich::C4F10]   ) Warning("Pixel data for C4F10 is disabled",StatusCode::SUCCESS);
  if ( !m_usedRads[Rich::CF4]     ) Warning("Pixel data for CF4 is disabled",StatusCode::SUCCESS);

  return sc;
}

StatusCode RichPixelCreatorFromCheatedRichDigits::finalize()
{
  // Execute base class method
  return RichPixelCreatorBase::finalize();
}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel *
RichPixelCreatorFromCheatedRichDigits::newPixel( const ContainedObject * obj ) const
{
  // Try to cast to RichDigit
  const RichDigit * digit = dynamic_cast<const RichDigit*>(obj);
  if ( !digit ) {
    Warning("Parent not of type RichDigit");
    return NULL;
  }

  // Find the MCRichDigit for this RichDigit
  const MCRichDigit * mcDigit = m_mcTool->mcRichDigit( digit );
  if ( !mcDigit ) {
    Warning("Failed to find MCRichDigit for given RichDigit"); return NULL;
  }

  // Loop over all MCRichHits for this MCRichDigit and make RichRecPixels
  RichRecPixel * returnPix = NULL;
  for ( SmartRefVector<MCRichHit>::const_iterator hit = mcDigit->hits().begin();
        hit != mcDigit->hits().end(); ++hit ) {
    returnPix = newPixelFromHit( digit, *hit );
  }

  // Return the RichRecPixel related to the last Hit
  return returnPix;
}

RichRecPixel *
RichPixelCreatorFromCheatedRichDigits::newPixelFromHit( const RichDigit * digit,
                                                        const MCRichHit * hit ) const
{

  // See if this RichRecPixel already exists
  if ( bookKeep() && m_pixelDone[hit->key()] )
  {
    return m_pixelExists[hit->key()];
  }
  else
  {

    RichRecPixel * newPixel = NULL;

    // Check if we are using this radiator
    if ( m_usedRads[hit->radiator()] )
    {

      // Check this hit is OK
      if ( pixelIsOK(digit->key()) )
      {

        // Find associated MCRichOpticalPhoton
        const MCRichOpticalPhoton * mcPhot = m_mcTool->mcOpticalPhoton(hit);
        if ( mcPhot ) {

          // Make a new RichRecPixel
          newPixel = new RichRecPixel();

          // Positions
          newPixel->setGlobalPosition( mcPhot->pdIncidencePoint() );
          newPixel->localPosition() =
            m_smartIDTool->globalToPDPanel(newPixel->globalPosition());

          // Set smartID
          newPixel->setSmartID( digit->key() );

          // Set parent information
          // Note - we are pretending to be RichDigits here...
          newPixel->setParentPixel( digit );
          newPixel->setParentType( Rich::PixelParent::Digit );

          // save the pixel
          savePixel( newPixel );

        }

      }

    }

    // Add to reference map
    if ( bookKeep() )
    {
      m_pixelExists [ hit->key() ] = newPixel;
      m_pixelDone   [ hit->key() ] = true;
    }

    return newPixel;
  }

}

StatusCode RichPixelCreatorFromCheatedRichDigits::newPixels() const
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

    // Make sure pixels are sorted
    // (Maybe not needed, but speed not so important anyway when using MC)
    sortPixels();

    // find iterators
    fillIterators();

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
