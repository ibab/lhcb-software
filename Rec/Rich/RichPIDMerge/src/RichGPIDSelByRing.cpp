
//------------------------------------------------------------------------
/** @file RichGPIDSelByRing.cpp
 *
 *  Implementation file for RICH algorithm : RichGPIDSelByRing
 *
 *  CVS Log :-
 *  $Id: RichGPIDSelByRing.cpp,v 1.1 2004-08-19 14:04:35 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-10
 */
//------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichGPIDSelByRing.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichGPIDSelByRing>          s_factory ;
const        IAlgFactory& RichGPIDSelByRingFactory = s_factory ;


// Standard constructor, initializes variables
RichGPIDSelByRing::RichGPIDSelByRing( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : RichAlgBase ( name , pSvcLocator )
{

  // Output location in TDS for RichPIDs
  declareProperty( "OutputPIDLocation",
                   m_richPIDLocation = RichPIDLocation::Default );
  // Input location in TDS for RichGlobalPIDs
  declareProperty( "InputGlobalPIDLocation",
                   m_richGlobalPIDLocation = RichGlobalPIDLocation::Default );
  // Input location in TDS for RichRingRefitPIDs
  declareProperty( "InputRingRefitPIDLocation",
                   m_richRingFitPIDLocation = RichRingRefitPIDLocation::Default );
  // Location of processing status object in TES
  declareProperty( "ProcStatusLocation",
                   m_procStatLocation = ProcStatusLocation::Default );

}

// Destructor
RichGPIDSelByRing::~RichGPIDSelByRing() {};

// Initialisation.
StatusCode RichGPIDSelByRing::initialize()
{
  // initialise base classclean
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Add any customisations here

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode RichGPIDSelByRing::execute()
{
  debug() << "Execute" << endreq;

  // Locate the processing status object
  ProcStatus * procStat = get<ProcStatus>( m_procStatLocation );
  if ( procStat->aborted() ) {
    Warning("Processing aborted -> No RICH PID results",StatusCode::SUCCESS);
    return StatusCode::SUCCESS;
  }

  // See if PIDs already exist at requested output location
  RichPIDs * newPIDs = NULL;
  bool pidsExist = false;
  unsigned int originalSize = 0;
  SmartDataPtr<RichPIDs> outPIDs( eventSvc(), m_richPIDLocation );
  if ( outPIDs ) {
    // Use existing container, emptied of previous results
    pidsExist = true;
    newPIDs = outPIDs;
    originalSize = newPIDs->size();
    newPIDs->clear();
  } else {
    // Form new container for output PIDs
    newPIDs = new RichPIDs();
    put( newPIDs, m_richPIDLocation );
  }

  unsigned int nUsedglobalPIDs = 0;

  // Load the PIDs
  RichGlobalPIDs * gPIDs    = get<RichGlobalPIDs>    ( m_richGlobalPIDLocation  );
  RichRingRefitPIDs * rPIDs = get<RichRingRefitPIDs> ( m_richRingFitPIDLocation );
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Located " << gPIDs->size() << " GlobalPIDs and " 
            << rPIDs->size() << " RingRefitPIDs" << endreq;
  }

  // iterate over Global PID results and form output persistent objects
  for ( RichGlobalPIDs::const_iterator gPID = gPIDs->begin();
        gPID != gPIDs->end(); ++gPID ) {

    // Try and locate the Ring PID result for the same track
    // if missing, skip this PID result
    if ( !rPIDs->object( (*gPID)->key() ) ) continue;

    // Form new PID object, using existing RichPID as template
    newPIDs->insert( new RichPID(*gPID), (*gPID)->key() );
    ++nUsedglobalPIDs;

  }

  // Update Rich status words
  procStat->addAlgorithmStatus( name()+":UsedGlobalPIDs",    nUsedglobalPIDs );
  procStat->addAlgorithmStatus( name()+":UsedLocalPIDs",     0 );
  procStat->addAlgorithmStatus( name()+":UsedRingRefitPIDs", 0 );

  // Final debug information
  if ( msgLevel(MSG::DEBUG) ) {
    if ( !pidsExist ) {
      debug() << "Successfully registered " << newPIDs->size()
              << " RichPIDs at " << m_richPIDLocation << endreq;
    } else {
      debug() << "Replaced " << originalSize << " pre-existing RichPIDs at "
              << m_richPIDLocation << " with " << newPIDs->size()
              << " new RichPIDs" << endreq;
    }
  }

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichGPIDSelByRing::finalize()
{
  // base class finalise
  return RichAlgBase::finalize();
}
