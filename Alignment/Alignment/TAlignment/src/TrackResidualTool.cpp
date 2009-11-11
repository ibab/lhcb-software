
#include <map>
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiAlg/GaudiTool.h"
#include "LHCbMath/MatrixInversion.h"
#include "ITrackResidualTool.h"
#include "IGetElementsToBeAligned.h"
#include "TrackInterfaces/ITrackKalmanFilter.h"
#include "Event/TrackFitResult.h"

namespace Al
{
  
  class TrackResidualTool : public GaudiTool,
			    virtual public ITrackResidualTool,
			    virtual public IIncidentListener
  {
  public:
    // constructor
    TrackResidualTool(const std::string& type,const std::string& name,const IInterface* parent);
    // destructor
    virtual ~TrackResidualTool() {} ;
    // initialisation
    StatusCode initialize() ;
    // finalize
    StatusCode finalize() ;
    // incident service handle
    void handle( const Incident& incident ) ;
    // get the residuals for this track
    const Al::TrackResiduals* get(const LHCb::Track& track) const ;

  private:  
    const Al::TrackResiduals* compute(const LHCb::Track& track) const ;
    Al::TrackResiduals* create( const LHCb::Track& track, 
				const std::vector<const LHCb::Node*>& nodes,
				std::vector<size_t>& residualnodeindices,
				size_t& refnodeindex ) const ;
  private:
    typedef std::map<const LHCb::Track*, const Al::TrackResiduals*> ResidualMap ;
    mutable ResidualMap m_residuals ;
    ToolHandle<IGetElementsToBeAligned> m_elementTool ;
    ToolHandle<ITrackKalmanFilter> m_kalmanFilter ;
  } ;
  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
  
#include <iostream>
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "Event/FitNode.h"
#include "Event/Track.h"

  
namespace Al
{
  
  DECLARE_TOOL_FACTORY( TrackResidualTool );
  
  
  TrackResidualTool::TrackResidualTool(const std::string& type,
				       const std::string& name,
				       const IInterface* parent)
    : GaudiTool(type,name,parent),
      m_elementTool("GetElementsToBeAligned"),
      m_kalmanFilter("TrackKalmanFilter",this)
  {
    // interfaces
    declareInterface<ITrackResidualTool>(this);
    declareProperty("KalmanFilter",m_kalmanFilter) ;
  }
  
  StatusCode TrackResidualTool::initialize()
  {
    StatusCode sc = GaudiTool::initialize();
    if (sc.isFailure()) return Error("Failed to initialize",sc);
    sc = m_elementTool.retrieve() ;
    incSvc()->addListener(this, IncidentType::EndEvent);
    sc = m_kalmanFilter.retrieve() ;
    return sc ;
  }
  
  StatusCode TrackResidualTool::finalize()
  {
    m_elementTool.release().ignore() ;
    return GaudiTool::finalize();
  }

  void TrackResidualTool::handle( const Incident& incident )
  {
    if ( IncidentType::EndEvent == incident.type() ) {
      for( ResidualMap::iterator it = m_residuals.begin() ;
	   it != m_residuals.end(); ++it)
	delete it->second ;
      m_residuals.clear() ;
    }
  }
  
  const Al::TrackResiduals* TrackResidualTool::get(const LHCb::Track& track) const
  {
    const Al::TrackResiduals* rc(0) ;
    ResidualMap::const_iterator it = m_residuals.find( & track ) ;
    if( it != m_residuals.end() ) 
      rc = it->second ;
    else {
      rc = compute( track ) ;
      m_residuals[&track] = rc ;
    }
    return rc ;
  }

  Al::TrackResiduals* TrackResidualTool::create( const LHCb::Track& track, 
						 const std::vector<const LHCb::Node*>& nodes,
						 std::vector<size_t>& residualnodeindices,
						 size_t& refnodeindex ) const
  {
    // first select nodes with a measurement and equip them with an index in the original list
    typedef std::pair<const LHCb::Node*, size_t> NodeWithIndex ;
    std::vector< NodeWithIndex > nodeswithindex ;
    nodeswithindex.reserve( nodes.size()) ;
    size_t index(0) ;
    for( std::vector<const LHCb::Node*>::const_iterator it = nodes.begin();
	 it != nodes.end(); ++it, ++index )
      if( (*it)->hasMeasurement() && (*it)->type() != LHCb::Node::Outlier ) 
	nodeswithindex.push_back( NodeWithIndex(*it,index) ) ;

    // now make sure the order is right for eventual vertexing
    bool backward = track.checkFlag(LHCb::Track::Backward) ;
    bool increasingz = nodes.front()->z() < nodes.back()->z() ;
    if( (backward && increasingz) || (!backward && !increasingz))
      std::reverse( nodeswithindex.begin(), nodeswithindex.end() ) ;

    // the first node is the reference for vertexing
    refnodeindex = nodeswithindex.front().second ;

    // now select the subset of nodes associated to an alignable object
    residualnodeindices.clear() ;
    residualnodeindices.reserve( nodeswithindex.size() ) ;
    Al::TrackResiduals* rc = new Al::TrackResiduals(track) ;
    rc->m_residuals.reserve( nodeswithindex.size() ) ;
    rc->m_state = nodeswithindex.front().first->state() ;
    for( std::vector< NodeWithIndex >::const_iterator it = nodeswithindex.begin() ;
	 it != nodeswithindex.end(); ++it ) {
      const LHCb::Node* node = it->first ;
      const AlignmentElement* elem = m_elementTool->findElement(node->measurement()) ;
      if( elem ) { 
	rc->m_residuals.push_back( Al::Residual1D( *node, *elem) ) ;
	residualnodeindices.push_back( it->second ) ;
      } else {
	rc->m_nExternalHits++ ;
      }
    }
    
    return rc ;
  }

  const Gaudi::TrackMatrix& getOffDiagCov( size_t l,
					   size_t k,
					   std::vector< std::vector<Gaudi::TrackMatrix*> >& offdiagcov,
					   const std::vector< Gaudi::TrackMatrix >& smoothergainmatrix)
  {
    assert(k < l ) ;
    if( offdiagcov[l][k] == 0 ) 
      offdiagcov[l][k] = new Gaudi::TrackMatrix( getOffDiagCov(l,k+1,offdiagcov,smoothergainmatrix)
						 * Transpose(smoothergainmatrix[k]) ) ;
    return *offdiagcov[l][k] ;
  }
  
  const Al::TrackResiduals* TrackResidualTool::compute(const LHCb::Track& track) const
  {
    debug() << "In TrackResidualTool::Compute" << endreq ;

    // Call one iteration with the smoother gain if necessary
    if( track.fitHistory() != LHCb::Track::StdKalman ) {
      Warning("Track was fitted with bi-directional fit. Will refit with standard kalman on the fly.",
	      StatusCode::SUCCESS,0).ignore() ;
      m_kalmanFilter->fit( const_cast<LHCb::Track&>(track) ) ;
    }
    
    // ingredients
    // - smoothed states plus covariance
    // - upstream predicted covariance
    //
    // how we do this
    // - first calculatethe correlation for state i and state i+i
    // - then calculate all the other off-diagonal elements
    //
    // things to cache
    // - we definitely need to cache the inverse of the smoothed covariance for every node
    
    // first determine the nodes that represent the first and last
    // measurements. we don't care about the others.
    LHCb::Track::ConstNodeRange allnodes = track.nodes() ;
    LHCb::Track::ConstNodeRange::const_iterator begin = allnodes.end() ;
    LHCb::Track::ConstNodeRange::const_iterator end   = allnodes.begin() ;
    for( LHCb::Track::ConstNodeRange::const_iterator it = allnodes.begin() ;
	 it != allnodes.end(); ++it)
      if( (*it)->hasMeasurement() && (*it)->type() != LHCb::Node::Outlier ) {
	if( begin == allnodes.end() ) begin = it ;
	end = it ;
      }
    ++end ;
    
    // put all these nodes in a new container, to make things easier
    std::vector<const LHCb::Node*> nodes ;
    nodes.insert(nodes.end(),begin,end) ;
    
    size_t numnodes = nodes.size() ;
    
    // These are the diagonal elements. At first we just copy these
    std::vector< const Gaudi::TrackSymMatrix* > diagcov(numnodes,0) ;
    for( size_t irow = 0; irow<numnodes; ++irow)
      diagcov[irow] = &(nodes[irow]->state().covariance()) ;
    
    // These are the off-diagonal elements, which are not symmetric. We
    // need to choose whether we store lower or upper triangle. We are
    // going to define now that row >= col (lower triangle), which means
    // that we store the correlation Cov{k,k-1} etc.
    
    // To make things extra confusing, we also store the row with zero
    // elements. We do not need to store the diagonal.
    std::vector< std::vector<Gaudi::TrackMatrix*> > offdiagcov(numnodes)  ;
    for( size_t irow = 0; irow<numnodes; ++irow) {
      offdiagcov[irow] = std::vector<Gaudi::TrackMatrix*>(irow,0) ;
    }
    
    // We first compute the smoother gain matrix on every (but the
    // last) node. Once we have the gain matrices, the rest is
    // trivial, though still CPU intensive.

    bool error = false ;
    std::vector< Gaudi::TrackMatrix > smoothergainmatrix(numnodes-1) ;
    for(size_t k=numnodes-1; k>0 && !error; --k) {
      //std::cout << "k, size: " << k << " " << offdiagcov[k].size() << std::endl ;
      const LHCb::FitNode* node     = dynamic_cast<const LHCb::FitNode*>(nodes[k]) ;
      const LHCb::FitNode* prevnode = dynamic_cast<const LHCb::FitNode*>(nodes[k-1]) ;
      // do these _really_ correspond to 'prevnode'? how do we find out?
      assert( node && prevnode ) ;

      if( track.fitHistory() == LHCb::Track::StdKalman ) {
	// we have run the smoother. that makes things really easy.
	Warning("StdKalman trackfit: Using smoother gain matrix from track fit",StatusCode::SUCCESS,1) ;
	smoothergainmatrix[k-1] = prevnode->smootherGainMatrix() ;
	Gaudi::TrackMatrix C_km1_k = smoothergainmatrix[k-1] * node->state().covariance() ;
	offdiagcov[k][k-1] = new Gaudi::TrackMatrix(Transpose(C_km1_k)) ;
      } else {
	// we have run the bi-directional fit. that basically means we have to rerun the smoother
	Warning("BiDirectional trackfit: track not correctly fitted for TrackResidualTool.",
		StatusCode::FAILURE,100) ;
	return 0 ;
      }
    }

    // calculate the remaining elements in the covariance matrix. We
    // need to do this off-diagonal by off-diagonal. The easiest is
    // probably to do it on demand, but the following should also work. Note that
    // this is also where things become really slow.
    
    Al::TrackResiduals* residuals(0) ;

    if( !error ) {

      // FIX ME: replace this with an 'on-demand' computation
      //       for(size_t l=2; l<numnodes; ++l) {
      // 	for(size_t k=l-1; k>0; --k) {
      // 	  assert( offdiagcov[l][k] != 0 ) ;
      // 	  offdiagcov[l][k-1] = new Gaudi::TrackMatrix( (*(offdiagcov[l][k])) * Transpose(smoothergainmatrix[k-1]) ) ;
      // 	}
      //       }
    
      //       // for now, just check that every element is filled
      //       for( size_t irow = 1; irow<numnodes; ++irow) for(size_t icol = 0; icol<irow; ++icol)
      // 	if( offdiagcov[irow][icol] == 0 )
      // 	  std::cout << "element not filled: " << irow << " " << icol << " " << numnodes << std::endl ;
      
      // now it is time to calculate the covariance matrix for the
      // residuals. First we need to know which nodes correspond to
      // measurements. After that, we fill the matrix.

      // create the Al::Residuals object
      std::vector<size_t> measurementnodeindices ;
      size_t refnodeindex ;
      residuals = create( track,nodes,measurementnodeindices,refnodeindex ) ;
      
      // now create the 'sparce' matrix HCH
      size_t nummeasurements = measurementnodeindices.size() ;
      residuals->m_HCHElements.reserve( nummeasurements*(nummeasurements-1)/2) ;
      for(size_t irow=0; irow<nummeasurements; ++irow) {
	size_t k = measurementnodeindices[irow] ;
	assert(&(residuals->m_residuals[irow].node())==nodes[k]);
	const Gaudi::TrackProjectionMatrix& Hk = nodes[k]->projectionMatrix() ;
	// first the off-diagonal elements
	for(size_t icol=0; icol<irow; ++icol) {
	  size_t l = measurementnodeindices[icol] ;
	  assert(&(residuals->m_residuals[icol].node())==nodes[l]);
	  const Gaudi::TrackProjectionMatrix& Hl = nodes[l]->projectionMatrix() ;
	  double hch = l < k ? 
	    (Hk * getOffDiagCov(k,l,offdiagcov,smoothergainmatrix) * Transpose(Hl))(0,0) :
	    (Hl * getOffDiagCov(l,k,offdiagcov,smoothergainmatrix) * Transpose(Hk))(0,0) ;
	  residuals->m_HCHElements.push_back(CovarianceElement( irow, icol, hch ) ) ;
	}
	// now the diagonal element. we recompute this, just to make
	// sure it is consistent. (this turns out not to be necessary,
	// but anyway.)
	residuals->m_residuals[irow].setHCH( ROOT::Math::Similarity(Hk,*(diagcov[k]))(0,0) ) ;
	
	// and finally the column for the correlation with the reference state
	Gaudi::Matrix1x5 correlationmatrix ;
	if( refnodeindex == k )
	  correlationmatrix = Hk * *(diagcov[k]) ;
	else if( refnodeindex < k )
	  correlationmatrix = Hk * getOffDiagCov(k,refnodeindex,offdiagcov,smoothergainmatrix) ;
	else
	  correlationmatrix = Hk * Transpose(getOffDiagCov(refnodeindex,k,offdiagcov,smoothergainmatrix)) ;
	// now copy it into the trackresidual. need to be very carefull
	// with signs here. use H = - dres/dstate. in fact, this the
	// only place we need to know what the sign of the residual is.
	residuals->m_residuals[irow].setResidualStateCov( - correlationmatrix ) ;
      }
      
      // Let's check the result
      if(!error) {
	std::string message ;
	error = residuals->testHCH(message) ;
	if(error) warning() << message << endreq ;
      }
      if(error) { delete residuals ; residuals = 0 ; }
    }
    
    // let's clean up:
    for( size_t irow = 0; irow<numnodes; ++irow)
      for( size_t icol = 0; icol < irow; ++icol)
	delete offdiagcov[irow][icol] ;
    
    debug() << "End of TrackResidualTool::Compute " << residuals << endreq ;

    return residuals ;
  }

  
}

