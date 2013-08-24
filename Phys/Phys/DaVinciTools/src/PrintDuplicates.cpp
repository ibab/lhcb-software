// $Id: PrintDuplicates.cpp,v 1.1 2012-10-11 11:56:34 elsasser Exp $

// local
#include "PrintDuplicates.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PrintDuplicates )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrintDuplicates::PrintDuplicates( const std::string& name,
                                  ISvcLocator* pSvcLocator )
: DaVinciAlgorithm ( name , pSvcLocator ),
  m_printDecay ( NULL ) 
{
  declareProperty( "MaxPrintoutsPerTESLoc", m_maxPrints = 10 );
}

//=============================================================================
/// Destructor
//=============================================================================
PrintDuplicates::~PrintDuplicates() {}

//#############################################################################
/// Execution
//#############################################################################
StatusCode PrintDuplicates::execute()
{
  // local map for this event
  LocHashMap hashMap;

  // Loop over particles and compute hashes
  for ( LHCb::Particle::Range::const_iterator ip = particles().begin();
        ip != particles().end(); ++ip )
  {
    const std::size_t h = LHCb::HashIDs::hashID( *ip );
    // current have to use energy to take PID swaps into account.
    // Would be better to have the option to include this in the hash. To Do.
    const double e = (*ip)->momentum().e();
    (hashMap[tesLocation(*ip)])[std::make_pair(h,e)] .push_back ( *ip );
  }

  // Look for duplicates within TES locations
  for ( LocHashMap::const_iterator iL = hashMap.begin();
        iL != hashMap.end(); ++iL )
  {
    const std::string& loc = iL->first;

    // Loop over the distinct trees within this TES location
    for ( PartHashMap::const_iterator iPH = iL->second.begin();
          iPH != iL->second.end(); ++iPH )
    {
      // do we have any duplicates
      if ( iPH->second.size() > 1 )
      {
        std::ostringstream mess;
        mess << "Found " << iPH->second.size()
             << " duplicate decays in '" << loc << "'";
        Warning( mess.str(), StatusCode::FAILURE, m_maxPrints );
        if ( m_countPerLoc[loc]++ < m_maxPrints )
        {
          printDecay()->printTree( iPH->second.begin(), iPH->second.end() );
        }
      }

    }

  }

  setFilterPassed(true);
  return StatusCode::SUCCESS;
}
