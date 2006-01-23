
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromMCRichHits.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPixelCreatorFromMCRichHits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromMCRichHits.cpp,v 1.16 2006-01-23 14:09:59 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromMCRichHits.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromMCRichHits>          s_factory ;
const        IToolFactory& RichPixelCreatorFromMCRichHitsFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromMCRichHits::
RichPixelCreatorFromMCRichHits( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : RichPixelCreatorBase( type, name, parent ),
    m_smartIDTool ( 0 ),
    m_mcTool      ( 0 ),
    m_usedRads    ( Rich::NRadiatorTypes, true )
{

  // Define job option parameters
  declareProperty( "MCRichHitLocation",
                   m_mcHitsLocation = MCRichHitLocation::Default );
  declareProperty( "UseRadiators", m_usedRads );

}

StatusCode RichPixelCreatorFromMCRichHits::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPixelCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool", m_smartIDTool, 0, true );
  acquireTool( "RichMCTruthTool", m_mcTool, 0, true      );

  // Check which radiators to use
  if ( !m_usedRads[Rich::Aerogel] ) Warning("Pixel data for Aerogel is disabled",StatusCode::SUCCESS);
  if ( !m_usedRads[Rich::C4F10]   ) Warning("Pixel data for C4F10 is disabled",StatusCode::SUCCESS);
  if ( !m_usedRads[Rich::CF4]     ) Warning("Pixel data for CF4 is disabled",StatusCode::SUCCESS);

  return sc;
}

StatusCode RichPixelCreatorFromMCRichHits::finalize()
{
  // Execute base class method
  return RichPixelCreatorBase::finalize();
}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel *
RichPixelCreatorFromMCRichHits::newPixel( const ContainedObject * obj ) const 
{

  // Try to cast to MCRichHit
  const MCRichHit * hit = dynamic_cast<const MCRichHit*>(obj);
  if ( !hit ) {
    Warning("Parent not of type MCRichHit");
    return NULL;
  }

  // hit key
  //const long int hitKey = hit->key();
  // See if this RichRecPixel already exists
  //if ( bookKeep() && m_pixelDone[hitKey] )
  //{
  //  return m_pixelExists[hitKey];
  //}
  //else
  //{

    RichRecPixel * newPixel = NULL;

    // Check if we are using this radiator
    if ( m_usedRads[hit->radiator()] ) {

      RichSmartID id(0);
      const StatusCode sc = m_smartIDTool->smartID( hit->entry(), id );
      if ( sc.isSuccess() && pixelIsOK(id) )
      {

        // Find associated MCRichOpticalPhoton
        const MCRichOpticalPhoton * mcPhot = m_mcTool->mcOpticalPhoton(hit);
        if ( mcPhot ) {

          // Make a new RichRecPixel
          newPixel = new RichRecPixel();

          // Positions
          newPixel->setGlobalPosition( mcPhot->pdIncidencePoint() );
          newPixel->setLocalPosition( m_smartIDTool->globalToPDPanel(newPixel->globalPosition()) );
          // compute corrected local coordinates
          computeRadCorrLocalPositions( newPixel );

          // Set smartID
          newPixel->setSmartID( id );

          // Set parent information
          newPixel->setParentPixel( hit );
          newPixel->setParentType( Rich::PixelParent::MCHit );

          // save this pixel
          savePixel( newPixel );

        }

      }

    }

    // Add to reference map
    //if ( bookKeep() )
    // {
    //  m_pixelExists[hitKey] = newPixel;
    //  m_pixelDone[hitKey] = true;
    // }

    return newPixel;
    //}

}

StatusCode RichPixelCreatorFromMCRichHits::newPixels() const {

  if ( !m_allDone )
  {
    m_allDone = true;

    // Obtain smart data pointer to RichDigits
    const MCRichHits * hits = get<MCRichHits>( m_mcHitsLocation );

    // Reserve space
    richPixels()->reserve( hits->size() );

    // Loop over hits and create working pixels
    for ( MCRichHits::const_iterator hit = hits->begin();
          hit != hits->end();
          ++hit )
    {
      // Make a Pixel for this RichSmartID
      newPixel( *hit );
    }

    // sort the pixels - Have to do this manually in this case.
    sortPixels();

    // find iterators
    fillIterators();

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Located " << hits->size() << " MCRichHits at "
              << m_mcHitsLocation << endreq
              << "Created " << richPixels()->size() << " RichRecPixels at "
              << pixelLocation() << endreq;
    }

  }

  return StatusCode::SUCCESS;
}
