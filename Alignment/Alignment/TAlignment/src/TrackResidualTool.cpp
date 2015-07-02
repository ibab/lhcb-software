#include <map>
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiAlg/GaudiTool.h"
#include "LHCbMath/MatrixInversion.h"
#include "ITrackResidualTool.h"
#include "IGetElementsToBeAligned.h"
#include "TrackInterfaces/ITrackKalmanFilter.h"
#include "Event/KalmanFitResult.h"
#include "Event/FitNode.h"

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
				const std::vector<const LHCb::FitNode*>& nodes,
				std::vector<size_t>& residualnodeindices,
				size_t& refnodeindex ) const ;
  private:
    typedef std::map<const LHCb::Track*, const Al::TrackResiduals*> ResidualMap ;
    mutable ResidualMap m_residuals ;
    bool m_testPosDef ;
    ToolHandle<IGetElementsToBeAligned> m_elementTool ;
  } ;
  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
  
#include <iostream>
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "Event/Track.h"

  
namespace Al
{
  
  DECLARE_TOOL_FACTORY( TrackResidualTool )
  
  
  TrackResidualTool::TrackResidualTool(const std::string& type,
				       const std::string& name,
				       const IInterface* parent)
    : GaudiTool(type,name,parent),
      m_elementTool("GetElementsToBeAligned")
  {
    // interfaces
    declareInterface<ITrackResidualTool>(this);
    declareProperty("TestPosDef",m_testPosDef = false) ;
  }
  
  StatusCode TrackResidualTool::initialize()
  {
    StatusCode sc = GaudiTool::initialize();
    if (sc.isFailure()) return Error("Failed to initialize",sc);
    sc = m_elementTool.retrieve() ;
    incSvc()->addListener(this, IncidentType::EndEvent);
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

  template<class Matrix> 
  Matrix correlationMatrix( const Matrix& M )
  {
    Matrix N ;
    for(int i=0; i<Matrix::kCols; ++i) {
      N(i,i) = 1 ;
      for(int j=0; j<i; ++j) 
	N(i,j) = M(i,j) / std::sqrt( M(i,i) * M(j,j)) ;
    }
    return N ;
  }

  template<class Matrix>
  bool testPosDef( const Matrix& M ) {
    Matrix N = correlationMatrix(M) ;
    double det ;
    /*bool success =*/ N.Det( det ) ;
    const double tolerance = 1e-9 ;
    return /*success &&*/ det+tolerance>0 ;
  }

  template<int N>
  ROOT::Math::SMatrix<double, 2*N, 2*N, ROOT::Math::MatRepSym<double,2*N> >
  constructNxN(const Gaudi::SymMatrix5x5& MAA, const Gaudi::SymMatrix5x5& MBB, const Gaudi::Matrix5x5& MAB  ) {
    ROOT::Math::SMatrix<double, 2*N, 2*N, ROOT::Math::MatRepSym<double,2*N> > M ;
    for(int i=0; i<N; ++i) {
      for(int j=0; j<=i; ++j) {
	M(i,j) = MAA(i,j) ;
	M(i+N,j+N) =  MBB(i,j) ;
      }
      for(int j=0; j<N; ++j) {
	M(i,j+N) = MAB(i,j) ;
      }
    }
    return M ;
  }
  
  Al::TrackResiduals* TrackResidualTool::create( const LHCb::Track& track, 
						 const std::vector<const LHCb::FitNode*>& nodes,
						 std::vector<size_t>& residualnodeindices,
						 size_t& refnodeindex ) const
  {
    // first select nodes with a measurement and equip them with an index in the original list
    typedef std::pair<const LHCb::FitNode*, size_t> NodeWithIndex ;
    std::vector< NodeWithIndex > nodeswithindex ;
    nodeswithindex.reserve( nodes.size()) ;
    size_t index(0) ;
    for( std::vector<const LHCb::FitNode*>::const_iterator it = nodes.begin();
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
    rc->m_state = nodeswithindex.front().first->classicalSmoothedState() ;
    for( std::vector< NodeWithIndex >::const_iterator it = nodeswithindex.begin() ;
	 it != nodeswithindex.end(); ++it ) {
      const LHCb::Node* node = it->first ;
      const AlignmentElement* elem = m_elementTool->findElement(*node) ;
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

    // this is not stricktly necessary, but let's do it anyway
    LHCb::KalmanFitResult* fr = 
      const_cast<LHCb::KalmanFitResult*>(static_cast<const LHCb::KalmanFitResult*>(track.fitResult())) ;
    if( !fr ) {
      error() << "Track was not fitted. Type = " << track.type() << ". Returning 0." << endreq ;
      return 0 ;
    }
    fr->setBiDirectionnalSmoother(false) ;
    
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
    std::vector<const LHCb::FitNode*> nodes ;
    for(LHCb::Track::ConstNodeRange::const_iterator it = begin; it != end; ++it)
      nodes.push_back( static_cast<const LHCb::FitNode*>(*it) ) ;
    size_t numnodes = nodes.size() ;
    
    // These are the diagonal elements. At first we just copy these
    bool error = false ;
    std::vector< const Gaudi::TrackSymMatrix* > diagcov(numnodes,0) ;
    for( size_t irow = 0; irow<numnodes; ++irow) {
      diagcov[irow] = &(nodes[irow]->classicalSmoothedState().covariance()) ;
      if( m_testPosDef && ! testPosDef( *(diagcov[irow]) ) ) {
	warning() << "Found non pos def matrix on track of type: " << track.type() << " "
		  << *(diagcov[irow]) << endreq ;
	error = true ;
      }
    }
    
      
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

    std::vector< Gaudi::TrackMatrix > smoothergainmatrix(numnodes-1) ;
    for(size_t k=numnodes-1; k>0 && !error; --k) {
      //std::cout << "k, size: " << k << " " << offdiagcov[k].size() << std::endl ;
      const LHCb::FitNode* node     = dynamic_cast<const LHCb::FitNode*>(nodes[k]) ;
      const LHCb::FitNode* prevnode = dynamic_cast<const LHCb::FitNode*>(nodes[k-1]) ;
      // do these _really_ correspond to 'prevnode'? how do we find out?
      assert( node && prevnode ) ;

      smoothergainmatrix[k-1] = prevnode->smootherGainMatrix() ;
      Gaudi::TrackMatrix C_km1_k = smoothergainmatrix[k-1] * node->classicalSmoothedState().covariance() ;
      offdiagcov[k][k-1] = new Gaudi::TrackMatrix(Transpose(C_km1_k)) ;
      
      if( m_testPosDef && !testPosDef(constructNxN<4>( *(diagcov[k-1]),*(diagcov[k]),C_km1_k) ) ) {
	warning() << "Found non pos def 10x10 matrix on track of type: " << track.type() << endreq ;
	error = true ;
      }
    }
    
    // calculate the remaining elements in the covariance matrix. We
    // need to do this off-diagonal by off-diagonal. The easiest is
    // probably to do it on demand, but the following should also work. Note that
    // this is also where things become really slow.
    if ( !error && (error=fr->inError()) )
      warning() << "Error in KalmanFitResult: " << fr->getError() << endreq ;
    
    Al::TrackResiduals* residuals(0) ;

    if( !error ) {

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
	if(error) {
	  warning() << message << endreq ;
	  warning() << "Track type = " << track.type() << " " << track.history() << " " << track.flag() << endreq ;
	}
      }
      if(error) { delete residuals ; residuals = 0 ; }
    }
    
    // let's clean up:
    for( size_t irow = 0; irow<numnodes; ++irow)
      for( size_t icol = 0; icol < irow; ++icol)
	delete offdiagcov[irow][icol] ;
    
    debug() << "End of TrackResidualTool::Compute " << residuals << endreq ;
    counter("Fraction of failures")+= bool(residuals==0) ;
    return residuals ;
  }

  
}

