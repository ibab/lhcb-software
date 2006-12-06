// $Id: TsaSeedSimpleSelector.cpp,v 1.1 2006-12-06 14:35:01 mneedham Exp $

// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

#include "TsaSeedSimpleSelector.h"
#include "TsaKernel/SeedFunctor.h"
#include "TsaKernel/SeedTrack.h"

#include <algorithm>
#include <utility>

#include "Event/State.h"

DECLARE_TOOL_FACTORY( TsaSeedSimpleSelector );

TsaSeedSimpleSelector::TsaSeedSimpleSelector(const std::string& type,
                               const std::string& name,
                                const IInterface* parent):
TsaSelectorBase(type, name, parent)
{
}

TsaSeedSimpleSelector::~TsaSeedSimpleSelector(){
  // destructer
}

StatusCode TsaSeedSimpleSelector::execute(LHCb::State& , std::vector<SeedTrack*>& seeds, std::vector<SeedHit*>*  ){
  return execute(seeds);
}

StatusCode TsaSeedSimpleSelector::execute(std::vector<SeedTrack*>& seeds, std::vector<SeedHit*>*  ){

//-------------------------------------------------------------------------
//  Select tracks in 3-D
//-------------------------------------------------------------------------

  // Sort seeds according to hits
  std::sort( seeds.begin(), seeds.end(), SeedFunctor::increasingByHits<const SeedTrack*>() );

  return select(seeds);
}
