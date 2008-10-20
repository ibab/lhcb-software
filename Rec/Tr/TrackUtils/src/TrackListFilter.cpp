// $Id: TrackListFilter.cpp,v 1.1 2008-10-20 10:46:39 wouter Exp $


/** @class TrackListFilter TrackListFilter.h
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


DECLARE_ALGORITHM_FACTORY( TrackListFilter );


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
  const LHCb::Tracks* tracks = get<LHCb::Tracks>(m_inputLocation);
  StatusCode sc = StatusCode::SUCCESS;
  if( tracks==0 ) {
    error() << "no track list at location: "
	    << m_inputLocation << endreq ;
    sc = StatusCode::FAILURE ;
  } else {
    setFilterPassed( tracks->empty() ? false : true ) ;
  }
  return sc ;
};
