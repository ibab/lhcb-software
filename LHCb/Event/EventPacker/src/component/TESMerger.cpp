/** @class TESMerger TESMerger.h
 *
 *  Merge different track lists.
 *
 *  @author Sean Benson
 *  @date   24/02/2014
 */

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/SharedObjectsContainer.h"
#include <string>
#include <vector>
#include "Event/Track.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"


template <class T> class TESMerger: public GaudiAlgorithm {

public:
  TESMerger(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual StatusCode execute();

private:
  std::vector<std::string> m_inputLocations;
  std::string m_outputLocation;
};
  
template <class T>
TESMerger<T>::TESMerger(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
  declareProperty( "inputLocations",  m_inputLocations) ;
  declareProperty( "outputLocation", m_outputLocation) ;
}

template <class T>
StatusCode TESMerger<T>::execute()
{
  //SharedObjectsContainer<T>* out = new SharedObjectsContainer<T>() ;
  KeyedContainer<T, Containers::HashMap >* out = new KeyedContainer<T, Containers::HashMap >() ;
  put( out, m_outputLocation) ;
  
  for( std::vector<std::string>::const_iterator ilist = m_inputLocations.begin() ;
       ilist != m_inputLocations.end(); ++ilist) {
    //typename Gaudi::NamedRange_<std::vector<const T*> > cont_in = get<typename Gaudi::NamedRange_<std::vector<const T*> > >(*ilist) ;
    KeyedContainer<T, Containers::HashMap >* cont_in = get<KeyedContainer<T, Containers::HashMap > >(*ilist) ;
    for(auto i_obj = cont_in->begin(); 
	 i_obj != cont_in->end(); ++i_obj) 
	out->insert( (*i_obj)->clone() ) ;
  }
  return StatusCode::SUCCESS;
}

typedef TESMerger<LHCb::ProtoParticle> TESMergerProtoParticle;
DECLARE_ALGORITHM_FACTORY( TESMergerProtoParticle )
typedef TESMerger<LHCb::Track> TESMergerTrack;
DECLARE_ALGORITHM_FACTORY( TESMergerTrack )
typedef TESMerger<LHCb::Particle> TESMergerParticle;
DECLARE_ALGORITHM_FACTORY( TESMergerParticle )
