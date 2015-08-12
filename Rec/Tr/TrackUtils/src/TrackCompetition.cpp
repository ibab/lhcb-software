// track interfaces
#include "Event/Track.h"
#include "TrackCompetition.h"

#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>


using namespace LHCb;
using namespace boost::lambda;


DECLARE_ALGORITHM_FACTORY( TrackCompetition )

TrackCompetition::TrackCompetition(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
  declareProperty( "inputLocation",  m_inputLocation  = TrackLocation::Default );
  declareProperty( "outputLocation",  m_outputLocation  = TrackLocation::Default );
  declareProperty("fracUsed", m_fracUsed = 0.25);

}

TrackCompetition::~TrackCompetition()
{
  // destructor
}


StatusCode TrackCompetition::execute(){

  Tracks* inCont = get<Tracks>(m_inputLocation);
  Tracks* outCont = new Tracks(); outCont->reserve(inCont->size());
  put(outCont, m_outputLocation);

  // sort
  std::stable_sort(inCont->begin(),inCont->end(),bind(&Track::chi2,_1) < bind(&Track::chi2,_2));
  std::stable_sort(inCont->begin(),inCont->end(),bind(&Track::nLHCbIDs,_1) > bind(&Track::nLHCbIDs, _2));


  std::vector<LHCb::LHCbID> used; used.reserve(4000);
  for (Tracks::const_iterator iterT = inCont->begin(); 
       iterT != inCont->end(); ++iterT){
    // get the list of LHCbIDs 
    unsigned int nUsed = 0u;
    const std::vector<LHCbID>& ids = (*iterT)->lhcbIDs();
    BOOST_FOREACH(LHCbID id, ids){
      if (find(used.begin(), used.end(), id) != used.end()){
        ++nUsed;
      }
    } // for each
    
    double fracUsed = nUsed/double(ids.size());
    if (fracUsed < m_fracUsed){
      BOOST_FOREACH(LHCbID id, ids){
        used.push_back(id);
      } // for each
      Track* newTrack = (*iterT)->clone();
      outCont->insert(newTrack);   
    }        
  } // for each


  return StatusCode::SUCCESS;
}
