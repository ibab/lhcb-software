// $Id: TsaXProjSelector.cpp,v 1.1 2006-12-06 14:35:03 mneedham Exp $

// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

#include "TsaXProjSelector.h"
#include "TsaKernel/SeedFunctor.h"
#include "TsaKernel/SeedTrack.h"

#include <algorithm>
#include <utility>

#include "Event/State.h"

DECLARE_TOOL_FACTORY( TsaXProjSelector );

TsaXProjSelector::TsaXProjSelector(const std::string& type,
                               const std::string& name,
                                const IInterface* parent):
GaudiTool(type, name, parent)
{
  declareProperty("fracUsed", m_fracUsed = 0.9);
 
  declareInterface<ITsaSeedStep>(this);
};

TsaXProjSelector::~TsaXProjSelector(){
  // destructer
}

StatusCode TsaXProjSelector::execute(LHCb::State& , std::vector<SeedTrack*>& seeds, std::vector<SeedHit*>*  ){
  return execute(seeds);
}

StatusCode TsaXProjSelector::execute(std::vector<SeedTrack*>& seeds, std::vector<SeedHit*>*  ){

//-------------------------------------------------------------------------
//  Select tracks in x projection
//-------------------------------------------------------------------------

  // Sort seeds according to their likelihood
  std::sort(seeds.begin(),seeds.end(), SeedFunctor::LessByXChi2<const SeedTrack*>());
  std::stable_sort( seeds.begin(), seeds.end(), SeedFunctor::increasingByXHits<const SeedTrack*>() );

  std::vector<SeedTrack*>::iterator iter = seeds.begin() ; 
  for (; iter != seeds.end(); ++iter){


    std::vector<Tsa::Cluster*> xClusVector = (*iter)->xClusters();
    double fracUsedX = (double)std::count_if(xClusVector.begin(),xClusVector.end(), 
                                            std::mem_fun(&Tsa::Cluster::onTrack))/(double)xClusVector.size();

    if (fracUsedX < m_fracUsed) {
      std::for_each(xClusVector.begin(),xClusVector.end(), std::bind2nd(std::mem_fun
                    (&Tsa::Cluster::setOnTrack),true));
    }
    else {
       (*iter)->setLive(false);
    }
  } //iter
 
  // loop again and reset used.
  for (iter = seeds.begin() ; iter != seeds.end(); ++iter){
    std::vector<Tsa::Cluster*> xClusVector = (*iter)->xClusters();
    std::for_each(xClusVector.begin(),xClusVector.end(), std::bind2nd(std::mem_fun(&Tsa::Cluster::setOnTrack),false));
  }

  return StatusCode::SUCCESS;
}
