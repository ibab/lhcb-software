
//------------------------------------------------------------------------
/** @file RichPIDMerge.cpp
 *
 *  Implementation file for RICH algorithm : RichPIDMerge
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-10
 */
//------------------------------------------------------------------------

// local
#include "RichPIDMerge.h"

// namespaces
using namespace Rich::Rec;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PIDMerge )

// Standard constructor, initializes variables
PIDMerge::PIDMerge( const std::string& name,
                    ISvcLocator* pSvcLocator )
: Rich::AlgBase ( name , pSvcLocator )
{
  // Output location in TDS for RichPIDs
  declareProperty( "OutputLocation", m_output );
  
  // Input location in TDS for RichGlobalPIDs
  declareProperty( "InputLocations", m_inputs );

  //setProperty( "OutputLevel", 1 );
}

// Destructor
PIDMerge::~PIDMerge() { }

// Initialisation.
StatusCode PIDMerge::initialize()
{
  // initialise base classclean
  const StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  if ( m_output.empty() )
  { return Error( "Output RichPID Location is not set" ); }

  return sc;
}

// Main execution
StatusCode PIDMerge::execute()
{

  // Create/get the output PID location
  LHCb::RichPIDs * mergedPIDs = getOrCreate<LHCb::RichPIDs,LHCb::RichPIDs>(m_output);
 
  // If non-empty, clean out existing PIDs
  if ( !mergedPIDs->empty() )
  {
    _ri_debug << "Found " << mergedPIDs->size() << " existing RichPIDs at '"
              << m_output << "'. -> Will empty and reuse" << endmsg;
    mergedPIDs->clear();
  }

  // Keep track of the version of all input locations
  std::unordered_set<unsigned char> inputVersions;

  // Loop over all input locations and clone to output
  for ( const std::string& loc : m_inputs )
  {
    const LHCb::RichPIDs * pids = getIfExists<LHCb::RichPIDs>(loc);
    if ( pids )
    {
      // Save the version for this container
      inputVersions.insert( pids->version() );
      // reserve size
      mergedPIDs->reserve( mergedPIDs->size() + pids->size() );
      // Loop over PIDs and clone
      for ( const LHCb::RichPID * pid : *pids )
      {
        // Insert new cloned PID object with the same key as original
        // As this is based on the underlying track key, which should be unique
        // across all PID containers, all keys should be unique...
        mergedPIDs->insert( new LHCb::RichPID(*pid), pid->key() );
      }
    }
    else
    {
      Warning( "Failed to load input RichPIDs at '" + loc + "'" ).ignore();
    }
  }

  // Make sure all input PID containers have the same version,
  // and set the output version to the same value.
  if ( !inputVersions.empty() )
  {
    if ( 1 != inputVersions.size() )
    {
      Warning( "Mis-mastch in DataObject version for input locations" ).ignore();
    }
    mergedPIDs->setVersion( *inputVersions.begin() );
  }

  _ri_debug << "Created " << mergedPIDs->size() << " RichPIDs at '" 
            << m_output << "'" << endmsg;

  return StatusCode::SUCCESS;
}
