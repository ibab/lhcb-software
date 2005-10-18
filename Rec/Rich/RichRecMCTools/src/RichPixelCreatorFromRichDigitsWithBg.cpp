
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromRichDigitsWithBg.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPixelCreatorFromRichDigitsWithBg
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRichDigitsWithBg.cpp,v 1.13 2005-10-18 12:46:37 jonrob Exp $
 *
 *  @author Andy Buckley  buckley@hep.phy.cam.ac.uk
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/10/2004
 */
//-----------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromRichDigitsWithBg.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromRichDigitsWithBg>          s_factory ;
const        IToolFactory& RichPixelCreatorFromRichDigitsWithBgFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromRichDigitsWithBg::
RichPixelCreatorFromRichDigitsWithBg( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichPixelCreatorBase ( type, name, parent ),
    m_smartIDTool        ( 0 ),
    m_numBgTracksToAdd   ( Rich::NRiches, 0 )
{
  // Define job option parameters
  declareProperty( "RecoDigitsLocation", m_recoDigitsLocation = RichDigitLocation::Default );
  declareProperty( "NumberBackgroundTracksToAdd",  m_numBgTracksToAdd );
}

// Initializer
StatusCode RichPixelCreatorFromRichDigitsWithBg::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPixelCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDTool", m_smartIDTool, 0, true );
  acquireTool( "RichMCTruthTool", m_mcTool, 0, true      );

  // warn that this background adding creator is being used
  info() << "Using background adding pixel creator : Will add " << m_numBgTracksToAdd[Rich::Rich1]
         << "/" << m_numBgTracksToAdd[Rich::Rich2] << " traversing particles to RICH(1/2)" << endreq;

  return sc;
}

StatusCode RichPixelCreatorFromRichDigitsWithBg::finalize()
{
  // Execute base class method
  return RichPixelCreatorBase::finalize();
}

// Fill the stack of background tracks from the current event
// (effectively emptying this event)
StatusCode
RichPixelCreatorFromRichDigitsWithBg::fillBgTrackStack() const
{
  // Obtain smart data pointer to RichDigits
  RichDigits * digits = get<RichDigits>( m_recoDigitsLocation );
  RichMap<Rich::DetectorType, unsigned int> originalNumTracksInStack;
  originalNumTracksInStack[Rich::Rich1] = m_digitsForTrackBg[Rich::Rich1].size();
  originalNumTracksInStack[Rich::Rich2] = m_digitsForTrackBg[Rich::Rich2].size();
  RichMap<Rich::DetectorType, unsigned int> numDigitsAddedToStack;

  // Loop over RichDigits and create working pixels
  for ( RichDigits::const_iterator digit = digits->begin(); digit != digits->end(); ++digit ) {
    const Rich::DetectorType whichRich( (*digit)->key().rich() );

    // Make association to the MCRichDigit via IRichMCTruthTool tool:
    const MCRichDigit* mcDigit( m_mcTool->mcRichDigit(*digit) );
    if (mcDigit) {

      // Get MC hits from the MC digit (can be >1 photon producing one digit in general)
      if ( !mcDigit->hits().empty() ) {

        // For each hit, retrieve the MCParticle and add it to the stack if valid
        for (SmartRefVector<MCRichHit>::const_iterator mcHit = mcDigit->hits().begin();
             mcHit != mcDigit->hits().end(); ++mcHit) {
          const MCParticle* mcParticle( (*mcHit)->mcParticle() );
          if (mcParticle) {
            if ( m_numBgTracksToAdd[whichRich] > 0 ) {
              m_digitsForTrackBg[whichRich][mcParticle].push_back( (*digit)->key() );
              ++numDigitsAddedToStack[whichRich];
            }
          }
        }

      }
    }
  }

  // Empty the current event's digit collection
  digits->clear();

  // Work out how many tracks were added to the stack and tell the world about it
  if ( msgLevel(MSG::INFO) )
  {
    RichMap<Rich::DetectorType, unsigned int> numTracksAddedToStack;
    numTracksAddedToStack[Rich::Rich1] =
      m_digitsForTrackBg[Rich::Rich1].size() - originalNumTracksInStack[Rich::Rich1];
    numTracksAddedToStack[Rich::Rich2] =
      m_digitsForTrackBg[Rich::Rich2].size() - originalNumTracksInStack[Rich::Rich2];
    if (m_numBgTracksToAdd[Rich::Rich1] > 0) {
      info() << "Added "
             << numTracksAddedToStack[Rich::Rich1]  << " particles ("
             << numDigitsAddedToStack[Rich::Rich1] << " digits) to the RICH1 stack ("
             << originalNumTracksInStack[Rich::Rich1] << "->" << m_digitsForTrackBg[Rich::Rich1].size() << ")"
             << endreq;
    }
    if (m_numBgTracksToAdd[Rich::Rich2] > 0) {
      info() << "Added "
             << numTracksAddedToStack[Rich::Rich2]  << " particles ("
             << numDigitsAddedToStack[Rich::Rich2] << " digits) to the RICH2 stack ("
             << originalNumTracksInStack[Rich::Rich2] << "->" << m_digitsForTrackBg[Rich::Rich2].size() << ")"
             << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

// Forms a new RichRecPixel object from a RichDigit
RichRecPixel *
RichPixelCreatorFromRichDigitsWithBg::newPixel( const ContainedObject * obj ) const
{
  // Try to cast to RichDigit
  const RichDigit * digit = dynamic_cast<const RichDigit*>(obj);
  if ( !digit )
  {
    Warning("Parent not of type RichDigit");
    return NULL;
  }

  // Get the pixel via the smart ID
  RichRecPixel * newPixel( newPixel( digit->key() ) );

  // Set parent information
  if ( newPixel )
  {
    newPixel->setParentPixel( digit );
    newPixel->setParentType( Rich::PixelParent::Digit );
  }

  return newPixel;
}

RichRecPixel *
RichPixelCreatorFromRichDigitsWithBg::newPixel( const RichSmartID id ) const
{

  // See if this RichRecPixel already exists
  if ( bookKeep() && m_pixelDone[id] )
  {
    return m_pixelExists[id];
  }
  else
  {

    RichRecPixel* newPixel(0);

    // Check this hit is OK
    if ( pixelIsOK(id) )
    {

      // Make a new RichRecPixel
      newPixel = new RichRecPixel();

      // Positions
      newPixel->globalPosition() = m_smartIDTool->globalPosition( id );
      newPixel->localPosition()  = m_smartIDTool->globalToPDPanel(newPixel->globalPosition());
      // compute corrected local coordinates
      computeRadCorrLocalPositions( newPixel );

      // Set smartID
      newPixel->setSmartID( id );

      // save this pixel
      savePixel( newPixel );

    }

    // Add to reference map
    if ( bookKeep() )
    {
      m_pixelExists[ id ] = newPixel;
      m_pixelDone  [ id ] = true;
    }

    return newPixel;
  }
}


StatusCode RichPixelCreatorFromRichDigitsWithBg::newPixels() const
{
  if ( !m_allDone )
  {
    m_allDone = true;

    // Do we need to fill the background stacks ?
    if ( m_numBgTracksToAdd[Rich::Rich1] > m_digitsForTrackBg[Rich::Rich1].size() ||
         m_numBgTracksToAdd[Rich::Rich2] > m_digitsForTrackBg[Rich::Rich2].size() )
    {
      debug() << "Not enough background tracks in the stack ( R1="
              << m_digitsForTrackBg[Rich::Rich1].size() + m_digitsForTrackBg[Rich::Rich2].size()
              << " R2=" << m_numBgTracksToAdd[Rich::Rich1] + m_numBgTracksToAdd[Rich::Rich2] << " ). "
              << "Refilling from this event" << endreq;

      // fill the background data stack
      fillBgTrackStack();

      // Reset iterators to defaults
      resetIterators();

      return StatusCode::SUCCESS; // no digits to run on
    }

    // Obtain smart data pointer to RichDigits
    const RichDigits * digits = get<RichDigits>( m_recoDigitsLocation );

    // Loop over RichDigits and create working pixels
    richPixels()->reserve( digits->size() );
    for ( RichDigits::const_iterator digit = digits->begin();
          digit != digits->end(); ++digit ) { newPixel( *digit ); }

    // Do the same thing for the bg track digits
    RichMap<Rich::DetectorType, size_t> numBgTracksAdded;
    size_t numBgPixelsAdded(0);
    // RICH1
    while ( numBgTracksAdded[Rich::Rich1] < m_numBgTracksToAdd[Rich::Rich1] )
    {

      // Add hits for this particle
      std::vector<RichSmartID> & bgdigits = m_digitsForTrackBg[Rich::Rich1].begin()->second;
      for ( std::vector<RichSmartID>::const_iterator digit = bgdigits.begin();
            digit != bgdigits.end(); ++digit )
      {
        RichRecPixel * newPix = newPixel( *digit );
        newPix->setParentType( Rich::PixelParent::NoParent );
        ++numBgPixelsAdded;
      }

      // count number of added particles
      ++numBgTracksAdded[Rich::Rich1];

      // Remove this particle's digits from the stack
      m_digitsForTrackBg[Rich::Rich1].erase(m_digitsForTrackBg[Rich::Rich1].begin());
    }
    // RICH2
    while ( numBgTracksAdded[Rich::Rich2] < m_numBgTracksToAdd[Rich::Rich2] )
    {

      // Add hits for this particle
      std::vector<RichSmartID> & bgdigits = m_digitsForTrackBg[Rich::Rich2].begin()->second;
      for ( std::vector<RichSmartID>::const_iterator digit = bgdigits.begin();
            digit != bgdigits.end(); ++digit )
      {
        RichRecPixel * newPix = newPixel( *digit );
        newPix->setParentType( Rich::PixelParent::NoParent );
        ++numBgPixelsAdded;
      }

      // count number of added particles
      ++numBgTracksAdded[Rich::Rich2];

      // Remove this particle's digits from the stack
      m_digitsForTrackBg[Rich::Rich2].erase(m_digitsForTrackBg[Rich::Rich2].begin());

    }

    // Sort the pixels. Have to do this in this case since adding the additional
    // hits corrupts the automatic sorting of the main event RichDigits
    sortPixels();

    // find iterators
    fillIterators();

    // Report
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Located " << digits->size() << " RichDigits at "
              << m_recoDigitsLocation << endreq
              << "Added " << numBgTracksAdded << " additional background tracks with "
              << numBgPixelsAdded << " pixels" << endreq
              << "Created " << richPixels()->size() << " RichRecPixels at "
              << pixelLocation() << endreq;
    }

  }

  return StatusCode::SUCCESS;
}
