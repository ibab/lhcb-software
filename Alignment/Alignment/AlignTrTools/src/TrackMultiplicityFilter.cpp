// $Id: TrackListFilter.cpp,v 1.2 2010-01-05 11:13:39 wouter Exp $


/** @class TrackMultiplicityFilter TrackMultiplicityFilter.h
 *
 *  Algorithm to filter events in which a track list is not empty
 *
 *  @author W. Hulsbergen
 *  @date   2008
 */

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>

// track interfaces
#include "Event/STCluster.h"

class TrackMultiplicityFilter: public GaudiAlgorithm 
{
public:
  // Constructors and destructor
  TrackMultiplicityFilter(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackMultiplicityFilter();
  virtual StatusCode execute();
  
private:
  size_t m_maxNumITHits ;
};

DECLARE_ALGORITHM_FACTORY( TrackMultiplicityFilter );


TrackMultiplicityFilter::TrackMultiplicityFilter(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
  declareProperty( "MaxNumITHits",m_maxNumITHits = 500) ;
}

TrackMultiplicityFilter::~TrackMultiplicityFilter()
{
}

StatusCode TrackMultiplicityFilter::execute()
{
  setFilterPassed(true) ;
  std::string m_clusterLocation = LHCb::STClusterLocation::ITClusters ;
  if(exist<LHCb::STClusters>(m_clusterLocation)) {
    LHCb::STClusters* clusters = get<LHCb::STClusters>(m_clusterLocation);
    if( clusters && clusters->size() > m_maxNumITHits )
      setFilterPassed(false) ;
  }
  return  StatusCode::SUCCESS ;
}
