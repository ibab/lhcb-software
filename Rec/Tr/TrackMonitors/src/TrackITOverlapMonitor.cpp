// Include files

// from std
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/Node.h"

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "TfKernel/IITHitCreator.h"
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/Line.h"

#include "TrackKernel/TrackTraj.h"

#include <boost/foreach.hpp>

namespace Al {
  class Equations;
  class Residuals ;
}

/** @class TrackITOverlapMonitor TrackITOverlapMonitor.cpp
*
* Algorithm to monitor overlap between IT boxes and between IT and OT
*
*  @author Wouter Hulsbergen
*  @date   2008-09-07
*/

namespace {
  struct DeltaHistogrammer
  {
    AIDA::IHistogram1D* resh1 ;
    AIDA::IHistogram1D* hotresh1 ;
    AIDA::IHistogram1D* dyh1 ;
    AIDA::IHistogram1D* hotdyh1 ;
    AIDA::IHistogram2D* resVsLayerH1 ;
    AIDA::IHistogram2D* resVsLadderH1 ;
    
    DeltaHistogrammer( const std::string& tracktype, 
		       const std::string& boxtype,
		       GaudiHistoAlg& parent ) 
    {
      resh1 = parent.book1D(tracktype + "/" + boxtype + "/hitres", boxtype + " dx", -5,5,100) ;
      hotresh1 = parent.book1D(tracktype + "/" + boxtype + "/hotres", boxtype + " dx", -5,5,100) ;
      dyh1 = parent.book1D(tracktype + "/" + boxtype + "/hity", 
			   boxtype + " y position wrt to sensor edge", -50,250,300) ;
      hotdyh1 = parent.book1D(tracktype + "/" + boxtype + "/hoty", 
			      boxtype + " hot y position wrt to sensor edge", -50,250,300) ;
      resVsLayerH1 = parent.book2D(tracktype + "/" + boxtype + "/hitresVsLayer", boxtype + " dx",
				   0.5,4.5,4,-5,5,100) ;
      resVsLadderH1 = parent.book2D(tracktype + "/" + boxtype + "/hitresVsLadder", boxtype + " dx",
				    0.5,28.5,28,-5,5,100) ;
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
  enum TrackType {OTASide=0, OTCSide, ITASide, ITCSide, ITTop, ITBottom, OtherTTrack, NoTTrack } ;
  int trackType( const LHCb::Track& track ) const ;
  DeltaHistogrammer* getHistogrammer( size_t itstation,
				      size_t itbox,
				      size_t tracktype ) {
    size_t id = itbox + 4* (itstation + 8*tracktype ) ;
    if(m_histograms.size()<id+1 || m_histograms[id]==0) {
      if(m_histograms.size()<id+1) m_histograms.resize(id+1) ;
      const char stations[3][256] = { "IT1","IT2","IT3" } ;
      const char boxes[4][256]    = { "CSideBox","ASideBox","BottomBox","TopBox"} ;
      const char types[7][256]    = { "OTASideTrack", "OTCSideTrack", "ITASideTrack", "ITCSideTrack", "ITTopTrack", "ITBottomTrack", "Other" } ;
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
  AIDA::IHistogram1D* m_tracktypeh1 ;
  AIDA::IHistogram1D* m_numregionsh1 ;
  ToolHandle<Tf::IITHitCreator> m_ithitcreator ;
  std::string m_trackLocation ;
  double m_maxDistance ;
  double m_maxTolY ;
  double m_maxTrackError ;
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackITOverlapMonitor )

TrackITOverlapMonitor::TrackITOverlapMonitor( const std::string& name,
				ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_tracktypeh1(0),
    m_numregionsh1(0),
    m_ithitcreator("Tf::STHitCreator<Tf::IT>/ITHitCreator"),
    m_maxDistance(10.* Gaudi::Units::mm ),
    m_maxTrackError(1.0 * Gaudi::Units::mm )
{
  declareProperty("TrackLocation", m_trackLocation = LHCb::TrackLocation::Default  ); 
  declareProperty("MaxTolY", m_maxTolY = 50.0 ) ;
}

TrackITOverlapMonitor::~TrackITOverlapMonitor()
{
  BOOST_FOREACH( DeltaHistogrammer* h, m_histograms ) {
    delete h ;
  }
  m_histograms.clear() ;
}


StatusCode TrackITOverlapMonitor::initialize() {
  if ( !GaudiHistoAlg::initialize().isSuccess() ) return StatusCode::FAILURE ;
  
  if(!m_ithitcreator.retrieve().isSuccess()) 
    return Error("==> Failed to retrieve IT hit creator",StatusCode::FAILURE); 
  setHistoTopDir("Track/");
  m_tracktypeh1 = book1D("tracktype","tracktype",-0.5,7.5,8) ;
  m_numregionsh1 = book1D("numregions","number of traversed regions",-0.5,24.5,25) ;

  return StatusCode::SUCCESS;
}

StatusCode TrackITOverlapMonitor::finalize() 
{
  m_ithitcreator.release().ignore() ;
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
  int rc = NoTTrack ;
  if(      numIT[0] >= 8 ) rc = ITCSide ;
  else if( numIT[1] >= 8 ) rc = ITASide ;
  else if( numIT[2] >= 8 ) rc = ITBottom ;
  else if( numIT[3] >= 8 ) rc = ITTop ;
  else if( numOT[0] >= 16 ) rc = OTASide ;
  else if( numOT[1] >= 16 ) rc = OTCSide ;
  else if( numIT[0] +  numIT[1] + numIT[2] +  numIT[3] + numOT[0] +  numOT[1] >= 8 ) rc = OtherTTrack ;
  return rc ;
}


StatusCode TrackITOverlapMonitor::execute() 
{
  // Get tracks.
  LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_trackLocation);

  // For some reason this doesn't work if I don't just first get the
  // hits. This must be a bug in the STHitCreator.
  m_ithitcreator->hits() ;
    
  // loop over all tracks
  BOOST_FOREACH( const LHCb::Track* track, tracks) {

    // get the nodes for later use
    LHCb::Track::ConstNodeRange nodes = track->nodes() ;

    // make sure that there are sufficient active nodes behind the
    // magnet. we want at least 8 active nodes.
    size_t numnodes(0) ;
    BOOST_FOREACH( const LHCb::Node* node, nodes) 
      if(node->type() == LHCb::Node::HitOnTrack) {
	double z = node->z() ;
	if( 7500 < z && z < 9500 ) ++numnodes ;
      }

    // get the track type.
    int tracktype = trackType(*track) ;
    m_tracktypeh1->fill( tracktype ) ;
    
    // only process tracks that are classified.
    if( tracktype < NoTTrack && numnodes >= 8 ) {

      // create a tracktraj
      LHCb::TrackTraj tracktraj(*track) ;

      // keep some statistics about how many regions the track crosses
      size_t numtraversedregions(0), numcompatible(0) ;

      // loop over all regions
      for(size_t istation=0; istation<3; ++istation)
	for(size_t ilayer=0; ilayer<4; ++ilayer)
	  for(size_t iregion=0; iregion<4;++iregion) {

	    const Tf::IITHitCreator::STRegion* region = 
	      m_ithitcreator->region( istation,ilayer,iregion ) ;
	    // here we could significantly speed up things by first
	    // testing that the track goes through the region.

	    double regionz = 0.5*(region->zmin() + region->zmax() ) ;
	    LHCb::State refstate = tracktraj.state( regionz ) ;
	    // make sure the track is reasonably well known
	    if( refstate.covariance()(0,0) < m_maxTrackError * m_maxTrackError ) {

	      double statex = refstate.x() + (regionz - refstate.z())*refstate.tx() ;
	      double statey = refstate.y() + (regionz - refstate.z())*refstate.ty() ;
	      double tolx = 10*m_maxDistance/region->cosT() ;
	      double toly = m_maxTolY ;

	      if( region->isXCompatible( statex, tolx ) &&
		  region->isYCompatible( statey, toly ) ) {

		// update the number of traversed regions
		++numtraversedregions ;

		// most efficient: use local x-coordinate. hope this works:-)
		double localx = region->xLocal( statex, statey ) ;
		Tf::STHitRange thesehits = m_ithitcreator->hitsLocalXRange( istation, ilayer, iregion, 
									    localx -  tolx,
									    localx +  tolx) ;
		Gaudi::XYZPointF trkpoint((float) refstate.x(),(float) refstate.y(),(float) refstate.z()) ;
		Gaudi::XYZVectorF trkdir((float) refstate.tx(),(float) refstate.ty(),1) ;
	    
		// now loop over all hits
		BOOST_FOREACH( const Tf::STHit* hit, thesehits ) {
		  double ytrk = refstate.y() + ( hit->zMid() - refstate.z() ) * refstate.ty() ;
		  // this window is a bit large
		  if( hit->isYCompatible(ytrk,m_maxTolY) ) {
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
		      bool isOnTrack = track->isOnTrack( hit->lhcbID() ) ;
		      if( isOnTrack ) {
			// find the corresponding node and unbias the distance
			for( LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin(); 
			     inode != nodes.end(); ++inode)
			  if( (*inode)->type() == LHCb::Node::HitOnTrack &&
			      (*inode)->measurement().lhcbID() ==  hit->lhcbID() ) {
			    distance *=  (*inode)->errMeasure2() / (*inode)->errResidual2() ;
			    break ;
			  }
		      }
		      // make sure all contributions get the same
		      // sign. (some IT layers are rotated)
		      int sign = hit->yBegin() < hit->yEnd() ? 1 : -1 ;
		      double dy = ytrk - std::min(hit->yBegin(),hit->yEnd())  ;
		      histos->resh1->fill( sign * distance ) ;
		      histos->resVsLayerH1->fill( stid.layer(), sign * distance ) ;
		      histos->resVsLadderH1->fill( (stid.layer()-1)*7 + stid.sector(), sign * distance ) ;
		      histos->dyh1->fill( dy ) ;
		      if( isOnTrack ) {
			histos->hotresh1->fill( sign * distance ) ;
			histos->hotdyh1->fill( dy ) ;
		      }
		      ++numcompatible ;
		    }
		  }
		}
	      }
	    }
	  }
      m_numregionsh1->fill(numtraversedregions) ;
    }
  }
  return StatusCode::SUCCESS ;
}
