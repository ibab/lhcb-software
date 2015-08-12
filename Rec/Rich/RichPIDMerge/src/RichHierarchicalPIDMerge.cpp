
//------------------------------------------------------------------------
/** @file RichHierarchicalPIDMerge.cpp
 *
 *  Implementation file for RICH algorithm : RichHierarchicalPIDMerge
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-10
 */
//------------------------------------------------------------------------

// local
#include "RichHierarchicalPIDMerge.h"

// namespaces
using namespace Rich::Rec;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HierarchicalPIDMerge )

// Standard constructor, initializes variables
HierarchicalPIDMerge::HierarchicalPIDMerge( const std::string& name,
                                            ISvcLocator* pSvcLocator )
  : Rich::AlgBase ( name , pSvcLocator )
{

  // Output location in TDS for RichPIDs
  declareProperty( "OutputPIDLocation", m_richPIDLocation );

  // Input location in TDS for RichGlobalPIDs
  declareProperty( "InputGlobalPIDLocation", 
                   m_richGlobalPIDLocation = contextSpecificTES(LHCb::RichGlobalPIDLocation::Default) );

  // Location of processing status object in TES
  declareProperty( "ProcStatusLocation",
                   m_procStatLocation = LHCb::ProcStatusLocation::Default );

  // Flags to turn on/off various PID results
  declareProperty( "UseGlobalPIDs",    m_useGlobalPIDs = true  );

  // fill ProcStat
  declareProperty( "FillProcStat", m_fillProcStat = !contextContains("HLT") );

  // version
  declareProperty( "PIDVersion", m_PIDversion = 0 );

}

// Destructor
HierarchicalPIDMerge::~HierarchicalPIDMerge() { }

// Initialisation.
StatusCode HierarchicalPIDMerge::initialize()
{
  // initialise base classclean
  const StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  if ( m_richPIDLocation.empty() )
  { return Error( "Output RichPID Location is not set" ); }

  return sc;
}

// Main execution
StatusCode HierarchicalPIDMerge::execute()
{
  // See if PIDs already exist at requested output location
  LHCb::RichPIDs * newPIDs = NULL;
  bool pidsExist = false;
  unsigned long originalSize = 0;
  SmartDataPtr<LHCb::RichPIDs> outPIDs( eventSvc(), m_richPIDLocation );
  if ( outPIDs )
  {
    // Use existing container, emptied of previous results
    pidsExist = true;
    newPIDs = outPIDs;
    originalSize = newPIDs->size();
    newPIDs->clear();
  } 
  else
  {
    // Form new container for output PIDs
    newPIDs = new LHCb::RichPIDs();
    put( newPIDs, m_richPIDLocation );
  }

  // PID version
  newPIDs->setVersion( (unsigned char)m_PIDversion );

  // Locate the processing status object  
  LHCb::ProcStatus * procStat = ( m_fillProcStat ? 
                                  get<LHCb::ProcStatus>(m_procStatLocation) : NULL );
  if ( procStat && procStat->aborted() )
  {
    return Warning("Processing already aborted -> Empty RichPID container",StatusCode::SUCCESS);
  }

  // tallies of number of PID results used of each type
  unsigned long nUsedglobalPIDs(0);

  if ( m_useGlobalPIDs )
  {
    // iterate over Global PID results and form output persistent objects

    SmartDataPtr<LHCb::RichGlobalPIDs> gPIDs( eventSvc(), m_richGlobalPIDLocation );
    if ( !gPIDs )
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Cannot locate RichGlobalPIDs at " << m_richGlobalPIDLocation << endmsg;
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "Successfully located " << gPIDs->size()
                  << " RichGlobalPIDs at " << m_richGlobalPIDLocation << endmsg;

      for ( LHCb::RichGlobalPIDs::const_iterator gPID = gPIDs->begin();
            gPID != gPIDs->end(); ++gPID )
      {
        // Form new PID object, using existing RichPID as template
        newPIDs->insert( new LHCb::RichPID(**gPID), (*gPID)->key() );
        ++nUsedglobalPIDs;
      }

    }

  } // use Global PIDs

  // Final debug information
  if ( msgLevel(MSG::DEBUG) )
  {
    if ( !pidsExist )
    {
      debug() << "Successfully registered " << newPIDs->size()
              << " RichPIDs at " << m_richPIDLocation
              << " : Global=" << nUsedglobalPIDs
              << endmsg;
    }
    else
    {
      debug() << "Replaced " << originalSize << " pre-existing RichPIDs at "
              << m_richPIDLocation << " with " << newPIDs->size()
              << " new RichPIDs"
              << " : Global=" << nUsedglobalPIDs
              << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}
