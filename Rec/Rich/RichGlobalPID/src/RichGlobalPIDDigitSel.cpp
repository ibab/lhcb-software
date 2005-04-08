
//--------------------------------------------------------------------------
/** @file RichGlobalPIDDigitSel.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : RichGlobalPIDDigitSel
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDDigitSel.cpp,v 1.15 2005-04-08 13:16:45 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichGlobalPIDDigitSel.h"

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichGlobalPIDDigitSel>          s_factory ;
const        IAlgFactory& RichGlobalPIDDigitSelFactory = s_factory ;

// Standard constructor, initializes variables
RichGlobalPIDDigitSel::RichGlobalPIDDigitSel( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : RichGlobalPIDAlgBase ( name, pSvcLocator ) {

  // Job options
  declareProperty( "MaxUsedPixels", m_maxUsedPixels = 8000 );
  declareProperty( "ProcStatusLocation",
                   m_procStatLocation = ProcStatusLocation::Default );

}

// Destructor
RichGlobalPIDDigitSel::~RichGlobalPIDDigitSel() {}

//  Initialize
StatusCode RichGlobalPIDDigitSel::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
}

// Initialise pixels
StatusCode RichGlobalPIDDigitSel::execute() {

  debug() << "Execute" << endreq;

  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Check if track processing was aborted.
  ProcStatus * procStat = get<ProcStatus>( m_procStatLocation );
  if ( procStat->aborted() ) {
    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::ProcStatAbort );
    richStatus()->setEventOK( false );
    return Warning( "Processing aborted -> Abort", StatusCode::SUCCESS );
  }

  // Create all RichRecPixels
  if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;

  // check the number of pixels
  if ( richPixels()->empty() ) { // empty event ?

    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::NoRichPixels );
    richStatus()->setEventOK( false );
    return Warning( "Event contains no pixels -> Abort", StatusCode::SUCCESS );

  } else if ( m_maxUsedPixels < richPixels()->size() ) { // too many pixels

    procStat->addAlgorithmStatus( m_richGPIDName, Rich::Rec::ReachedPixelLimit );
    richStatus()->setEventOK( false );
    return Warning( "Max. number of pixels exceeded -> Abort", StatusCode::SUCCESS );

  }

  // final printout of selected number of pixels
  debug() << "Selected " << richPixels()->size() << " RichRecPixels" << endreq;

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichGlobalPIDDigitSel::finalize()
{
  // Execute base class method
  return RichRecAlgBase::finalize();
}
