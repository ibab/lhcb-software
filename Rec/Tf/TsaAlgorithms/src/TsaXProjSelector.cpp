#include "TsaXProjSelector.h"

#include <algorithm>
#include <utility>

#include "Event/State.h"

#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

using namespace boost::lambda;
using namespace Tf::Tsa;

DECLARE_TOOL_FACTORY( XProjSelector )

XProjSelector::XProjSelector(const std::string& type,
                             const std::string& name,
                             const IInterface* parent):
  GaudiTool(type, name, parent)
{
  declareProperty("fracUsed", m_fracUsed = 0.9);

  declareInterface<ITsaSeedStep>(this);
}

XProjSelector::~XProjSelector() { }

StatusCode XProjSelector::execute(LHCb::State& ,
                                  std::vector<SeedTrack*>& seeds, std::vector<SeedHit*>*  ){
  return execute(seeds);
}

StatusCode XProjSelector::execute(std::vector<SeedTrack*>& seeds, std::vector<SeedHit*>*  ){

  //-------------------------------------------------------------------------
  //  Select tracks in x projection
  //-------------------------------------------------------------------------

  // Sort seeds according to their likelihood
  std::stable_sort(seeds.begin(),seeds.end(), SeedFunctor::LessByXChi2<const SeedTrack*>());
  std::stable_sort( seeds.begin(), seeds.end(), SeedFunctor::increasingByXHits<const SeedTrack*>() );

  std::vector<SeedTrack*>::iterator iter = seeds.begin() ;
  for (; iter != seeds.end(); ++iter){


    std::vector<SeedPnt>& xClusVector = (*iter)->xPnts();
    double fracUsedX = (double)std::count_if(xClusVector.begin(),xClusVector.end(),
                                             bind(&SeedPnt::onTrack,_1))/(double)xClusVector.size();

    if (fracUsedX < m_fracUsed) {
      std::for_each(xClusVector.begin(),xClusVector.end(),bind(&SeedPnt::setOnTrack,_1,true));
    }
    else {
      (*iter)->setLive(false);
    }
  } //iter

  // loop again and reset used.
  for (iter = seeds.begin() ; iter != seeds.end(); ++iter){
    std::vector<SeedPnt>& xClusVector = (*iter)->xPnts();
    std::for_each(xClusVector.begin(),xClusVector.end(),bind(&SeedPnt::setOnTrack,_1,false));
  }

  return StatusCode::SUCCESS;
}
