// $Id: RichGlobalPIDDigitSel.cpp,v 1.11 2004-07-12 14:51:48 jonrob Exp $
// Include files

// local
#include "RichGlobalPIDDigitSel.h"

//--------------------------------------------------------------------------
// Implementation file for class : RichGlobalPIDDigitSel
//
// 017/04/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
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
    return Warning("Processing aborted -> Abort",StatusCode::SUCCESS);
  }

  // Create all RichRecPixels
  if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;
  debug() << "Selected " << richPixels()->size() << " RichDigits" << endreq;

  // How many pixels ?
  if ( m_maxUsedPixels < richPixels()->size() ) {
    procStat->addAlgorithmStatus(m_richGPIDName,Rich::Rec::ReachedPixelLimit);
    richStatus()->setEventOK( false );
    return Warning("Max. number of pixels exceeded -> Abort",StatusCode::SUCCESS);
  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichGlobalPIDDigitSel::finalize() 
{
  // Execute base class method
  return RichRecAlgBase::finalize();
}
