
//------------------------------------------------------------------------
/** @file RichHierarchicalPIDMerge.cpp
 *
 *  Implementation file for RICH algorithm : RichHierarchicalPIDMerge
 *
 *  CVS Log :-
 *  $Id: RichHierarchicalPIDMerge.cpp,v 1.6 2006-12-19 09:41:27 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-10
 */
//------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichHierarchicalPIDMerge.h"

// namespaces
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( RichHierarchicalPIDMerge );


// Standard constructor, initializes variables
RichHierarchicalPIDMerge::RichHierarchicalPIDMerge( const std::string& name,
                                                    ISvcLocator* pSvcLocator )
  : RichAlgBase ( name , pSvcLocator )
{

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

  // Flags to turn on/off various PID results
  declareProperty( "UseLocalPIDs",     m_useLocalPIDs  = true  );
  declareProperty( "UseGlobalPIDs",    m_useGlobalPIDs = true  );

}

// Destructor
RichHierarchicalPIDMerge::~RichHierarchicalPIDMerge() {};

// Initialisation.
StatusCode RichHierarchicalPIDMerge::initialize()
{
  // initialise base classclean
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Add any customisations here

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode RichHierarchicalPIDMerge::execute()
{
  debug() << "Execute" << endreq;

  // See if PIDs already exist at requested output location
  RichPIDs * newPIDs = NULL;
  bool pidsExist = false;
  unsigned int originalSize = 0;
  SmartDataPtr<RichPIDs> outPIDs( eventSvc(), m_richPIDLocation );
  if ( outPIDs )
  {
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

  // Locate the processing status object
  ProcStatus * procStat = get<ProcStatus>( m_procStatLocation );
  if ( procStat->aborted() )
  {
    return Warning("Processing aborted -> Empty RichPID container",StatusCode::SUCCESS);
  }

  // tallies of number of PID results used of each type
  unsigned int nUsedglobalPIDs(0), nUsedlocalPIDs(0);

  if ( m_useGlobalPIDs )
  {
    // iterate over Global PID results and form output persistent objects

    SmartDataPtr<RichGlobalPIDs> gPIDs( eventSvc(), m_richGlobalPIDLocation );
    if ( !gPIDs )
    {
      Warning("Cannot locate RichGlobalPIDs at "+m_richGlobalPIDLocation);
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "Successfully located " << gPIDs->size()
                  << " RichGlobalPIDs at " << m_richGlobalPIDLocation << endreq;

      for ( RichGlobalPIDs::const_iterator gPID = gPIDs->begin();
            gPID != gPIDs->end(); ++gPID )
      {
        // Form new PID object, using existing RichPID as template
        newPIDs->insert( new RichPID(*gPID), (*gPID)->key() );
        ++nUsedglobalPIDs;
      }

    }

  }

  if ( m_useLocalPIDs )
  {
    // iterate over Local PID results and place in output container

    SmartDataPtr<RichLocalPIDs> lPIDs(eventSvc(), m_richLocalPIDLocation);
    if ( !lPIDs )
    {
      Warning("Cannot locate RichLocalPIDs at "+m_richLocalPIDLocation);
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "Successfully located " << lPIDs->size()
                  << " RichLocalPIDs at " << m_richLocalPIDLocation << endreq;

      for ( RichLocalPIDs::const_iterator lPID = lPIDs->begin();
            lPID != lPIDs->end(); ++lPID )
      {
        // if pid with this key exists, skip
        if ( !( newPIDs->object((*lPID)->key()) ) )
        {
          // Form new PID object, using existing RichPID as template
          newPIDs->insert( new RichPID(*lPID), (*lPID)->key() );
          ++nUsedlocalPIDs;
        }
      }

    }

  }

  // Update Rich status words
  procStat->addAlgorithmStatus( name()+":UsedGlobalPIDs",    nUsedglobalPIDs );
  procStat->addAlgorithmStatus( name()+":UsedLocalPIDs",     nUsedlocalPIDs  );

  // Final debug information
  if ( msgLevel(MSG::DEBUG) )
  {
    if ( !pidsExist )
    {
      debug() << "Successfully registered " << newPIDs->size()
              << " RichPIDs at " << m_richPIDLocation
              << " : Global=" << nUsedglobalPIDs
              << " Local=" << nUsedlocalPIDs
              << endreq;
    }
    else
    {
      debug() << "Replaced " << originalSize << " pre-existing RichPIDs at "
              << m_richPIDLocation << " with " << newPIDs->size()
              << " new RichPIDs"
              << " : Global=" << nUsedglobalPIDs
              << " Local=" << nUsedlocalPIDs
              << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichHierarchicalPIDMerge::finalize()
{
  // base class finalise
  return RichAlgBase::finalize();
}
