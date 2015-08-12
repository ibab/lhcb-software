// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITrackManipulator.h"

// event
#include "Event/Track.h"
#include "Event/TrackFitResult.h"
#include "Event/FitNode.h"
#include "Event/OTMeasurement.h"
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

namespace {
  struct OTHit
  {
    OTHit() : node(0),meas(0),projectedX(0),active(true) {}
    LHCb::OTChannelID otid ;
    const LHCb::Node* node ;
    const LHCb::OTMeasurement* meas ;
    double projectedX ;
    bool active ;
  } ;
  
  struct OTCluster : public StaticArray<OTHit,16>
  {
    unsigned int numRemoved ;
    unsigned int numLRSignsSet ;
    OTCluster() : numRemoved(0), numLRSignsSet(0) {}
  } ;
}

/******************************************************************************/
// Tool to resolve subset of LR signs by exploiting the shape of a
// cluster. Sets the value of ambiguity to +/-2. The absolute value
// '2' signals to the projector that the ambiguity is fixed.
/******************************************************************************/

class TrackOTLRSignTool: public GaudiTool, virtual public ITrackManipulator
{
public:
  // Constructors and destructor
  TrackOTLRSignTool(const std::string& type,const std::string& name,const IInterface* parent);
  virtual StatusCode execute(LHCb::Track& aTrack) const ;
  virtual StatusCode initialize() ;
  virtual StatusCode finalize() ;
private:
  size_t removeHitsInLayer( OTCluster& ) const ;
  size_t processHitsInLayer( OTCluster& ) const ;
private:
  const DeOTDetector* m_otdet ;
  bool m_removeOutliers ;
  bool m_printCounters ;
};

DECLARE_TOOL_FACTORY(TrackOTLRSignTool)

TrackOTLRSignTool::TrackOTLRSignTool(const std::string& type,
				     const std::string& name,
				     const IInterface* parent):
  GaudiTool(type, name, parent),
  m_otdet(0)
{
  // constructor
  declareInterface<ITrackManipulator>(this);
  declareProperty("RemoveOutliers",m_removeOutliers = true ) ;
  declareProperty("PrintCounters",m_printCounters = false ) ;
}

StatusCode TrackOTLRSignTool::initialize()
{  
  StatusCode sc = GaudiTool::initialize();
  m_otdet = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
  return sc ;
}

size_t TrackOTLRSignTool::removeHitsInLayer(OTCluster& othits) const
{
  // get the minimum and maximum X in the projected range
  int minX(-1),maxX(-1) ;
  double sumX(0) ;
  size_t nActive = (0) ;
  size_t N = othits.size() ;
  for(size_t i=0; i<N; ++i)
    if( othits[i].active ) {
      // node that this uses the reference, but that should be fine
      double x = othits[i].projectedX ;
      if( minX<0 || othits[minX].projectedX > x) minX = i ;
      if( maxX<0 || othits[maxX].projectedX < x) maxX = i ;
      sumX += x ;
      ++nActive ;
    }
  if(nActive>1) {
    //in the projected plane, we cannot have a single coordinate more
    //than 2.5mm away. that means that the difference between minX and
    //maxX shouldn't be bigger than 5mm
    if( othits[maxX].projectedX - othits[minX].projectedX > 5*Gaudi::Units::mm ) {
      if( nActive==2 ) {
	// flag them both inactive. we will not set the LR sign.
	othits[maxX].active = othits[minX].active = false ;
	othits.numRemoved +=2 ;
      } else {
	double avX = sumX/nActive ;
	int hitToRemove = 
	  std::abs(othits[minX].projectedX - avX ) > std::abs(othits[maxX].projectedX - avX ) ? minX : maxX ;
	othits[hitToRemove].active = false ;
	othits.numRemoved += 1 ;
	if( nActive-1 >=2 ) removeHitsInLayer(othits) ;
      }
    } else {
      // all hits are fine. now set the LR signs. for now,  only do two of them.
      if( othits[maxX].projectedX - othits[minX].projectedX > 2.5*Gaudi::Units::mm ) {
	// need to correct for direction of trajectory. note that this
	// depends on definition in projector.
	int maxXSign =  1 * ( othits[maxX].meas->trajectory().direction(0).y()>0 ? 1 : -1 ) ;
	int minXSign = -1 * ( othits[minX].meas->trajectory().direction(0).y()>0 ? 1 : -1 ) ;
	// Leave this for debugging: whener we change something in
	// signs, we want to make sure we use consistent signs all
	// over.
	// info() << "LRSign before/after max: " 
	//        << maxXSign << " " << othits[maxX].meas->ambiguity() 
	//        << " min : " 
	//        << minXSign << " " << othits[minX].meas->ambiguity() << endmsg ;
	// if( maxXSign * othits[maxX].meas->ambiguity() < 0 ||
	//     minXSign * othits[minX].meas->ambiguity() <0 ) {
	//   const LHCb::FitNode* minNode = dynamic_cast<const LHCb::FitNode*>(othits[minX].node) ;
	//   const LHCb::FitNode* maxNode = dynamic_cast<const LHCb::FitNode*>(othits[maxX].node) ;
	//   std::cout << "fit doesn't quite agree: "
	// 	    << othits[minX].projectedX << " " << othits[maxX].projectedX << std::endl
	// 	    << minNode->doca() << " " << maxNode->doca() << std::endl
	// 	    << othits[minX].meas->trajectory().direction(0).y() << " "
	// 	    << othits[maxX].meas->trajectory().direction(0).y() << std::endl ;
	// }

	// set the ambiguities. the '2' signals to the projector that the ambiguity is fixed.
	const_cast<LHCb::OTMeasurement*>(othits[maxX].meas)->setAmbiguity( 2*maxXSign ) ;
	const_cast<LHCb::OTMeasurement*>(othits[minX].meas)->setAmbiguity( 2*minXSign ) ;
	othits.numLRSignsSet += 2 ;
      }
    }
  }
  return othits.numRemoved ;
}

size_t TrackOTLRSignTool::processHitsInLayer( OTCluster& cluster ) const
{
  size_t rc = 0  ;
  size_t N = cluster.size() ;
  if(N>=2) {  
    // get a reference
    const LHCb::StateVector& refstate = cluster.front().node->refVector() ;
    Gaudi::XYZVector dref(refstate.tx(),refstate.ty(),1) ;
    Gaudi::XYZPoint pref(refstate.x(),refstate.y(),refstate.z()) ;    // fill the residuals
  
    // project the x-coordinates in a single plane perpendicular to
    // the slope. if the track is fitted, all we need is 'doca', I think
    for(size_t i=0; i<N; ++i) {
      OTHit& hit = cluster[i] ;
      hit.active = true ;
      // we could also use the meas->trajectory here, which may be
      // faster (though not by much). the most important thing is that
      // the straws all point in the same direction, otherwise we get
      // trouble with signs. not also that you need to adjust the sign
      // again when setting the ambiguity.
      const DeOTModule* module = m_otdet->findModule(hit.otid) ;
      double dxdy, dzdy, xAtYEq0, zAtYEq0, ybegin,  yend ;
      module->trajectory( hit.otid.straw(), dxdy, dzdy, xAtYEq0, zAtYEq0, ybegin,  yend) ;
      Gaudi::XYZVector dhit(dxdy,1,dzdy) ;
      Gaudi::XYZPoint  phit(xAtYEq0,0,zAtYEq0) ;
      Gaudi::XYZVector proj = dref.Cross( dhit ) ;
      hit.projectedX = (phit - pref).Dot( proj ) / proj.R() ;
    }
    
    // remove outlying hits and set LR signs
    rc = removeHitsInLayer(cluster) ;
    // deactivate the nodes, but only for clusters of size larger than 2 (otherwise we cannto choose)
    if(rc>0 && m_removeOutliers && cluster.size()>2) {
      for(size_t i=0; i<N; ++i) 
	if( !cluster[i].active) 
	  (const_cast<LHCb::Node*>(cluster[i].node))->setType(LHCb::Node::Outlier) ;
    }
  }
  return rc ;
}

StatusCode TrackOTLRSignTool::execute(LHCb::Track& track) const
{
  // first identify all OT nodes. don't forget to set the LR sign to zero
  LHCb::TrackFitResult* fr = track.fitResult() ;
  if( ! fr ) {
    warning() << "Cannot find a fit result on track" << endmsg ;
    return StatusCode::FAILURE;
  } else {
    // now there are two options. if the track was fitted, we'll run
    // over nodes and disable any node in a cluster. if the track was
    // not yet fitted, we'll remove lhcbids 
    const int numUniqueLayers=12 ;
    // sort the nodes by unique layer
    int nremoved(0), nset(0), nall(0) ;
    typedef std::vector< OTCluster > OTClusterContainer ;
    OTClusterContainer nodesByLayer(numUniqueLayers) ;
    OTHit othit ;
    const LHCb::TrackFitResult::NodeContainer& nodes = fr->nodes() ;
    for( LHCb::TrackFitResult::NodeContainer::const_iterator it = nodes.begin() ;
	 it != nodes.end() ; ++it ) {
      const LHCb::Node* node = *it ;  
      if( node->type()==LHCb::Node::HitOnTrack &&
	  node->measurement().type()==LHCb::Measurement::OT ) {
	const LHCb::OTMeasurement* meas = static_cast<LHCb::OTMeasurement*>(&(node->measurement())) ;
	LHCb::OTChannelID otid = meas->lhcbID().otID() ;
	unsigned int uniquelayer = (otid.station()-1)*4 + otid.layer() ;
	othit.node = node ;
	othit.meas = meas ;
	othit.otid = otid ;
	nodesByLayer[ uniquelayer ].push_back( othit ) ;
	// make sure to 'release' any ambiguity that was already set
	if( std::abs( meas->ambiguity() ) > 1 )
	  const_cast<LHCb::OTMeasurement*>(meas)->setAmbiguity( meas->ambiguity() >0 ? 1 : -1 ) ;
      }
    }
    
    // now loop over the layers, assign LR signs and identify problematic clusters
    for( OTClusterContainer::iterator iclus = nodesByLayer.begin() ;
	 iclus != nodesByLayer.end(); ++iclus ) {
      int N = iclus->size() ;
      if(N>0) {
	processHitsInLayer(*iclus) ;
	nremoved += iclus->numRemoved ;
	nset += iclus->numLRSignsSet ;
	nall += N ;
	//lot2D( N, n, "number of removed hits vs n hits", -0.5, 6.5, -0.5, 6.5, 7, 7) ;
      }
    }
    if(nall>0 && m_printCounters) {
      counter("Fraction of removed OT hits") += nremoved/double(nall) ;
      counter("Fraction of set LR signs") += nset/double(nall) ;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TrackOTLRSignTool::finalize()
{  
  if( m_printCounters ) {
    info() << counter("Average number of removed OT hits") << endmsg ;
    info() << counter("Fraction of set LR signs") << endmsg ;
  }
  return GaudiTool::finalize();
}
