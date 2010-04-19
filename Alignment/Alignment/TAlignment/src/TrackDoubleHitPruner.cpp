// $Id: TrackDoubleHitPruner.cpp,v 1.1 2010-04-19 22:35:56 wouter Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolHandle.h"

#include "GaudiAlg/GaudiAlgorithm.h"

// track interfaces
#include "TrackInterfaces/ITrackKalmanFilter.h"

// event
#include "Event/Track.h"
#include "Event/TrackFitResult.h"
#include "Event/Node.h"

#include <string>
#include <boost/foreach.hpp>


class TrackDoubleHitPruner: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackDoubleHitPruner(const std::string& name,
		       ISvcLocator* pSvcLocator);
  virtual ~TrackDoubleHitPruner();

  virtual StatusCode execute();
  virtual StatusCode initialize();
  virtual StatusCode finalize();

private:
  std::string m_inputLocation;
  ToolHandle<ITrackKalmanFilter> m_kalmanFilter ;
};


DECLARE_ALGORITHM_FACTORY( TrackDoubleHitPruner );

TrackDoubleHitPruner::TrackDoubleHitPruner(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator),
  m_kalmanFilter("TrackKalmanFilter",this)
{
  // constructor
  declareProperty( "TrackLocation",  m_inputLocation  = LHCb::TrackLocation::Default );
}

TrackDoubleHitPruner::~TrackDoubleHitPruner()
{
  // destructor
}

StatusCode TrackDoubleHitPruner::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  sc = m_kalmanFilter.retrieve() ;
  return sc ;
}

StatusCode TrackDoubleHitPruner::finalize()
{
  m_kalmanFilter.release().ignore() ;
  return GaudiAlgorithm::finalize();
}

bool sameOTMonoLayer( const LHCb::OTChannelID& rhs,
			const LHCb::OTChannelID& lhs ) 
{
  return rhs.sequentialUniqueLayer() == lhs.sequentialUniqueLayer() &&
    rhs.straw()/32 == lhs.straw()/32 ;
}

bool sameSTModule( const LHCb::STChannelID& rhs,
		    const LHCb::STChannelID& lhs ) 
{
  return 
    rhs.station() == lhs.station() &&
    rhs.layer() == lhs.layer() &&
    rhs.detRegion() == lhs.detRegion() &&
    rhs.sector() == lhs.sector() ;
}

bool sameVeloSensor( const LHCb::VeloChannelID& rhs,
		     const LHCb::VeloChannelID& lhs ) 
{
  return rhs.sensor() == lhs.sensor() ;
}




StatusCode TrackDoubleHitPruner::execute()
{
  // get them as a range .... hen const-cast:-)
  LHCb::Track::Range tracks  = get<LHCb::Track::Range>(m_inputLocation) ;
  BOOST_FOREACH(const LHCb::Track* track, tracks) {
    // we assume that the nodes are sorted. we now check if there are
    // neighbouring nodes in the same detector layer. this is rather
    // detector specific.
    if( track->fitResult() ) {
      const LHCb::Node* prevnode(0) ;
      bool trackWasModified(false) ;
      BOOST_FOREACH(const LHCb::Node* node, track->fitResult()->nodes()) {
	if( node->type()==LHCb::Node::HitOnTrack ) {
	  if( prevnode ) {
	    LHCb::LHCbID idA = prevnode->measurement().lhcbID() ;
	    LHCb::LHCbID idB = node->measurement().lhcbID() ;
	    if( idA.detectorType() == idB.detectorType() ) {
	      bool samelayer(false) ;
	      if ( idA.isVelo() )
		samelayer = sameVeloSensor( idA.veloID(), idB.veloID() ) ;
	      else if ( idA.isTT() || idA.isIT() ) 
		samelayer = sameSTModule( idA.stID(), idB.stID() ) ;
	      // if( idA.isOT() )
	      // samelayer = sameOTMonoLayer( idA.otID(), idB.otID()) ;

	      if( samelayer ) {
		if( std::abs(node->residual()) > std::abs(prevnode->residual()) )
		  const_cast<LHCb::Node*>(node)->setType( LHCb::Node::Outlier ) ;
		else
		  const_cast<LHCb::Node*>(prevnode)->setType( LHCb::Node::Outlier ) ;
		trackWasModified = true ;
	      }
	    }
	  }
	  prevnode = node ;
	} else {
	  prevnode = 0 ;
	}
      }
      counter("Fraction of modified tracks") += trackWasModified ;
      if( trackWasModified ) {
	LHCb::Track* nctrack =  const_cast<LHCb::Track*>(track)  ;
	StatusCode sc = m_kalmanFilter->fit( *nctrack ) ;
	if( sc.isFailure() ) 
	  nctrack->setFitStatus( LHCb::Track::FitFailed ) ;
      } 
    }
  }
  return StatusCode::SUCCESS;
};

