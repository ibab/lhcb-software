// $Id: TrackRemoveOddOTClusters.cpp,v 1.2 2009-10-09 10:35:59 wouter Exp $
// Include files


// from Gaudi
#include <GaudiAlg/GaudiHistoAlg.h>
#include "GaudiKernel/AlgFactory.h"

// Event
#include <Event/Track.h>
#include <Event/FitNode.h>
#include <Event/OTMeasurement.h>
#include <boost/foreach.hpp>

// local
class TrackRemoveOddOTClusters : public GaudiHistoAlg
{
public: 
  TrackRemoveOddOTClusters(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~TrackRemoveOddOTClusters( ); ///< Destructor
  StatusCode execute();
private:
  std::string m_trackLocation ;
};

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackRemoveOddOTClusters )

//=============================================================================
TrackRemoveOddOTClusters::TrackRemoveOddOTClusters(const std::string& name,
                             ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty("TrackLocation", m_trackLocation = LHCb::TrackLocation::Default) ;
}

//=============================================================================
TrackRemoveOddOTClusters::~TrackRemoveOddOTClusters()
{
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode TrackRemoveOddOTClusters::execute()
{
   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

   // Get the Tracks.
   LHCb::Tracks* tracks = get<LHCb::Tracks>(m_trackLocation);
   for (LHCb::Tracks::iterator itrk = tracks->begin(); itrk != tracks->end(); ++itrk) {

     // loop over the nodes, sort hits by monom layer. if there is
     // more than one hit in a monolayer, take only the closest and
     // all other hits that are within one cell-radius.
     
     std::map<int, std::vector< LHCb::FitNode* > > nodesInMono ;
     BOOST_FOREACH( const LHCb::Node* node, (*itrk)->nodes()) 
       if( node->type() == LHCb::Node::HitOnTrack &&
	   node->measurement().type() == LHCb::Measurement::OT ) {
	 // unique mono-layer = mono + 2 * ( layer + 4 * (station-1) )
	 const LHCb::OTMeasurement* meas = dynamic_cast<const LHCb::OTMeasurement*>(&(node->measurement())) ;
	 const DeOTModule& mod = meas->module() ;
	 const LHCb::OTChannelID otchan = meas->channel() ;
	 int mono = mod.monoLayerA(otchan.straw()) ? 0 : 1 ;
	 size_t uniquemono = mono + 2 * ( otchan.layer() + 4 * (otchan.station() - 1) ) ;
	 nodesInMono[uniquemono].push_back( const_cast<LHCb::FitNode*>(dynamic_cast<const LHCb::FitNode*>(node))) ;
       }
     for( std::map<int, std::vector< LHCb::FitNode* > >::iterator
	    imono = nodesInMono.begin() ; imono != nodesInMono.end(); ++imono ) {
       std::vector< LHCb::FitNode* >& nodes = imono->second ;
       plot1D(nodes.size(),"clustersize","cluster size",0.5,10.5,10) ;
       profile1D((*itrk)->firstState().tx(),nodes.size(),"clustersizeVsTx","cluster size versus Tx",-1,1,20) ;
       if( nodes.size() > 1 ) {
	 LHCb::FitNode* bestnode(0) ;
	 for( std::vector< LHCb::FitNode* >::iterator inode = nodes.begin() ;
	      inode != nodes.end() ; ++inode) {
	   if( bestnode==0 || std::abs((*inode)->residual()) < std::abs(bestnode->residual() ) ) 
	     bestnode = *inode ;
	   const double cellradius = 2.45 ;
	   if( std::abs((*inode)->residual())  > cellradius ) (*inode)->setType( LHCb::Node::Outlier) ;
	 }
	 bestnode->setType( LHCb::Node::HitOnTrack ) ;
       }
     }
   }
   return StatusCode::SUCCESS;
}

