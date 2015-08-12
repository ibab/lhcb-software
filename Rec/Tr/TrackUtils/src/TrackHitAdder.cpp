// Include files

// from std
#include <string>
#include <vector>
#include "boost/lexical_cast.hpp"

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"

// from TrackEvent
#include "Event/Track.h"

#include "TrackKernel/TrackTraj.h"
#include "TfKernel/IITHitCreator.h"
#include "TfKernel/IOTHitCreator.h"
#include "TfKernel/ITTHitCreator.h"
#include "TrackInterfaces/ITrackHitCollector.h"
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/Line.h"

#include <boost/foreach.hpp>

#ifdef _WIN32
#pragma warning ( disable : 4355 ) // this in base member init. list, needed for tool handles
#pragma warning ( disable : 4244 ) // conversion of double to float in Root
#endif


class TrackHitAdder : public GaudiHistoAlg
{

public:
  /// Standard constructor
  TrackHitAdder( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackHitAdder( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize();    ///< Algorithm initialization
  virtual StatusCode execute();    ///< Algorithm execution

private:
  bool printDebug()   const {return msgLevel(MSG::DEBUG);};
  bool printVerbose() const {return msgLevel(MSG::VERBOSE);};
private:
  ToolHandle<Tf::IITHitCreator> m_ithitcreator ;
  ToolHandle<Tf::ITTHitCreator> m_tthitcreator ;
  ToolHandle<Tf::IOTHitCreator> m_othitcreator ;
  ToolHandle<ITrackHitCollector> m_trackhitcollector ;

  std::string m_trackLocation ;
  double m_maxDistOT ;
  double m_maxDistIT ;
  double m_maxDistTT ;
  double m_maxDistVelo ;
  double m_maxTolY ;
  bool m_forceOT ;
  bool m_forceIT ;
  bool m_forceTT ;
  bool m_forceVelo ;
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackHitAdder )

TrackHitAdder::TrackHitAdder( const std::string& name,
				ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_ithitcreator("Tf::STHitCreator<Tf::IT>/ITHitCreator"),
    m_tthitcreator("Tf::STHitCreator<Tf::TT>/TTHitCreator"),
    m_othitcreator("Tf::OTHitCreator/OTHitCreator"),
    m_trackhitcollector("TrackHitCollector",this)
{
  declareProperty("TrackLocation", m_trackLocation = LHCb::TrackLocation::Default  );
  declareProperty("MaxDistOT", m_maxDistOT = 2.5 ) ;
  declareProperty("MaxDistIT", m_maxDistIT = 1.0 ) ;
  declareProperty("MaxDistTT", m_maxDistTT = 0.5 ) ;
  declareProperty("MaxDistVelo", m_maxDistVelo = 0.1 ) ;
  declareProperty("MaxTolY", m_maxTolY = 10.0 ) ; 
  declareProperty("ForceOT", m_forceOT = false ) ;
  declareProperty("ForceIT", m_forceIT = false ) ;
  declareProperty("ForceTT", m_forceTT = false ) ;
  declareProperty("ForceVelo", m_forceVelo = false ) ;
}

TrackHitAdder::~TrackHitAdder() {}


StatusCode TrackHitAdder::initialize() {
  if ( !GaudiHistoAlg::initialize().isSuccess() ) return StatusCode::FAILURE ;
    
  if(!m_ithitcreator.retrieve().isSuccess()) 
    return Error("==> Failed to retrieve IT hit creator",StatusCode::FAILURE);
  
  if(!m_tthitcreator.retrieve().isSuccess()) 
    return Error("==> Failed to retrieve TT hit creator",StatusCode::FAILURE);
  
  if(!m_othitcreator.retrieve().isSuccess()) 
    return Error("==> Failed to retrieve OT hit creator",StatusCode::FAILURE);

  if(!m_trackhitcollector.retrieve().isSuccess()) 
    return Error("==> Failed to retrieve hit collector",StatusCode::FAILURE);


  return StatusCode::SUCCESS;
}

StatusCode TrackHitAdder::finalize() 
{
  m_ithitcreator.release() ;
  m_tthitcreator.release() ;
  m_othitcreator.release() ;
  m_trackhitcollector.release() ;
  return GaudiHistoAlg::finalize();
}

//=============================================================================
// Main execution
//=============================================================================

namespace {

  struct OT
  {
    typedef Tf::IOTHitCreator HitCreatorType ;
    typedef Tf::IOTHitCreator::OTRegion RegionType ;
    typedef Tf::OTHit HitType ;
    typedef Tf::OTHitRange HitRangeType ;
    enum {MaxStation=3,MaxLayer=4,MaxRegion=2} ;
  } ;
  
  struct IT
  {
    typedef Tf::IITHitCreator HitCreatorType ;
    typedef Tf::IITHitCreator::STRegion RegionType ;
    typedef Tf::STHit HitType ;
    typedef Tf::STHitRange HitRangeType ;
    enum {MaxStation=3,MaxLayer=4,MaxRegion=4} ;
  } ;
  
  struct TT
  {
    typedef Tf::ITTHitCreator HitCreatorType ;
    typedef Tf::ITTHitCreator::STRegion RegionType ;
    typedef Tf::STHit HitType ;
    typedef Tf::STHitRange HitRangeType ;
    enum {MaxStation=2,MaxLayer=2,MaxRegion=12} ;
  } ;
  
  template<class TYPE>
  size_t addHits( const LHCb::TrackTraj& reftraj,
		  const typename TYPE::HitCreatorType& hitcreator,
		  double maxDist,
		  double maxTolY,
		  std::vector<LHCb::LHCbID>& lhcbids)
  {
    // only take tracks with a small error in the middle of T
    LHCb::StateVector state ; 
    //= reftraj.state( 8000 ) ;
    //if( std::sqrt( state.covariance()(0,0) ) > 2 ) return 0 ;
    
    size_t nhits = lhcbids.size() ;
    hitcreator.hits().size() ;
    for(size_t istation=0; istation< TYPE::MaxStation; ++istation) {
      for(size_t ilayer=0; ilayer< TYPE::MaxLayer; ++ilayer) {
	for(size_t iregion=0; iregion< TYPE::MaxRegion;++iregion) {
	  
	  const typename TYPE::RegionType* region = hitcreator.region( istation,ilayer,iregion ) ;
	  assert(region!=0) ;
	  double regionz = 0.5*(region->zmin() + region->zmax() ) ;

	  // do this only once per layer, so speed up things
	  /*if(iregion==0)  */
	  state = reftraj.stateVector( regionz ) ;
	  double statex = state.x() + (regionz - state.z())*state.tx() ;
	  double statey = state.y() + (regionz - state.z())*state.ty() ;
	  //tolx = m_numSigmaOT * std::sqrt( state.covariance()(0,0) ) + m_hitSigmaOT/region->cosT() ;
	  //toly = m_numSigmaOT * std::sqrt( state.covariance()(1,1) ) + m_hitSigmaOT/region->sinT() ;
	  
	  double tolx = 10*maxDist/region->cosT() ;
	  double toly = 2*maxTolY ;
	  //std::min(10*maxDist/(region->sinT()+0.000001),5000.) ; 
	  
	  if( region->isXCompatible( statex, tolx ) &&
	      region->isYCompatible( statey, toly ) ) {
	    // most efficient: use local x-coordinate. hope this works:-)
	    double localx = region->xLocal( statex, statey ) ;
	    typename TYPE::HitRangeType thesehits = hitcreator.hitsLocalXRange( istation, ilayer, iregion, 
										localx -  tolx,
										localx +  tolx) ;
	    Gaudi::XYZPointF trkpoint(state.x(),state.y(),state.z()) ;
	    Gaudi::XYZVectorF trkdir(state.tx(),state.ty(),1) ;
	    // if there is more than one hit, choose the closest?
	    lhcbids.reserve( lhcbids.size() + thesehits.size() ) ;
	    BOOST_FOREACH( const typename TYPE::HitType* hit, thesehits ) 
	      if( hit->isYCompatible( statey, maxTolY ) ) {
		// compute the distance
		Gaudi::XYZPointF hitpoint = hit->beginPoint() ;
		Gaudi::XYZVectorF hitdir = hit->direction() ;
		Gaudi::XYZVectorF projectiondir = hitdir.Cross( trkdir ).Unit() ;
		Gaudi::XYZVectorF delta = hitpoint-trkpoint ;
		double distance = delta.Dot(projectiondir) ;
		if( std::abs(distance) < maxDist ) lhcbids.push_back( hit->lhcbID() );
	      }
	  }
	}
      }
    }
    return lhcbids.size() - nhits ;
  }
}

#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
using namespace boost::lambda;

namespace {
  size_t addLHCbIDs( std::vector<LHCb::LHCbID>& lhcbids,
		     std::vector<LHCb::LHCbID>& newlhcbids)
  {
    std::sort( newlhcbids.begin(), newlhcbids.end() ) ;
    std::vector<LHCb::LHCbID> mergedlhcbids( lhcbids.size() + newlhcbids.size()) ;
    std::vector<LHCb::LHCbID>::iterator end = 
      std::set_union( lhcbids.begin(), lhcbids.end(),
		      newlhcbids.begin(), newlhcbids.end(),
		      mergedlhcbids.begin() ) ;
    mergedlhcbids.erase( end, mergedlhcbids.end() ) ;
    lhcbids.swap( mergedlhcbids ) ;
    return lhcbids.size() - mergedlhcbids.size() ;
  }
}

StatusCode TrackHitAdder::execute() 
{
  // Get tracks.
  LHCb::Tracks* tracks = get<LHCb::Tracks>(m_trackLocation);

  // now loop over all tracks, create a line from the last state, loop
  // over all hits, etc
  
  for( LHCb::Tracks::iterator itrk = tracks->begin() ;
       itrk != tracks->end() ; ++itrk ) {
    LHCb::Track* track = *itrk ;

    bool trackIsModified(false) ;
    if( track->nStates() > 0 && track->fitStatus() != LHCb::Track::FitFailed ) {

      // check that the LHCbIDs on the track are unique and sorted
      LHCb::Track::LHCbIDContainer lhcbids = track->lhcbIDs() ;
      std::sort( lhcbids.begin(), lhcbids.end() ) ;
      if( lhcbids != track->lhcbIDs() ) {
	error() << "LHCbIds on input track are not sorted!" << endmsg;
	BOOST_FOREACH(const LHCb::LHCbID& id, track->lhcbIDs() ) {
	  error() << id << endmsg ;
	}
      }

      LHCb::Track::LHCbIDContainer::iterator it = std::unique( lhcbids.begin(), lhcbids.end() ) ;
      if( it != lhcbids.end() ) {
	error() << "LHCbIds on input track are not unique!" << endmsg;
	BOOST_FOREACH(const LHCb::LHCbID& id, track->lhcbIDs() ) {
	  error() << id << endmsg ;
	}
	lhcbids.erase( it, lhcbids.end() ) ;
      }
      
      LHCb::TrackTraj tracktraj( *track ) ;
      LHCb::Track::LHCbIDContainer lhcbidsorig = lhcbids ;
      
      if( (track->hasT() || m_forceOT) && m_maxDistOT>0) {
	// add OT hits
	std::vector<LHCb::LHCbID> otlhcbids ;
	addHits<OT>( tracktraj, *m_othitcreator, m_maxDistOT, m_maxTolY, otlhcbids) ;
	size_t numotadded = addLHCbIDs(lhcbids,otlhcbids) ;
	counter("NumOTHitsAdded") += numotadded ;
      }

      if( (track->hasT() || m_forceIT) && m_maxDistIT>0) {
	// add IT hits
	std::vector<LHCb::LHCbID> itlhcbids ;
	addHits<IT>( tracktraj, *m_ithitcreator, m_maxDistIT, m_maxTolY, itlhcbids) ;
	size_t numitadded = addLHCbIDs(lhcbids,itlhcbids) ;
	counter("NumITHitsAdded") += numitadded ;
      }
      
      const unsigned int nTTHits = std::count_if(track->lhcbIDs().begin(), track->lhcbIDs().end(),
						 bind(&LHCb::LHCbID::isTT,_1));
      if( (nTTHits>0 || m_forceTT) && m_maxDistTT > 0 ) {
	std::vector<LHCb::LHCbID> ttlhcbids ;
	addHits<TT>( tracktraj, *m_tthitcreator, m_maxDistTT, m_maxTolY, ttlhcbids) ;
	counter("NumTTHitsAdded") += addLHCbIDs(lhcbids,ttlhcbids) ;
      }

      if( (track->hasVelo() || m_forceVelo) && m_maxDistVelo > 0 ) {
	// delegate to ITrackHitCollector
	std::vector<ITrackHitCollector::IDWithResidual> veloidswithresidual ;
	m_trackhitcollector->execute(*track,veloidswithresidual,true,false,false,false,false) ;
	std::vector<LHCb::LHCbID> velolhcbids ;
	BOOST_FOREACH( const ITrackHitCollector::IDWithResidual& id, veloidswithresidual) {
	  if( std::abs(id.m_res) < m_maxDistVelo )
	    velolhcbids.push_back( id.m_id ) ;
	}
	counter("NumVeloHitsAdded") += addLHCbIDs(lhcbids,velolhcbids) ;
      }
  
      // if there were hits added, make sure fit is entirely
      // reinitialized. this is where we can gain a lot of time by
      // wrigin some functionality to insert nodes in existing track.
      if( lhcbidsorig != lhcbids ) {
	track->setSortedLhcbIDs( lhcbids ) ;
	track->setFitResult(0) ;
	trackIsModified = true ;
      }
    }
    counter("FracTracksModified") += trackIsModified ;
  }
  
  return StatusCode::SUCCESS ;
}
