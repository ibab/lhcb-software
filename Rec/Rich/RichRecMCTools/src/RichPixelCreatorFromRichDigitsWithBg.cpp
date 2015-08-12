
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromRichDigitsWithBg.cpp
 *
 *  Implementation file for RICH reconstruction tool : Rich::Rec::PixelCreatorFromRichDigitsWithBg
 *
 *  @author Andy Buckley  buckley@hep.phy.cam.ac.uk
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/10/2004
 */
//-----------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromRichDigitsWithBg.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PixelCreatorFromRichDigitsWithBg )

// Standard constructor
PixelCreatorFromRichDigitsWithBg::
PixelCreatorFromRichDigitsWithBg( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : Rich::Rec::PixelCreatorBase ( type, name, parent ),
    m_numBgTracksToAdd ( Rich::NRiches, 0 )
{
  // Define job option parameters
  declareProperty( "RecoDigitsLocation", m_recoDigitsLocation = LHCb::RichDigitLocation::Default );
  declareProperty( "NumberBackgroundTracksToAdd",  m_numBgTracksToAdd );
}

// Initializer
StatusCode PixelCreatorFromRichDigitsWithBg::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::PixelCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichMCTruthTool", m_mcTool, NULL, true );

  // warn that this background adding creator is being used
  info() << "Using background adding pixel creator : Will add " << m_numBgTracksToAdd[Rich::Rich1]
         << "/" << m_numBgTracksToAdd[Rich::Rich2] << " traversing particles to RICH(1/2)" << endmsg;

  return sc;
}

// Fill the stack of background tracks from the current event
// (effectively emptying this event)
StatusCode
PixelCreatorFromRichDigitsWithBg::fillBgTrackStack() const
{
  // Obtain smart data pointer to RichDigits
  LHCb::RichDigits * digits = get<LHCb::RichDigits>( m_recoDigitsLocation );
  Rich::Map<Rich::DetectorType, unsigned int> originalNumTracksInStack;
  originalNumTracksInStack[Rich::Rich1] = m_digitsForTrackBg[Rich::Rich1].size();
  originalNumTracksInStack[Rich::Rich2] = m_digitsForTrackBg[Rich::Rich2].size();
  Rich::Map<Rich::DetectorType, unsigned int> numDigitsAddedToStack;

  // Loop over RichDigits and create working pixels
  for ( LHCb::RichDigits::const_iterator digit = digits->begin(); digit != digits->end(); ++digit ) {
    const Rich::DetectorType whichRich( (*digit)->key().rich() );

    // Make association to the MCRichDigit via IRichMCTruthTool tool:
    const LHCb::MCRichDigit* mcDigit( m_mcTool->mcRichDigit( (*digit)->richSmartID() ) );
    if (mcDigit) {

      // Get MC hits from the MC digit (can be >1 photon producing one digit in general)
      if ( !mcDigit->hits().empty() ) {

        // For each hit, retrieve the MCParticle and add it to the stack if valid
        for ( LHCb::MCRichDigitHit::Vector::const_iterator mcHit = mcDigit->hits().begin();
              mcHit != mcDigit->hits().end(); ++mcHit) {
          const LHCb::MCParticle* mcParticle( (*mcHit).mcRichHit()->mcParticle() );
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
    Rich::Map<Rich::DetectorType, unsigned int> numTracksAddedToStack;
    numTracksAddedToStack[Rich::Rich1] =
      m_digitsForTrackBg[Rich::Rich1].size() - originalNumTracksInStack[Rich::Rich1];
    numTracksAddedToStack[Rich::Rich2] =
      m_digitsForTrackBg[Rich::Rich2].size() - originalNumTracksInStack[Rich::Rich2];
    if (m_numBgTracksToAdd[Rich::Rich1] > 0) {
      info() << "Added "
             << numTracksAddedToStack[Rich::Rich1]  << " particles ("
             << numDigitsAddedToStack[Rich::Rich1] << " digits) to the RICH1 stack ("
             << originalNumTracksInStack[Rich::Rich1] << "->" << m_digitsForTrackBg[Rich::Rich1].size() << ")"
             << endmsg;
    }
    if (m_numBgTracksToAdd[Rich::Rich2] > 0) {
      info() << "Added "
             << numTracksAddedToStack[Rich::Rich2]  << " particles ("
             << numDigitsAddedToStack[Rich::Rich2] << " digits) to the RICH2 stack ("
             << originalNumTracksInStack[Rich::Rich2] << "->" << m_digitsForTrackBg[Rich::Rich2].size() << ")"
             << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

// Forms a new RichRecPixel object from a RichDigit
LHCb::RichRecPixel *
PixelCreatorFromRichDigitsWithBg::newPixel( const ContainedObject * obj ) const
{
  // Try to cast to RichDigit
  const LHCb::RichDigit * digit = dynamic_cast<const LHCb::RichDigit*>(obj);
  if ( !digit )
  {
    Warning("Parent not of type RichDigit");
    return NULL;
  }

  // Get the pixel via the smart ID
  LHCb::RichRecPixel * newPix( newPixel( digit->key() ) );

  // Set parent information
  if ( newPix )
  {
    newPix->setParentPixel( digit );
    newPix->setParentType( Rich::Rec::PixelParent::Digit );
  }

  return newPix;
}

LHCb::RichRecPixel *
PixelCreatorFromRichDigitsWithBg::newPixel( const LHCb::RichSmartID id ) const
{

  // See if this RichRecPixel already exists
  if ( bookKeep() && m_pixelDone[id] )
  {
    return m_pixelExists[id];
  }
  else
  {

    LHCb::RichRecPixel* newPix(0);

    // Check this hit is OK
    if ( hpdIsOK(id) && pixelIsOK(id) )
    {

      // Make a new RichRecPixel
      Gaudi::XYZPoint gPos;
      const StatusCode sc = smartIDTool()->globalPosition( id, gPos );
      if ( sc.isSuccess() )
      {

        newPix = new LHCb::RichRecPixel( id, gPos,
                                         smartIDTool()->globalToPDPanel(gPos),
                                         Rich::Rec::PixelParent::RawBuffer, NULL
                                         );

        // save this pixel
        savePixel( newPix );

      }

    }

    // Add to reference map
    if ( bookKeep() )
    {
      m_pixelExists[ id ] = newPix;
      m_pixelDone  [ id ] = true;
    }

    return newPix;
  }
}


StatusCode PixelCreatorFromRichDigitsWithBg::newPixels() const
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
              << "Refilling from this event" << endmsg;

      // fill the background data stack
      fillBgTrackStack();

      // Reset iterators to defaults
      resetIterators();

      return StatusCode::SUCCESS; // no digits to run on
    }

    // Obtain smart data pointer to RichDigits
    const LHCb::RichDigits * digits = get<LHCb::RichDigits>( m_recoDigitsLocation );

    // Loop over RichDigits and create working pixels
    richPixels()->reserve( digits->size() );
    for ( LHCb::RichDigits::const_iterator digit = digits->begin();
          digit != digits->end(); ++digit ) { newPixel( *digit ); }

    // Do the same thing for the bg track digits
    Rich::Map<Rich::DetectorType, unsigned int> numBgTracksAdded;
    unsigned int numBgPixelsAdded(0);
    // RICH1
    while ( numBgTracksAdded[Rich::Rich1] < m_numBgTracksToAdd[Rich::Rich1] )
    {

      // Add hits for this particle
      LHCb::RichSmartID::Vector & bgdigits = m_digitsForTrackBg[Rich::Rich1].begin()->second;
      for ( LHCb::RichSmartID::Vector::const_iterator digit = bgdigits.begin();
            digit != bgdigits.end(); ++digit )
      {
        LHCb::RichRecPixel * newPix = newPixel( *digit );
        newPix->setParentType( Rich::Rec::PixelParent::NoParent );
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
      LHCb::RichSmartID::Vector & bgdigits = m_digitsForTrackBg[Rich::Rich2].begin()->second;
      for ( LHCb::RichSmartID::Vector::const_iterator digit = bgdigits.begin();
            digit != bgdigits.end(); ++digit )
      {
        LHCb::RichRecPixel * newPix = newPixel( *digit );
        newPix->setParentType( Rich::Rec::PixelParent::NoParent );
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
              << m_recoDigitsLocation << endmsg
              << "Added " << numBgTracksAdded
              << " additional background tracks with "
              << numBgPixelsAdded << " pixels" << endmsg
              << "Created " << richPixels()->size() << " RichRecPixels at "
              << pixelLocation() << endmsg;
    }

  }

  return StatusCode::SUCCESS;
}
