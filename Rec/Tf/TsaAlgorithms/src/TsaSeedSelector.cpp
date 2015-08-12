#include "TsaSeedSelector.h"
#include "TsaKernel/SeedFunctor.h"
#include "TsaKernel/SeedTrack.h"

#include <algorithm>
#include <utility>

#include "Event/State.h"

using namespace Tf::Tsa;

DECLARE_TOOL_FACTORY( SeedSelector )

SeedSelector::SeedSelector(const std::string& type,
                           const std::string& name,
                           const IInterface* parent):
  SelectorBase(type, name, parent) { }

SeedSelector::~SeedSelector() { }

StatusCode SeedSelector::execute( LHCb::State& ,
                                  std::vector<SeedTrack*>& seeds,
                                  std::vector<SeedHit*>* )
{
  return execute(seeds);
}

StatusCode SeedSelector::execute( std::vector<SeedTrack*>& seeds,
                                  std::vector<SeedHit*>* )
{
  //-------------------------------------------------------------------------
  //  Select tracks in 3-D
  //-------------------------------------------------------------------------

  // Sort seeds according to their likelihood + # hits
  std::stable_sort(seeds.begin(), seeds.end(), 
                   SeedFunctor::decreasingLikelihood<const SeedTrack*>() );
  std::stable_sort(seeds.begin(), seeds.end(), 
                   SeedFunctor::increasingByHits<const SeedTrack*>() );
  const StatusCode sc = select(seeds);
  if ( msgLevel(MSG::VERBOSE) )
  {

    for ( std::vector<SeedTrack*>::const_iterator iS = seeds.begin(); iS != seeds.end(); ++iS )
    {

    }
  }
  return sc;
}
