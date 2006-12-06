// $Id: TsaSelectorBase.cpp,v 1.1 2006-12-06 14:35:02 mneedham Exp $

// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

#include "TsaSelectorBase.h"
#include "TsaKernel/SeedFunctor.h"
#include "TsaKernel/SeedTrack.h"

#include <algorithm>
#include <utility>

#include "Event/State.h"

TsaSelectorBase::TsaSelectorBase(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent):
GaudiTool(type, name, parent)
{
  declareProperty("fracUsed", m_fracUsed = 0.4);
 
  declareInterface<ITsaSeedStep>(this);
};

TsaSelectorBase::~TsaSelectorBase(){
  // destructer
}

StatusCode TsaSelectorBase::select(std::vector<SeedTrack*>& seeds){

//-------------------------------------------------------------------------
//  Select tracks in 3-D - tracks are sorted by some criteria in the derived 
//  class
//-------------------------------------------------------------------------

  std::vector<SeedTrack*>::iterator iter = seeds.begin();

  for ( ; iter != seeds.end(); ++iter){

    if ((*iter)->live() == false || (*iter)->select() == true) continue;

    std::vector<Tsa::Cluster*> clusVector = (*iter)->clusters();
    double fracUsed = (double)std::count_if(clusVector.begin(),clusVector.end(), 
                                            std::mem_fun(&Tsa::Cluster::onTrack))/(double)clusVector.size();

    if (fracUsed < m_fracUsed) {
      (*iter)->setSelect(true);
      std::for_each(clusVector.begin(),clusVector.end(), std::bind2nd(std::mem_fun(&Tsa::Cluster::setOnTrack),true));
    }
  } //iter
 
  // loop again and reset used.
  for (iter = seeds.begin() ; iter != seeds.end(); ++iter){
    std::vector<Tsa::Cluster*> xClusVector = (*iter)->xClusters();
    if ((*iter)->select()  ==false){
      std::for_each(xClusVector.begin(),xClusVector.end(), std::bind2nd(std::mem_fun(&Tsa::Cluster::setOnTrack),false));
    }
    else {
      std::for_each(xClusVector.begin(),xClusVector.end(), std::bind2nd(std::mem_fun(&Tsa::Cluster::setOnTrack),true));
    }
  }

  return StatusCode::SUCCESS;
}
