
#include "ResidualCovarianceTool.h"

#include "Event/Node.h"
#include "Event/FitNode.h"
#include "CLHEP/Matrix/SymMatrix.h"

int fail;

StatusCode ResidualCovarianceTool::compute(const LHCb::Track& track)
{
  //std::cout << "ResidualCovarianceTool::compute" << std::endl ;

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
  const std::vector<LHCb::Node*> allnodes = track.nodes() ;
  std::vector<LHCb::Node*>::const_iterator begin = allnodes.end() ;
  std::vector<LHCb::Node*>::const_iterator end   = allnodes.begin() ;
  for( std::vector<LHCb::Node*>::const_iterator it = allnodes.begin() ;
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

  // These are the diagonal elements. Those can just be pointers to the nodes.
  std::vector< const Gaudi::TrackSymMatrix* > diagcov(numnodes) ;
  // These are the inverse of the diagonal elements
  for( size_t irow = 0; irow<numnodes; ++irow) {
    diagcov[irow]    = &(nodes[irow]->state().covariance());
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

  // We will not first compute the smoother gain matrix on every (but
  // the last) node. Once we have the gain matrices, the rest is
  // trivial, though still CPU intensive.

  std::vector< Gaudi::TrackMatrix > smoothergainmatrix(numnodes-1) ;
  for(size_t k=1; k<numnodes; ++k) {
    //std::cout << "k, size: " << k << " " << offdiagcov[k].size() << std::endl ;
    const LHCb::FitNode* node     = dynamic_cast<const LHCb::FitNode*>(nodes[k]) ;
    const LHCb::FitNode* prevnode = dynamic_cast<const LHCb::FitNode*>(nodes[k-1]) ;
      // do these _really_ correspond to 'prevnode'? how do we find out?
    if(!node || !prevnode) {
      std::cout << "serious problem." << std::endl ;
      continue;
    }

    // this is the transport matrix  from  k-1 to k
    const Gaudi::TrackMatrix& F          = node->transportMatrix();
    // this is the noise matrix from k-1 to k
    const Gaudi::TrackSymMatrix& Q       = node->noiseMatrix();
    // this is the prediction from k-1 to k
    const Gaudi::TrackSymMatrix& predcov = node->predictedStateUp().covariance() ;
    // this is the smoothedcovariance
    const Gaudi::TrackSymMatrix& cov    = node->state().covariance() ;
    
    // invert that prediction
    Gaudi::TrackSymMatrix invpredcov = predcov.Inverse(fail); assert(fail==0);

    // invert the transport matrix
    Gaudi::TrackMatrix invF = F.Inverse(fail) ; assert(fail==0);
    
    // Now compute the smoother gain matrix for node k-1. The full formula is
    //
    // A_{k-1} = invF * ( predcov - Q ) * invpredcov
    //
    // The one big problem is the subtraction: If Q is large
    // compared to the filter covariance from the previous state
    // (which we do no longer have), then this subtraction can go
    // wrong. (The correlation is then essentially 'zero'.) Let's
    // hope that this does not happen.

    // Gaudi::TrackSymMatrix upstreamfiltered = predcov - Q ;
    // do some test
    
    // continue
    smoothergainmatrix[k-1] = invF * ( predcov - Q ) * invpredcov ;

    // Now calculate the diagonal (k-1,k). This matrix represents
    // the covariance of the statvectors of k-1 and k. (multiply
    // with k-1 on the left and with k on the right)
    Gaudi::TrackMatrix C_km1_k = smoothergainmatrix[k-1] * cov ;

    // What we actually need is the transpose. We can imporve this later:

    assert( k < offdiagcov.size() ) ;
    if ( !(k-1 < offdiagcov[k].size() ) ) {
	    std::cout << "k,size" << k << " " << offdiagcov[k-1].size() << std::endl ;
    }

    assert( k-1< offdiagcov[k].size() ) ;

    offdiagcov[k][k-1] = new Gaudi::TrackMatrix(Transpose(C_km1_k)) ;
  }

  // calculate the remaining elements in the covariance matrix. We
  // need to do this off-diagonal by off-diagonal. The easiest is
  // probably to do it on demand, but the following should also work. Note that
  // this is also where things become really slow.
  
  for(size_t l=2; l<numnodes; ++l) {
    for(size_t k=l-1; k>0; --k) {
      assert( offdiagcov[l][k] != 0 ) ;
      offdiagcov[l][k-1] = new Gaudi::TrackMatrix( (*(offdiagcov[l][k])) * Transpose(smoothergainmatrix[k-1]) ) ;
    }
  }


  // for now, just check that every element is filled
  for( size_t irow = 1; irow<numnodes; ++irow) for(size_t icol = 0; icol<irow; ++icol)
      if( offdiagcov[irow][icol] == 0 )
        std::cout << "element not filled: " << irow << " " << icol << " " << numnodes << std::endl ;
  
  
  // now it is time to calculate the covariance matrix for the
  // residuals. First we need to know which nodes correspond to
  // measurements. After that, we fill the matrix.
  std::vector<size_t> measurementnodes ;
  m_nodes.clear() ;
  for(size_t i=0; i<numnodes; ++i)
    if( nodes[i]->hasMeasurement() &&
	nodes[i]->type() != LHCb::Node::Outlier ) {
      measurementnodes.push_back(i) ;
      m_nodes.push_back(nodes[i]);
    }
  
  // allocate the H C H^T matrix
  // NOTE: we normalize H C H^T relative to V
  size_t nummeasurements = measurementnodes.size() ;
  m_HCH = CLHEP::HepSymMatrix(nummeasurements,0) ;
  CLHEP::HepSymMatrix V = CLHEP::HepSymMatrix(nummeasurements,0) ;
  for(size_t irow=0; irow<nummeasurements; ++irow) {
    size_t k = measurementnodes[irow] ;
    assert(m_nodes[irow]==nodes[k]);
    const Gaudi::TrackProjectionMatrix& Hk = nodes[k]->projectionMatrix() ;
    // first the off-diagonal elements
    for(size_t icol=0; icol<irow; ++icol) {
      size_t l = measurementnodes[icol] ;
      assert(m_nodes[icol]==nodes[l]) ;
      const Gaudi::TrackProjectionMatrix& Hl = nodes[l]->projectionMatrix() ;
      m_HCH.fast(irow+1,icol+1) = (Hk * *(offdiagcov[k][l]) * Transpose(Hl))(0,0)/
        sqrt(nodes[k]->errMeasure2()*nodes[l]->errMeasure2()) ;
    }
    // now the diagonal element
    m_HCH.fast(irow+1,irow+1) = ROOT::Math::Similarity(Hk,*(diagcov[k]))(0,0)/nodes[k]->errMeasure2() ;
  }

  // let's clean up:
  for( size_t irow = 0; irow<numnodes; ++irow) {
    for( size_t icol = 0; icol < irow; ++icol)
      delete offdiagcov[irow][icol] ;
  }

  // Let's check the result. Keep track of the roots.
  m_error = false ;
  std::vector<double> diagroots ;
  for(size_t irow=0; irow<nummeasurements && !m_error; ++irow) {
    double c = m_HCH.fast(irow+1,irow+1) ;
    m_error = ! (0 <= c && c <=1) ;
    if(m_error) std::cout << "found bad element on diagonal: "
			  << c << std::endl ;
    diagroots.push_back(std::sqrt(c)) ;
  }
  for(size_t irow=0; irow<nummeasurements && !m_error; ++irow) 
    for(size_t icol=0; icol<irow && !m_error; ++icol) {
      double c = m_HCH.fast(irow+1,icol+1) / (diagroots[irow]*diagroots[icol]) ;
      m_error =  ! (-1 <= c && c <=1) ;
      if(m_error) std::cout << "found bad element on offdiagonal: "
			    << c << std::endl ;
    }
  return StatusCode::SUCCESS ;
}

double ResidualCovarianceTool::HCH_norm(const LHCb::Node& i, const LHCb::Node& j) const 
{
   int ii(-1),ij(-1);   
   for (unsigned k=0;k<m_nodes.size();++k) {
    if (&i == m_nodes[k]) ii=k+1;
    if (&j == m_nodes[k]) ij=k+1;
   }
   assert(ii!=-1 && ij!=-1);
   return m_HCH(ii,ij);
}
