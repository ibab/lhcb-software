/** @class TrackListFilter TrackListFilter.h
 *
 *  Algorithm to filter events in which a track list is not empty
 *
 *  @author W. Hulsbergen
 *  @date   2008
 */

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>

// track interfaces
#include "Event/Track.h"

class TrackListFilter: public GaudiAlgorithm 
{
public:
  // Constructors and destructor
  TrackListFilter(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackListFilter();
  virtual StatusCode execute();

private:
  std::string m_inputLocation;
};


DECLARE_ALGORITHM_FACTORY( TrackListFilter )


TrackListFilter::TrackListFilter(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
  declareProperty( "inputLocation",  
		   m_inputLocation  = LHCb::TrackLocation::Default );
}

TrackListFilter::~TrackListFilter()
{
}

StatusCode TrackListFilter::execute()
{
  LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_inputLocation);
  setFilterPassed( tracks.empty() ? false : true ) ;
  return  StatusCode::SUCCESS ;
}
