
//--------------------------------------------------------------------------
/** @file RichGlobalPIDDigitSel.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::DigitSel
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichGlobalPIDDigitSel.h"

// namespaces
using namespace Rich::Rec::GlobalPID;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DigitSel )

// Standard constructor, initializes variables
DigitSel::DigitSel( const std::string& name,
                    ISvcLocator* pSvcLocator )
  : AlgBase ( name, pSvcLocator )
{
  // Job options
  declareProperty( "MaxUsedPixels", m_maxUsedPixels = 8000 );
}

// Destructor
DigitSel::~DigitSel() {}

//  Initialize
StatusCode DigitSel::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // trick to force pre-loading of various tools. Avoids loading
  // during first processed event and thus biasing any timing numbers
  pixelCreator();  // pre-load the pixel creator

  return sc;
}

// Initialise pixels
StatusCode DigitSel::execute()
{
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Check if track processing was aborted.
  if ( procStatus()->aborted() ) 
  {
    richStatus()->setEventOK( false );
    deleteGPIDEvent();
    return Warning( "Processing aborted -> Abort", StatusCode::SUCCESS, 0 );
  }

  // Create all RichRecPixels
  if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;

  // check the number of pixels
  if ( richPixels()->empty() ) 
  { // empty event ?
    procStatus()->addAlgorithmStatus( gpidName(), "RICH", "NoRichPixels",
                                      Rich::Rec::NoRichPixels, false );
    richStatus()->setEventOK( false );
    return Warning( "Event contains no pixels -> Abort", StatusCode::SUCCESS, 0 );
  }
  else if ( m_maxUsedPixels < (int)richPixels()->size() ) 
  { // too many pixels
    procStatus()->addAlgorithmStatus( gpidName(), "RICH", "ReachedPixelLimit",
                                      Rich::Rec::ReachedPixelLimit, true );
    richStatus()->setEventOK( false );
    std::ostringstream mess;
    mess << "Number of selected pixels exceeds maximum of " << m_maxUsedPixels << " -> Abort";
    return Warning( mess.str(), StatusCode::SUCCESS, 0 );
  }

  // final printout of selected number of pixels
  _ri_debug << "Selected " << richPixels()->size() << " RichRecPixels" << endmsg;

  return StatusCode::SUCCESS;
}
