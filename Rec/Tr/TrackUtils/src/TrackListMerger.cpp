/** @class TrackListMerger TrackListMerger.h
 *
 *  Merge different track lists.
 *
 *  @author Wouter Hulsbergen
 *  @date   05/01/2010
 */

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/SharedObjectsContainer.h"
#include <string>
#include <vector>
#include "Event/Track.h"


class TrackListMerger: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackListMerger(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackListMerger();
  virtual StatusCode execute();

private:
  std::vector<std::string> m_inputLocations;
  std::string m_outputLocation;
};


DECLARE_ALGORITHM_FACTORY( TrackListMerger )

TrackListMerger::TrackListMerger(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
  declareProperty( "inputLocations",  m_inputLocations) ;
  declareProperty( "outputLocation", m_outputLocation) ;
}

TrackListMerger::~TrackListMerger()
{
  // destructor
}

StatusCode TrackListMerger::execute()
{
  LHCb::Track::Selection* tracksout = new LHCb::Track::Selection() ;
  put( tracksout, m_outputLocation) ;
  
  // loop 
  for( std::vector<std::string>::const_iterator ilist = m_inputLocations.begin() ;
       ilist != m_inputLocations.end(); ++ilist) {
    LHCb::Track::Range tracksin  = get<LHCb::Track::Range>(*ilist) ;
    for (Gaudi::Range_<LHCb::Track::ConstVector>::const_iterator itrack = tracksin.begin(); 
	 itrack != tracksin.end(); ++itrack) 
      // make sure the track is not yet there!
      if( std::find( tracksout->begin(),tracksout->end(),*itrack) == tracksout->end() )
	tracksout->insert( *itrack ) ;
  }
  return StatusCode::SUCCESS;
}
