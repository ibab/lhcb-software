// $Id: RichPIDSimpleMerge.cpp,v 1.6 2004-07-12 14:52:49 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichPIDSimpleMerge.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPIDSimpleMerge
//
// 2002-07-10 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichPIDSimpleMerge>          s_factory ;
const        IAlgFactory& RichPIDSimpleMergeFactory = s_factory ;


// Standard constructor, initializes variables
RichPIDSimpleMerge::RichPIDSimpleMerge( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : RichAlgBase ( name , pSvcLocator ) {

  // Output location in TDS for RichPIDs
  declareProperty( "OutputPIDLocation",
                   m_richPIDLocation = RichPIDLocation::Default );
  // Input location in TDS for RichGlobalPIDs
  declareProperty( "InputGlobalPIDLocation",
                   m_richGlobalPIDLocation = RichGlobalPIDLocation::Default );
  // Input location in TDS for RichLocalPIDs
  declareProperty( "InputLocalPIDLocation",
                   m_richLocalPIDLocation = RichLocalPIDLocation::Default );
  // Location of processing status object in TES
  declareProperty( "ProcStatusLocation",
                   m_procStatLocation = ProcStatusLocation::Default );

}

// Destructor
RichPIDSimpleMerge::~RichPIDSimpleMerge() {};

// Initialisation.
StatusCode RichPIDSimpleMerge::initialize() 
{
  // initialise base classclean
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Add any customisations here

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode RichPIDSimpleMerge::execute()
{

  //
  // For the moment take global then local result in that order of preference
  // In the future need some kind of proper merging of results
  //

  debug() << "Execute" << endreq;

  // Obtain Global PID results
  SmartDataPtr<RichGlobalPIDs> gPIDs( eventSvc(), m_richGlobalPIDLocation );
  if ( msgLevel(MSG::VERBOSE) ) {
    if ( !gPIDs ) {
      verbose() << "Cannot locate RichGlobalPIDs at "
                << m_richGlobalPIDLocation << endreq;
    } else {
      verbose() << "Successfully located " << gPIDs->size()
                << " RichGlobalPIDs at " << m_richGlobalPIDLocation << endreq;
    }
  }

  // Obtain Local PID results
  SmartDataPtr<RichLocalPIDs> lPIDs(eventSvc(), m_richLocalPIDLocation);
  if ( msgLevel(MSG::VERBOSE) ) {
    if ( !lPIDs ) {
      verbose() << "Cannot locate RichLocalPIDs at "
                << m_richLocalPIDLocation << endreq;
    } else {
      verbose()  << "Successfully located " << lPIDs->size()
                 << " RichLocalPIDs at " << m_richLocalPIDLocation << endreq;
    }
  }

  // See if PIDs already exist at requested output location
  RichPIDs * newPIDs = NULL;
  bool pidsExist = false;
  unsigned int originalSize = 0;
  SmartDataPtr<RichPIDs> outPIDs( eventSvc(), m_richPIDLocation );
  if ( outPIDs ) {
    pidsExist = true;
    newPIDs = outPIDs;
    originalSize = newPIDs->size();
    newPIDs->clear();
  } else {
    // Form new container for output PIDs
    newPIDs = new RichPIDs();
    put( newPIDs, m_richPIDLocation );
  }

  // Locate the processing status object
  ProcStatus * procStat = get<ProcStatus>( m_procStatLocation );

  unsigned int nUsedglobalPIDs = 0;
  unsigned int nUsedlocalPIDs  = 0;
  if ( !procStat->aborted() ) {

    // iterate over Global PID results and form output persistent objects
    if ( gPIDs ) {
      for ( RichGlobalPIDs::const_iterator gPID = gPIDs->begin();
            gPID != gPIDs->end(); ++gPID ) {

        // Form new PID object, using existing RichPID as template
        RichPID * outPID = new RichPID( *gPID );
        newPIDs->insert( outPID, (*gPID)->key() );
        ++nUsedglobalPIDs;

      }
    }

    // iterate over Local PID results and place in output container if
    // global PID result not present
    if ( lPIDs ) {
      for ( RichLocalPIDs::const_iterator lPID = lPIDs->begin();
            lPID != lPIDs->end();
            ++lPID ) {

        // if pid with this key exists, skip
        if ( newPIDs->object( (*lPID)->key() ) ) continue;

        // Form new PID object, using existing RichPID as template
        RichPID * outPID = new RichPID( *lPID );
        newPIDs->insert( outPID, (*lPID)->key() );
        ++nUsedlocalPIDs;

      }
    }

  }

  // Update Rich status words
  procStat->addAlgorithmStatus( name()+":UsedGlobalPIDs", nUsedglobalPIDs );
  procStat->addAlgorithmStatus( name()+":UsedLocalPIDs",  nUsedlocalPIDs  );

  // Final debug information
  if ( msgLevel(MSG::DEBUG) ) {
    if ( !pidsExist ) {
      debug() << "Successfully registered " << newPIDs->size()
              << " RichPIDs at " << m_richPIDLocation
              << " : Global=" << nUsedglobalPIDs
              << " Local=" << nUsedlocalPIDs
              << endreq;
    } else {
      debug() << "Replaced " << originalSize << " pre-existing RichPIDs at "
              << m_richPIDLocation << " with " << newPIDs->size()
              << " new RichPIDs"
              << " : Global=" << nUsedglobalPIDs
              << " Local=" << nUsedlocalPIDs
              << endreq;
    }
  }

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichPIDSimpleMerge::finalize() 
{
  // base class finalise
  return RichAlgBase::finalize();
}
