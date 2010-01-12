// Include files

// from std
#include <string>
#include <vector>
#include "boost/lexical_cast.hpp"

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/AlgFactory.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/Node.h"
#include "Event/TrackFitResult.h"
#include "Event/FitNode.h"
#include "Event/STMeasurement.h"

// AIDA
#include "AIDA/IHistogram1D.h"

#include "TfKernel/IITHitCreator.h"
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/Line.h"

#include "TrackKernel/TrackTraj.h"
#include "TfKernel/IITHitCreator.h"

#include <boost/foreach.hpp>

namespace Al {
  class Equations;
  class Residuals ;
}
class IHistogram1D ;
class IHistogram2D ;

/** @class AlignAlgorithm AlignAlgorithm.h
*
*
*  @author Wouter Hulsbergen
*  @date   2008-09-07
*/

namespace {
  struct DeltaHistogrammer
  {
    AIDA::IHistogram1D* resh1 ;
    AIDA::IHistogram1D* hotresh1 ;
    DeltaHistogrammer( const std::string& tracktype, 
		       const std::string& boxtype,
		       GaudiHistoAlg& parent ) 
    {
      resh1 = parent.book1D(tracktype + "/" + boxtype + "/hitres", boxtype + " dx", -5,5,100) ;
      hotresh1 = parent.book1D(tracktype + "/" + boxtype + "/hotres", boxtype + " dx", -5,5,100) ;
    }
  } ;
}

class TrackITOverlapMonitor : public GaudiHistoAlg
{

public:
  TrackITOverlapMonitor( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrackITOverlapMonitor( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize();    ///< Algorithm initialization
  virtual StatusCode execute();    ///< Algorithm execution

private:
  enum TrackType {OTASide=0, OTCSide, ITASide, ITCSide, ITTop, ITBottom} ;
  int trackType( const LHCb::Track& track ) const ;
  DeltaHistogrammer* getHistogrammer( size_t itstation,
				      size_t itbox,
				      size_t tracktype ) {
    size_t id = itbox + 4* (itstation + 8*tracktype ) ;
    if(m_histograms.size()<id+1 || m_histograms[id]==0) {
      if(m_histograms.size()<id+1) m_histograms.resize(id+1) ;
      const char stations[3][256] = { "IT1","IT2","IT3" } ;
      const char boxes[4][256]    = { "CSideBox","ASideBox","BottomBox","TopBox"} ;
      const char types[6][256]    = { "OTASideTrack", "OTCSideTrack", "ITASideTrack", "ITCSideTrack", "ITTopTrack", "ITBottomTrack" } ;
      char boxname[256] ;
      sprintf(boxname,"%s%s",stations[itstation],boxes[itbox]) ;
      setHistoTopDir("Track/");
      m_histograms[id] = new DeltaHistogrammer(types[tracktype],boxname,*this) ;
    }
    return m_histograms[id] ;
  }
  
  size_t accumulate( const LHCb::Track& track ) ;

  size_t accumulate( const Tf::STHitRange& hits, 
		     const LHCb::State& refstate,
		     const LHCb::Track& track) ;
private:
  std::vector<DeltaHistogrammer*> m_histograms ;
  ToolHandle<Tf::IITHitCreator> m_ithitcreator ;
  std::string m_trackLocation ;
  double m_maxDistance ;
  double m_maxFineDistance ;
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackITOverlapMonitor );

TrackITOverlapMonitor::TrackITOverlapMonitor( const std::string& name,
				ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_ithitcreator("Tf::STHitCreator<Tf::IT>/Tf::ITHitCreator"),
    m_maxDistance(50.),
    m_maxFineDistance(10.)
{
  declareProperty("TrackLocation", m_trackLocation = LHCb::TrackLocation::Default  );
}

TrackITOverlapMonitor::~TrackITOverlapMonitor() {}


StatusCode TrackITOverlapMonitor::initialize() {
  if ( !GaudiHistoAlg::initialize().isSuccess() ) return StatusCode::FAILURE ;
  
  if(!m_ithitcreator.retrieve().isSuccess()) 
    return Error("==> Failed to retrieve IT hit creator",StatusCode::FAILURE);
  
  return StatusCode::SUCCESS;
}

StatusCode TrackITOverlapMonitor::finalize() 
{
  m_ithitcreator.release() ;
  return GaudiHistoAlg::finalize();
}

//=============================================================================
// Main execution
//=============================================================================

int TrackITOverlapMonitor::trackType( const LHCb::Track& track) const
{
  size_t numIT[4] = {0,0,0,0} ;
  size_t numOT[2] = {0,0} ;
  BOOST_FOREACH( const LHCb::LHCbID& id, track.lhcbIDs()) {
    if( id.isIT() ) {
      LHCb::STChannelID stid = id.stID() ;
      ++(numIT[stid.detRegion()-1]) ;
    } else if( id.isOT() ) {
      LHCb::OTChannelID otid = id.otID() ;
      if( otid.quarter() == 1 || otid.quarter() == 3 )
	++numOT[0] ;
      else
	++numOT[1] ;
    }
  }
  int rc = -1 ;
  if(      numIT[0] >= 8 ) rc = ITCSide ;
  else if( numIT[1] >= 8 ) rc = ITASide ;
  else if( numIT[2] >= 8 ) rc = ITBottom ;
  else if( numIT[3] >= 8 ) rc = ITTop ;
  else if( numOT[0] >= 16 ) rc = OTASide ;
  else if( numOT[1] >= 16 ) rc = OTCSide ;
  return rc ;
}


size_t TrackITOverlapMonitor::accumulate( const LHCb::Track& track )
{
  size_t numcompatible(0) ;
  int tracktype = trackType(track) ;
  if( tracktype>=0 ) {
    LHCb::TrackTraj trackraj(track) ;
    for(size_t istation=0; istation<3; ++istation) {
      for(size_t ilayer=0; ilayer<4; ++ilayer) {
	for(size_t iregion=0; iregion<4;++iregion) {
	  const Tf::IITHitCreator::STRegion* region = 
	    m_ithitcreator->region( istation,ilayer,iregion ) ;
	  double regionz = 0.5*(region->zmin() + region->zmax() ) ;
	  LHCb::StateVector refstate = trackraj.stateVector( regionz ) ;
	  Gaudi::XYZPointF trkpoint(refstate.x(),refstate.y(),refstate.z()) ;
	  Gaudi::XYZVectorF trkdir(refstate.tx(),refstate.ty(),1) ;
	  BOOST_FOREACH( const Tf::STHit* hit, region->hits() ) {
	    double ytrk = refstate.y() + ( hit->zMid() - refstate.z() ) * refstate.ty() ;
	    if( hit->isYCompatible(ytrk,20) ) {
	      // Juan still has something odd here, so I'll just compute it myself then ...
	      //double distance = Gaudi::Math::distance( trkline, **ihit ) ;
	      Gaudi::XYZPointF hitpoint = hit->beginPoint() ;
	      Gaudi::XYZVectorF hitdir = hit->direction() ;
	      Gaudi::XYZVectorF projectiondir = hitdir.Cross( trkdir ).Unit() ;
	      Gaudi::XYZVectorF delta = hitpoint-trkpoint ;
	      double distance = delta.Dot(projectiondir) ;
	      if ( std::abs(distance) < m_maxDistance ) {
		LHCb::STChannelID stid = hit->lhcbID().stID() ;
		DeltaHistogrammer* histos = getHistogrammer( stid.station()-1,
							     stid.detRegion()-1,
							     tracktype ) ;
		// unbias if necessary
		bool isOnTrack = track.isOnTrack( hit->lhcbID() ) ;
		if( isOnTrack ) {
		  // find the corresponding node
		  const LHCb::Node* node(0) ;
		  for( LHCb::TrackFitResult::NodeContainer::const_iterator
			 inode = track.fitResult()->nodes().begin(); 
		       node==0 && inode != track.fitResult()->nodes().end(); ++inode)
		    if( (*inode)->type() == LHCb::Node::HitOnTrack &&
			(*inode)->measurement().lhcbID() ==  hit->lhcbID() ) 
		      node = *inode ;
		  if(node) {
		    const LHCb::FitNode* fitnode = dynamic_cast<const LHCb::FitNode*>(node) ;
		    const LHCb::STMeasurement* stmeas =  dynamic_cast<const LHCb::STMeasurement*>(&(node->measurement())) ;
		    distance *=  node->errMeasure2() / node->errResidual2() ;
		    if( false && tracktype == 4 && stmeas) {
		      std::cout << "*************************************" << std::endl ;
		      std::cout << "dist/res: " << distance << " "
				<< fitnode->unbiasedResidual() << std::endl ;
		      std::cout << "refstate: " << refstate << std::endl ;
		      std::cout << "nodestate: " << node->state().z() << " " << node->state().stateVector() << std::endl ;
		      std::cout << "dx: " << refstate.x() + refstate.tx() * ( node->state().z() - refstate.z() ) - node->state().x() << std::endl ;
		      std::cout << "dy: " << refstate.y() + refstate.ty() * ( node->state().z() - refstate.z() ) - node->state().y() << std::endl ;
		      
		      Gaudi::XYZPointF hitbegin =  hitpoint + hitdir * hit->yBegin() ;
		      std::cout << "hit: " << hitbegin << " " << hitdir << std::endl ;
		      Gaudi::XYZPoint trajbegin = node->measurement().trajectory().beginPoint() ;
		      std::cout << "meas: " << trajbegin << " "
				<< node->measurement().trajectory().endPoint() << " "
				<< std::endl ;
		      
		      Gaudi::XYZPointF begin = hitpoint + node->measurement().trajectory().beginPoint().y() * hitdir ;
		      std::cout << "coor diff: " 
				<< begin.x() - trajbegin.x() << "," 
				<< begin.y() - trajbegin.y() << ","
				<< begin.z() - trajbegin.z() << std::endl ;
		      
		      std::cout << "hit strip: "
				<< hit->cluster().channelID().strip() << " "
				<< hit->cluster().interStripFraction() << std::endl ;
		      std::cout << "meas strip: "
				<< stmeas->cluster()->channelID().strip() << " "
				<< stmeas->cluster()->interStripFraction() << std::endl ;
		      const DeSTSector& sector = hit->sector() ;
		      int strip  = hit->cluster().channelID().strip() ;
		      double offset = hit->cluster().interStripFraction() ;
		      double dxdy, dzdy, xAtYEq0, zAtYEq0, ybegin, yend ;

		      // let's update the cache explicitely
		      //(const_cast<DeSTSector&>(sector)).cacheInfo() ;
		      sector.trajectory(strip,offset,dxdy,dzdy,xAtYEq0,zAtYEq0,ybegin,yend) ;
		      std::cout << "from fast traj: (" << xAtYEq0+ybegin*dxdy << ","
				<< ybegin << "," <<  zAtYEq0+ybegin*dzdy << std::endl ;
		      std::auto_ptr<LHCb::Trajectory> newtraj = sector.trajectory(stid,offset) ;
		      std::cout << "newtraj begin: " << newtraj->beginPoint() << std::endl ;
		    }
		  }
		}
		int sign = hit->yBegin() < hit->yEnd() ? 1 : -1 ;
		histos->resh1->fill( sign * distance ) ;
		if( isOnTrack ) histos->hotresh1->fill( sign * distance ) ;
	      }
	    }
	    ++numcompatible ; 
	  }
	}
      }
    }
  }
  return numcompatible ;
}

size_t TrackITOverlapMonitor::accumulate( const Tf::STHitRange& hits,
                                          const LHCb::State& refstate,
                                          const LHCb::Track& track)
{
  size_t numcompatible(0) ;   
  Gaudi::XYZPointF trkpoint(refstate.x(),refstate.y(),refstate.z()) ;
  Gaudi::XYZVectorF trkdir(refstate.tx(),refstate.ty(),1) ;
  //Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> trkline( refstate.position(), refstate.slopes() ) ;
  int tracktype = trackType(track) ;
  if( tracktype>=0 ) {
    for( Tf::STHitRange::const_iterator ihit = hits.begin() ; ihit != hits.end(); ++ihit) {
      const Tf::STHit* hit = *ihit ;
      double ytrk = refstate.y() + ( hit->zMid() - refstate.z() ) * refstate.ty() ;
      if( hit->isYCompatible(ytrk,20) ) {
	// Juan still has something odd here, so I'll just compute it myself then ...
	//double distance = Gaudi::Math::distance( trkline, **ihit ) ;
	Gaudi::XYZPointF hitpoint = hit->beginPoint() ;
	Gaudi::XYZVectorF hitdir = hit->direction() ;
	Gaudi::XYZVectorF projectiondir = hitdir.Cross( trkdir ).Unit() ;
	Gaudi::XYZVectorF delta = hitpoint-trkpoint ;
	double distance = delta.Dot(projectiondir) ;
	if ( std::abs(distance) < m_maxDistance ) {
	  LHCb::STChannelID stid = hit->lhcbID().stID() ;
	  DeltaHistogrammer* histos = getHistogrammer( stid.station()-1,
						       stid.detRegion()-1,
						       tracktype ) ;
	  int sign = hit->yBegin() < hit->yEnd() ? 1 : -1 ;
	  histos->resh1->fill( sign * distance ) ;
	  if(track.isOnTrack( hit->lhcbID() ) )
	    histos->hotresh1->fill( sign * distance ) ;
	}
      }
      ++numcompatible ; 
    }
  }
  return numcompatible ;
}

StatusCode TrackITOverlapMonitor::execute() 
{
  // Get tracks.
  LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_trackLocation);

  // Get the TT hits
  Tf::STHitRange ithits = m_ithitcreator->hits() ;

  // now loop over all tracks, create a line from the last state, loop
  // over all hits, etc
  BOOST_FOREACH( const LHCb::Track* track, tracks) {
    if( track->nStates() > 0 ) {
      accumulate( *track ) ;
    } else {
      warning() << "Track has no states" << endreq ;
    }
  }
  
  return StatusCode::SUCCESS ;
}
