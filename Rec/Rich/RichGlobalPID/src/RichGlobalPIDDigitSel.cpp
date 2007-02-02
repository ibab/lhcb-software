
//--------------------------------------------------------------------------
/** @file RichGlobalPIDDigitSel.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::DigitSel
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDDigitSel.cpp,v 1.20 2007-02-02 10:03:58 jonrob Exp $
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
DECLARE_ALGORITHM_FACTORY( DigitSel );

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
  // during first processed event and thus biased any timing numbers
  pixelCreator();  // pre-load the pixel creator

  return sc;
}

// Initialise pixels
StatusCode DigitSel::execute()
{
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Check if track processing was aborted.
  LHCb::ProcStatus * procStat = get<LHCb::ProcStatus>( m_procStatLocation );
  if ( procStat->aborted() ) 
  {
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::ProcStatAbort );
    richStatus()->setEventOK( false );
    return Warning( "Processing aborted -> Abort", StatusCode::SUCCESS );
  }

  // Create all RichRecPixels
  if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;

  // check the number of pixels
  if ( richPixels()->empty() ) 
  { // empty event ?

    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::NoRichPixels );
    richStatus()->setEventOK( false );
    return Warning( "Event contains no pixels -> Abort", StatusCode::SUCCESS );

  } 
  else if ( m_maxUsedPixels < richPixels()->size() ) 
  { // too many pixels

    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::ReachedPixelLimit );
    richStatus()->setEventOK( false );
    return Warning( "Max. number of pixels exceeded -> Abort", StatusCode::SUCCESS );

  }

  // final printout of selected number of pixels
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Selected " << richPixels()->size() << " RichRecPixels" << endreq;

  return StatusCode::SUCCESS;
}
