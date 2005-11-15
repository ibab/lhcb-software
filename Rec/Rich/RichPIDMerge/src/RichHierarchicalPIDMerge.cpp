
//------------------------------------------------------------------------
/** @file RichHierarchicalPIDMerge.cpp
 *
 *  Implementation file for RICH algorithm : RichHierarchicalPIDMerge
 *
 *  CVS Log :-
 *  $Id: RichHierarchicalPIDMerge.cpp,v 1.3 2005-11-15 13:28:57 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-10
 */
//------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichHierarchicalPIDMerge.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichHierarchicalPIDMerge>          s_factory ;
const        IAlgFactory& RichHierarchicalPIDMergeFactory = s_factory ;


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
  // Input location in TDS for RichLocalPIDs
  declareProperty( "InputRingRefitPIDLocation",
                   m_richRingFitPIDLocation = RichRingRefitPIDLocation::Default );
  // Location of processing status object in TES
  declareProperty( "ProcStatusLocation",
                   m_procStatLocation = ProcStatusLocation::Default );

  // Flags to turn on/off various PID results
  declareProperty( "UseRingRefitPIDs", m_useRingPIDs   = false );
  declareProperty( "UseLocalPIDs",     m_useLocalPIDs  = true  );
  declareProperty( "UseGlobalPIDs",    m_useGlobalPIDs = true  );

  declareProperty( "UseRingPIDsIfRICH1Available", m_ringSel = false );

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

  unsigned int nUsedglobalPIDs = 0;
  unsigned int nUsedlocalPIDs  = 0;
  unsigned int nUsedRingPIDs   = 0;

  if ( m_useGlobalPIDs ) {
    // iterate over Global PID results and form output persistent objects

    SmartDataPtr<RichGlobalPIDs> gPIDs( eventSvc(), m_richGlobalPIDLocation );
    if ( !gPIDs ) {
      Warning("Cannot locate RichGlobalPIDs at "+m_richGlobalPIDLocation);
    } else {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "Successfully located " << gPIDs->size()
                  << " RichGlobalPIDs at " << m_richGlobalPIDLocation << endreq;

      for ( RichGlobalPIDs::const_iterator gPID = gPIDs->begin();
            gPID != gPIDs->end(); ++gPID ) {

        // Form new PID object, using existing RichPID as template
        newPIDs->insert( new RichPID(*gPID), (*gPID)->key() );
        ++nUsedglobalPIDs;

      }
    }

  }

  if ( m_useLocalPIDs ) {
    // iterate over Local PID results and place in output container

    SmartDataPtr<RichLocalPIDs> lPIDs(eventSvc(), m_richLocalPIDLocation);
    if ( !lPIDs ) {
      Warning("Cannot locate RichLocalPIDs at "+m_richLocalPIDLocation);
    } else {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "Successfully located " << lPIDs->size()
                  << " RichLocalPIDs at " << m_richLocalPIDLocation << endreq;

      for ( RichLocalPIDs::const_iterator lPID = lPIDs->begin();
            lPID != lPIDs->end();
            ++lPID ) {

        // if pid with this key exists, skip
        if ( newPIDs->object( (*lPID)->key() ) ) continue;

        // Form new PID object, using existing RichPID as template
        newPIDs->insert( new RichPID(*lPID), (*lPID)->key() );
        ++nUsedlocalPIDs;

      }

    }

  }

  if ( m_useRingPIDs ) {
    // iterate over Ring PID results and form output persistent objects

    SmartDataPtr<RichRingRefitPIDs> rPIDs(eventSvc(), m_richRingFitPIDLocation);
    if ( !rPIDs ) {
      Warning("Cannot locate RichRingRefitPIDs at "+m_richRingFitPIDLocation);
    } else {
      if ( msgLevel(MSG::VERBOSE) )
        verbose()  << "Successfully located " << rPIDs->size()
                   << " RichRingRefitPIDs at " << m_richRingFitPIDLocation << endreq;

      for ( RichRingRefitPIDs::const_iterator rPID = rPIDs->begin();
            rPID != rPIDs->end(); ++rPID ) {

        // Do we already have a PID result for this track
        RichPID * oldPID = newPIDs->object( (*rPID)->key() );
        if ( oldPID ) {
          const bool inRich1 = oldPID->usedAerogel() || oldPID->usedC4F10();
          if ( m_ringSel || !inRich1 ) { newPIDs->erase( oldPID ); }
          else                         { continue;                 }
        }

        // Form new PID object, using existing RichPID as template
        newPIDs->insert( new RichPID(*rPID), (*rPID)->key() );
        ++nUsedRingPIDs;

      }

    }

  }

  // Update Rich status words
  procStat->addAlgorithmStatus( name()+":UsedGlobalPIDs",    nUsedglobalPIDs );
  procStat->addAlgorithmStatus( name()+":UsedLocalPIDs",     nUsedlocalPIDs  );
  procStat->addAlgorithmStatus( name()+":UsedRingRefitPIDs", nUsedRingPIDs   );

  // Final debug information
  if ( msgLevel(MSG::DEBUG) ) {
    if ( !pidsExist ) {
      debug() << "Successfully registered " << newPIDs->size()
              << " RichPIDs at " << m_richPIDLocation
              << " : Global=" << nUsedglobalPIDs
              << " Local=" << nUsedlocalPIDs
              << " RingRefit=" << nUsedRingPIDs
              << endreq;
    } else {
      debug() << "Replaced " << originalSize << " pre-existing RichPIDs at "
              << m_richPIDLocation << " with " << newPIDs->size()
              << " new RichPIDs"
              << " : Global=" << nUsedglobalPIDs
              << " Local=" << nUsedlocalPIDs
              << " RingRefit=" << nUsedRingPIDs
              << endreq;
    }
  }

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichHierarchicalPIDMerge::finalize()
{
  // base class finalise
  return RichAlgBase::finalize();
}
