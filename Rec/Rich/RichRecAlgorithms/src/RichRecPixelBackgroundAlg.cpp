
//--------------------------------------------------------------------------
/** @file RichRecPixelBackgroundAlg.cpp
 *
 *  Implementation file for algorithm class : RichRecInit
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichRecPixelBackgroundAlg.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PixelBackgroundAlg )

// Standard constructor, initializes variables
PixelBackgroundAlg::PixelBackgroundAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator ),
    m_bkgTool          ( NULL              ) { }

// Destructor
PixelBackgroundAlg::~PixelBackgroundAlg() {}

StatusCode PixelBackgroundAlg::initialize()
{
  const StatusCode sc = Rich::Rec::AlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // load tools
  acquireTool( "PixelBackgroundTool", m_bkgTool, this );

  // pre-cache tools
  statusCreator();
  pixelCreator();
  segmentCreator();

  return sc;
}

StatusCode PixelBackgroundAlg::execute()
{
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Check segments and pixels
  if ( !richSegments() ) return Error( "Failed to access RichRecSegments" );
  if ( !richPixels()   ) return Error( "Failed to access RichRecPixels"   );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Found " << richSegments()->size() << " RichRecSegments" << endmsg
            << "Found " << richPixels()->size() << " RichRecPixels" << endmsg;
  }

  // compute backgrounds
  m_bkgTool->computeBackgrounds();

  // return
  return StatusCode::SUCCESS;
}
