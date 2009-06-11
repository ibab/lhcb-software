
//------------------------------------------------------------------------
/** @file RichHierarchicalPIDMerge.cpp
 *
 *  Implementation file for RICH algorithm : RichHierarchicalPIDMerge
 *
 *  CVS Log :-
 *  $Id: RichHierarchicalPIDMerge.cpp,v 1.11 2009-06-11 14:21:10 jonrob Exp $
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
using namespace Rich::Rec;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HierarchicalPIDMerge );

// Standard constructor, initializes variables
HierarchicalPIDMerge::HierarchicalPIDMerge( const std::string& name,
                                            ISvcLocator* pSvcLocator )
  : Rich::AlgBase ( name , pSvcLocator ),
    m_richPIDLocation       ( LHCb::RichPIDLocation::Default ),
    m_richGlobalPIDLocation ( LHCb::RichGlobalPIDLocation::Default ),
    m_fillProcStat          ( true )
{

  if      ( context() == "Offline" )
  {
    m_richPIDLocation = LHCb::RichPIDLocation::Offline;
    m_richGlobalPIDLocation = LHCb::RichGlobalPIDLocation::Offline;
    m_fillProcStat = true;
  }
  else if ( context() == "HLT" || context() == "Hlt" )
  {
    m_richPIDLocation = LHCb::RichPIDLocation::HLT;
    m_richGlobalPIDLocation = LHCb::RichGlobalPIDLocation::HLT;
    m_fillProcStat = false;
  }

  // Output location in TDS for RichPIDs
  declareProperty( "OutputPIDLocation", m_richPIDLocation );
  // Input location in TDS for RichGlobalPIDs
  declareProperty( "InputGlobalPIDLocation", m_richGlobalPIDLocation );
  // Location of processing status object in TES
  declareProperty( "ProcStatusLocation",
                   m_procStatLocation = LHCb::ProcStatusLocation::Default );

  // Flags to turn on/off various PID results
  declareProperty( "UseGlobalPIDs",    m_useGlobalPIDs = true  );

  // fill procstat
  declareProperty( "FillProcStat", m_fillProcStat );

}

// Destructor
HierarchicalPIDMerge::~HierarchicalPIDMerge() { }

// Initialisation.
StatusCode HierarchicalPIDMerge::initialize()
{
  // initialise base classclean
  const StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Add any customisations here

  return sc;
}

// Main execution
StatusCode HierarchicalPIDMerge::execute()
{
  // See if PIDs already exist at requested output location
  LHCb::RichPIDs * newPIDs = NULL;
  bool pidsExist = false;
  unsigned int originalSize = 0;
  SmartDataPtr<LHCb::RichPIDs> outPIDs( eventSvc(), m_richPIDLocation );
  if ( outPIDs )
  {
    // Use existing container, emptied of previous results
    pidsExist = true;
    newPIDs = outPIDs;
    originalSize = newPIDs->size();
    newPIDs->clear();
  } else {
    // Form new container for output PIDs
    newPIDs = new LHCb::RichPIDs();
    put( newPIDs, m_richPIDLocation );
  }

  // Locate the processing status object
  
  LHCb::ProcStatus * procStat = ( m_fillProcStat ? get<LHCb::ProcStatus>(m_procStatLocation) : NULL );
  if ( procStat && procStat->aborted() )
  {
    return Warning("Processing aborted -> Empty RichPID container",StatusCode::SUCCESS);
  }

  // tallies of number of PID results used of each type
  unsigned int nUsedglobalPIDs(0);

  if ( m_useGlobalPIDs )
  {
    // iterate over Global PID results and form output persistent objects

    SmartDataPtr<LHCb::RichGlobalPIDs> gPIDs( eventSvc(), m_richGlobalPIDLocation );
    if ( !gPIDs )
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Cannot locate RichGlobalPIDs at " << m_richGlobalPIDLocation << endreq;
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "Successfully located " << gPIDs->size()
                  << " RichGlobalPIDs at " << m_richGlobalPIDLocation << endreq;

      for ( LHCb::RichGlobalPIDs::const_iterator gPID = gPIDs->begin();
            gPID != gPIDs->end(); ++gPID )
      {
        // Form new PID object, using existing RichPID as template
        newPIDs->insert( new LHCb::RichPID(**gPID), (*gPID)->key() );
        ++nUsedglobalPIDs;
      }

    }

  } // use Global PIDs

  // Update Rich status words
  if ( procStat )
  {
    procStat->addAlgorithmStatus( name()+":UsedGlobalPIDs",    nUsedglobalPIDs );
  }

  // Final debug information
  if ( msgLevel(MSG::DEBUG) )
  {
    if ( !pidsExist )
    {
      debug() << "Successfully registered " << newPIDs->size()
              << " RichPIDs at " << m_richPIDLocation
              << " : Global=" << nUsedglobalPIDs
              << endreq;
    }
    else
    {
      debug() << "Replaced " << originalSize << " pre-existing RichPIDs at "
              << m_richPIDLocation << " with " << newPIDs->size()
              << " new RichPIDs"
              << " : Global=" << nUsedglobalPIDs
              << endreq;
    }
  }

  return StatusCode::SUCCESS;
}
