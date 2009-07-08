#include "TrackKernel/TrackStateVertex.h"
#include "Event/TrackFunctor.h"
#include "LHCbMath/MatrixInversion.h"
#include "LHCbMath/MatrixTransforms.h"

namespace std {
  inline double sqr(double x) { return x*x ; }
}

namespace LHCb 
{
  namespace TrackVertexHelpers {

    class VertexTrack
    {
    public:
      typedef Gaudi::SymMatrix3x3 PositionCovariance ;
      typedef ROOT::Math::SVector<double,3> PositionParameters ;
      typedef ROOT::Math::SVector<double,3> MomentumParameters ;
      typedef ROOT::Math::SVector<double,5> TrackVector ;

      VertexTrack( const LHCb::State& state, const PositionCovariance& m_poscov ) ;
      
      /// compute chisq derivatives
      void project( const ROOT::Math::SVector<double,3>& position,
		    ROOT::Math::SVector<double,3>& halfDChisqDX,
		    Gaudi::SymMatrix3x3&           halfD2ChisqDX2) ;
      
      /// derivative of momentum to position
      const ROOT::Math::SMatrix<double,3,3>& WBGA() const { 
	return m_WBGA.first ? m_WBGA.second : computeWBGA() ;
      }
      
      /// momentum (tx,ty) covariance ('D' in Fruhwirth)
      const Gaudi::SymMatrix3x3& momcov() const {
	return m_momcov.first ? m_momcov.second : computeMomCov() ;
      }
      Gaudi::SymMatrix3x3& momcov() {
	return m_momcov.first ? m_momcov.second : computeMomCov() ;
      }
      
      /// momentum-position covariance ('E' in Fruhwirth)
      const ROOT::Math::SMatrix<double, 3, 3>& momposcov() const {
	return m_momposcov.first ? m_momposcov.second : computeMomPosCov() ;
      }
      ROOT::Math::SMatrix<double, 3, 3>& momposcov() {
	return m_momposcov.first ? m_momposcov.second : computeMomPosCov() ;
      }
      
      /// chisquare
      double chisq(const ROOT::Math::SVector<double,3>& pos) const {
	return ROOT::Math::Similarity(residual(pos),m_G) ;
      }

      /// update momentum for given change in position
      void updateSlopes( const ROOT::Math::SVector<double,3>& pos ) { 
	m_q += m_WBG * residual(pos) ; 
      }
      
      /// residual for given position
      inline TrackVector residual(const PositionParameters& pos) const ;
      inline TrackVector prediction(const PositionParameters& pos) const ;
      
      /// full state after fitting
      inline void momentum(ROOT::Math::SVector<double,3>& mom,
			   Gaudi::SymMatrix3x3& momcov,
			   ROOT::Math::SMatrix<double,3,3>& posmomcov) const ; 
      
      const LHCb::State& state() const { return m_state ; } 
      const ROOT::Math::SMatrix<double,5,3>& A() const { return m_A ; }
      const ROOT::Math::SMatrix<double,5,3>& B() const { return m_B ; }
      const MomentumParameters& mom() const { return m_q ; }
      MomentumParameters& mom() { return m_q ; }

#ifndef GOD_NOALLOC
      static void* operator new ( size_t size ) {
	return ( sizeof(VertexTrack) == size ?
		 boost::singleton_pool<VertexTrack, sizeof(VertexTrack)>::malloc() :
		 ::operator new(size) );
      }
      static void operator delete ( void* p ) {
	boost::singleton_pool<VertexTrack, sizeof(VertexTrack)>::free(p) ;
      }
#endif

    private:
      ROOT::Math::SMatrix<double,3,3>& computeWBGA() const {
	m_WBGA.first = true ; 
	return m_WBGA.second = m_WBG * m_A ; 
      }
      
      Gaudi::SymMatrix3x3& computeMomCov() const {
	m_momcov.first = true ;
	ROOT::Math::AssignSym::Evaluate(m_momcov.second , -momposcov() * ROOT::Math::Transpose(WBGA())) ;
	m_momcov.second += m_W ;
      	return m_momcov.second ;
      }
      
      ROOT::Math::SMatrix<double, 3, 3>& computeMomPosCov() const {
	m_momposcov.first = true ;
	m_momposcov.second = - WBGA() * m_poscov ; 
	return m_momposcov.second ;
      }
      
    private:
      const LHCb::State& m_state ;
      const PositionCovariance& m_poscov ;
      
      // In fruhwirth's notation
      Gaudi::SymMatrix5x5 m_G ;               // weight matrix of state p=(x,y,tz,ty)
      ROOT::Math::SMatrix<double,5,3> m_A ;   // projection matrix for vertex position
      ROOT::Math::SMatrix<double,5,3> m_B ;   // projection matrix for slopes (tx,ty, qop)
      ROOT::Math::SVector<double,3>   m_q ;   // predicted/fitted slope (tx,ty)
      Gaudi::SymMatrix3x3 m_W ;               // cov matrix of slopes for fixed position
      ROOT::Math::SMatrix<double,3,5> m_WBG  ; // derivative dmom/dresidual  
      mutable std::pair<bool, ROOT::Math::SMatrix<double,3,3> > m_WBGA ;
      mutable std::pair<bool, Gaudi::SymMatrix3x3 > m_momcov ;
      mutable std::pair<bool, ROOT::Math::SMatrix<double,3,3> > m_momposcov ;
    } ;
  
    VertexTrack::VertexTrack(const LHCb::State& state, const PositionCovariance& poscov)
      : m_state(state), m_poscov(poscov)
    {
      // reset cache of variables that we calculate only on demand
      m_WBGA.first = m_momcov.first = m_momposcov.first = false ;

      // compute the weight matrix (inverse of V)
      m_G = state.covariance() ;
      Gaudi::Math::invertPosDefSymMatrix( m_G ) ;
      m_q(0) = state.tx() ;
      m_q(1) = state.ty() ;
      m_q(2) = state.qOverP() ;
    }

    inline VertexTrack::TrackVector
    VertexTrack::prediction(const PositionParameters& pos) const
    {
      double dz = m_state.z() - pos(2) ;
      TrackVector vec ;
      vec(0) = ( pos(0) + dz * m_q(0)) ;
      vec(1) = ( pos(1) + dz * m_q(1)) ;
      vec(2) = m_q(0) ;
      vec(3) = m_q(1) ;
      vec(4) = m_q(2) ;
      return vec ;
    }
    
    inline VertexTrack::TrackVector
    VertexTrack::residual(const ROOT::Math::SVector<double,3>& pos) const
    {
      VertexTrack::TrackVector res = - prediction(pos) ;
      res(0) += m_state.x() ;
      res(1) += m_state.y() ;
      res(2) += m_state.tx() ;
      res(3) += m_state.ty() ;
      res(4) += m_state.qOverP() ;
      return res ;
    }
    
    void VertexTrack::project( const ROOT::Math::SVector<double,3>& pos,
			       ROOT::Math::SVector<double,3>& halfDChisqDX,
			       Gaudi::SymMatrix3x3&           halfD2ChisqDX2)
    {
      // projection matrix for vertex position
      m_A(0,0) = m_A(1,1) = 1 ;
      m_A(0,2) = -m_q(0) ;
      m_A(1,2) = -m_q(1) ;
      
      // projection matrix for momentum (tx,ty,qop)
      double dz = m_state.z() - pos(2) ;
      m_B(2,0) = m_B(3,1) = m_B(4,2) = 1 ;
      m_B(0,0) = m_B(1,1) = dz ;
      
      // Matrix W (Fruhwirth)
      m_W = ROOT::Math::Similarity( Transpose(m_B), m_G ) ;
      Gaudi::Math::invertPosDefSymMatrix( m_W ) ;

      // momentum gain matrix
      m_WBG  = m_W * Transpose(m_B) * m_G ;
      
      // Weight matrix after minimization to momentum
      Gaudi::SymMatrix5x5 GB = m_G - ROOT::Math::Similarity( m_G*m_B, m_W) ;
      
      // Add to chisquare plus derivatives
      halfDChisqDX += -ROOT::Math::Transpose(m_A) * GB * residual(pos) ;
      halfD2ChisqDX2 +=  ROOT::Math::Similarity(ROOT::Math::Transpose(m_A),GB) ;
      // reset cache of variables that we calculate only on demand
      m_WBGA.first = m_momcov.first = m_momposcov.first = false ;
    }

  }

  TrackStateVertex::~TrackStateVertex()
  {
    std::for_each(m_tracks.begin(),m_tracks.end(),TrackFunctor::deleteObject()) ; 
  }
  
  TrackStateVertex::TrackStateVertex()
    : m_fitStatus(UnFitted), m_chi2(-1)
  {
    for(size_t i=0; i<PositionParameters::kSize; ++i) m_pos(i) = 0 ;
  }
  
  TrackStateVertex::TrackStateVertex( const std::vector<const LHCb::State*>& states,
				      double maxdchisq, size_t maxnumiter)
    : m_fitStatus(UnFitted), m_chi2(-1)
  {
    for(size_t i=0; i<PositionParameters::kSize; ++i) m_pos(i) = 0 ;
    for( std::vector<const LHCb::State*>::const_iterator istate = states.begin() ;
	 istate != states.end(); ++istate ) 
      addTrack( **istate ) ;
    fit( maxdchisq, maxnumiter ) ;
  }
  
  void TrackStateVertex::addTrack( const LHCb::State& inputstate )
  {
    m_tracks.push_back( new VertexTrack( inputstate, m_poscov) ) ;
    m_fitStatus = UnFitted ;
    int N = m_tracks.size() ;
    m_pos(0) = ( (N-1) * m_pos(0) + inputstate.x())/N ;
    m_pos(1) = ( (N-1) * m_pos(1) + inputstate.y())/N ;
    m_pos(2) = ( (N-1) * m_pos(2) + inputstate.z())/N ;
  }

  double TrackStateVertex::fitOneStep()
  {
    // This implements the Billoir-Fruhwirth-Regler algorithm.
    Gaudi::SymMatrix3x3 halfD2ChisqDX2 ;
    ROOT::Math::SVector<double,3> halfDChisqDX ;
    // add all the tracks
    for( VertexTrackContainer::iterator itrack = m_tracks.begin() ;
         itrack != m_tracks.end(); ++itrack ) 
      (*itrack)->project( m_pos, halfDChisqDX, halfD2ChisqDX2) ;
    
    // calculate the covariance and the change in the position
    m_poscov = halfD2ChisqDX2 ;
    Gaudi::Math::invertPosDefSymMatrix(m_poscov) ;
    ROOT::Math::SVector<double,3> dpos = - m_poscov * halfDChisqDX ;
    // update the position
    m_pos += dpos ;
    // update the momenta
    for( VertexTrackContainer::const_iterator itrack = m_tracks.begin() ;
         itrack != m_tracks.end(); ++itrack ) 
      (*itrack)->updateSlopes(m_pos) ;
    // return the delta-chisquare
    double dchisq = ROOT::Math::Dot(dpos,halfDChisqDX) ;
    m_fitStatus = FitSuccess ;
    m_chi2      = -1 ;
    return dchisq ;
  }
  
  double TrackStateVertex::chi2() const
  {
    if( m_chi2 < 0 ) {
      m_chi2 = 0 ;
      for( VertexTrackContainer::const_iterator itrack = m_tracks.begin() ;
	   itrack != m_tracks.end(); ++itrack ) 
	m_chi2 += (*itrack)->chisq(m_pos) ;
    }
    return m_chi2 ;
  }

  TrackStateVertex::FitStatus TrackStateVertex::fit( double maxdchisq, size_t maxnumiter)
  {
    bool converged(false) ;
    size_t iter(0) ;
    for(; iter<maxnumiter &&!converged; ++iter) {
      double dchisq = fitOneStep() ;
      converged = -dchisq < maxdchisq ;
    }
    m_fitStatus = converged ? FitSuccess : FitFailure ;
    return m_fitStatus ;
  }

  const TrackStateVertex::MomentumParameters& TrackStateVertex::mom( size_t i ) const
  {
    return m_tracks[i]->mom() ;
  }

  const TrackStateVertex::MomentumCovariance& TrackStateVertex::momCovMatrix( size_t i ) const
  {
    return m_tracks[i]->momcov() ;
  }

  Gaudi::TrackVector
  TrackStateVertex::stateVector(size_t i) const
  {
    return m_tracks[i]->prediction(m_pos) ;
  }
  
  Gaudi::TrackSymMatrix
  TrackStateVertex::stateCovariance(size_t i) const
  { 
    const VertexTrack& track = *(m_tracks[i]) ;
    Gaudi::TrackSymMatrix rc ;
    const ROOT::Math::SMatrix<double,5,3>& A = track.A() ;
    const ROOT::Math::SMatrix<double,5,3>& B = track.B() ;

    Gaudi::Matrix5x5 tmp = A * ROOT::Math::Transpose(track.momposcov()) * ROOT::Math::Transpose(B) ;
    ROOT::Math::AssignSym::Evaluate( rc, tmp + ROOT::Math::Transpose(tmp) ) ;
    rc += ROOT::Math::Similarity( A, m_poscov ) ;
    rc += ROOT::Math::Similarity( B, track.momcov() ) ;
    return rc ;
  }
  
  const Gaudi::Matrix3x3& TrackStateVertex::computeMomMomCov(size_t i, size_t j) const
  {
    assert( i >= j ) ;
    if( m_mommomcov.empty() ) {
      // update the cache
      size_t dim = m_tracks.size() ;
      m_mommomcov.resize( dim*(dim+1)/2 ) ;
      for(size_t index =0 ; index<dim ; ++index) {
	// first the off-diagonal
	for(size_t jndex =0 ; jndex<index ; ++jndex) {
	  m_mommomcov[symIndex(index,jndex)] = m_tracks[index]->WBGA() * m_poscov * ROOT::Math::Transpose(m_tracks[jndex]->WBGA()) ;
	}
	// the on-diagonal is just a copy
	m_mommomcov[symIndex(index,index)] = m_tracks[index]->momcov() ;
      }
    }
    return m_mommomcov[symIndex(i,j) ] ;
  }
  
  Gaudi::Matrix5x5 
  TrackStateVertex::stateCovariance(size_t i, size_t j) const
  {
    assert(i!=j) ;
    const VertexTrack& track1 = *(m_tracks[i]) ;
    const VertexTrack& track2 = *(m_tracks[j]) ;
    const ROOT::Math::SMatrix<double,5,3>& A1 = track1.A() ;
    const ROOT::Math::SMatrix<double,5,3>& B1 = track1.B() ;
    ROOT::Math::SMatrix<double,3,5> A2 = ROOT::Math::Transpose(track2.A()) ;
    ROOT::Math::SMatrix<double,3,5> B2 = ROOT::Math::Transpose(track2.B()) ;
    
    //Gaudi::Matrix3x3 mommomcovtmp = track1.WBGA() * m_poscov * ROOT::Math::Transpose(track2.WBGA()) ;
    //const mommomcov
    Gaudi::Matrix3x3 mommomcov = momMomCovMatrix(i,j) ;

    Gaudi::Matrix5x5 rc ;
    rc += A1 * m_poscov * A2 ;
    rc += A1 * ROOT::Math::Transpose(track2.momposcov()) * B2 ;
    rc += B1 * track1.momposcov() * A2 ;
    rc += B1 * mommomcov * B2 ;
    return rc ;
  }

  
  LHCb::State
  TrackStateVertex::state(size_t i) const
  {
    return LHCb::State(stateVector(i),stateCovariance(i),
		       m_tracks[i]->state().z(),
		       m_tracks[i]->state().location()) ;
  }	       
  
  const LHCb::State&
  TrackStateVertex::inputState(size_t i) const
  {
    return m_tracks[i]->state() ;
  } 

  const ROOT::Math::SMatrix<double,5,3>& TrackStateVertex::matrixA(size_t i) const
  {
    return m_tracks[i]->A() ;
  }

  const ROOT::Math::SMatrix<double,5,3>& TrackStateVertex::matrixB(size_t i) const
  {
    return m_tracks[i]->B() ;
  }
  
  const ROOT::Math::SMatrix<double,3,3>& TrackStateVertex::momPosCovMatrix(size_t i) const
  {
    return m_tracks[i]->momposcov() ;
  }

  Gaudi::LorentzVector TrackStateVertex::p4(const std::vector<double>& masshypos) const 
  {
    Gaudi::LorentzVector p4sum,p4tmp ;
    for( size_t index = 0 ; index<m_tracks.size() ; ++index) {
      Gaudi::Math::geo2LA(m_tracks[index]->mom(),masshypos[index],p4tmp);
      p4sum += p4tmp ;
    }
    return p4sum ;
  }
  
  double TrackStateVertex::mass(const std::vector<double>& masshypos) const 
  {
    return p4(masshypos).M() ;
  }
  
  double TrackStateVertex::massErr(const std::vector<double>& masshypos) const 
  {  
    Gaudi::SymMatrix4x4 p4cov ;
    size_t dim = m_tracks.size() ;
    std::vector<Gaudi::Matrix4x3> dP4dMom(dim) ;
    Gaudi::Matrix4x4    tmp4x4 ;
    Gaudi::SymMatrix4x4 stmp4x4 ;
    for( size_t index = 0 ; index<dim ; ++index) {
      Gaudi::Math::JacobdP4dMom(m_tracks[index]->mom(),masshypos[index],dP4dMom[index]);
      p4cov += ROOT::Math::Similarity( dP4dMom[index], m_tracks[index]->momcov() ) ;
      for( size_t jndex = 0 ; jndex<index ; ++jndex) {
	tmp4x4 = dP4dMom[index] * momMomCovMatrixFast(index,jndex) * ROOT::Math::Transpose(dP4dMom[jndex]);
	ROOT::Math::AssignSym::Evaluate( stmp4x4, tmp4x4 + ROOT::Math::Transpose( tmp4x4 ) ) ;
	p4cov += stmp4x4 ;
      }
    }
    ROOT::Math::SMatrix<double,1,4> dMdP4 ;
    Gaudi::LorentzVector p4sum = p4(masshypos) ;
    double m = p4sum.M() ;
    dMdP4(0,0) = - p4sum.Px()/m ;
    dMdP4(0,1) = - p4sum.Py()/m ;
    dMdP4(0,2) = - p4sum.Pz()/m ;
    dMdP4(0,3) =   p4sum.E()/m ;
    return std::sqrt( ROOT::Math::Similarity( dMdP4, p4cov )(0,0) ) ;
  }
  
  TrackStateVertex::FitStatus TrackStateVertex::constrainMass(const std::vector<double>& masshypos,
							      double constrainedmass)
  {
    // note: this is not optimized for speed. we can improve a lot by
    // not testing the cache all the time ...

    // the easiest way is with a K-filter step. that requires us to
    // store the complete cov matrix, so we need to take care of that
    // first. once thatis done, use the following formalism:
    //
    // filtering an exact constraint:
    //  dx = - K * (m^2 - p4^2 )
    //    K = C H^T (HCH^T)^{-1}
    // the term on the right is the inverse variance of the mass^2
    //  dC  = - KHC
    //
    // now we need to split the gain matrix in a position and a momentum part, which should be quite easy
    //
    // 
    size_t dim = m_tracks.size() ;

    // compute the total p4, the mom-p4 jacobian and p4 covariance matrix
    std::vector<Gaudi::LorentzVector> p4(dim) ;
    std::vector<Gaudi::Matrix4x3> dP4dMom(dim) ;
    Gaudi::LorentzVector p4sum ;
    Gaudi::SymMatrix4x4 p4cov ;
    Gaudi::Matrix4x4    tmp4x4 ;
    Gaudi::SymMatrix4x4 stmp4x4 ;
    for( size_t index = 0 ; index< dim ; ++index) {
      Gaudi::Math::geo2LA(m_tracks[index]->mom(),masshypos[index],p4[index]);
      p4sum += p4[index] ;
      Gaudi::Math::JacobdP4dMom(m_tracks[index]->mom(),masshypos[index],dP4dMom[index]);
      p4cov += ROOT::Math::Similarity( dP4dMom[index], m_tracks[index]->momcov() ) ;
      for( size_t jndex = 0 ; jndex<index ; ++jndex) {
	tmp4x4 = dP4dMom[index] * momMomCovMatrixFast(index,jndex) * ROOT::Math::Transpose(dP4dMom[jndex]);
	ROOT::Math::AssignSym::Evaluate( stmp4x4, tmp4x4 + ROOT::Math::Transpose( tmp4x4 ) ) ;
	p4cov += stmp4x4 ;
      }
    }
    
    // now there are many ways to formulate the constraint. we'll
    // start with this one, which is the one in squares
    
    // (sum sqrt( m_i^2 + p_i^2))^2 - M^2 - (sum p_i)^2 = 0
    double constrainedmass2 = std::sqr(constrainedmass) ;
    // this is the 'residual'
    ROOT::Math::SVector<double,1> r(p4sum.M2() - constrainedmass2) ;
    
    // compute the covariance for the residual (which is the covariance on the mass2)
    ROOT::Math::SMatrix<double,1,4> dM2dP4 ;
    dM2dP4(0,0) = - 2*p4sum.Px() ;
    dM2dP4(0,1) = - 2*p4sum.Py() ;
    dM2dP4(0,2) = - 2*p4sum.Pz() ;
    dM2dP4(0,3) =   2*p4sum.E() ;  // <--- it might be better to use M^2 + P^2 here !
    dM2dP4(0,3) =   2*std::sqrt( p4sum.P2() + constrainedmass2) ;
    Gaudi::SymMatrix1x1 R = ROOT::Math::Similarity( dM2dP4, p4cov ) ;
    Gaudi::SymMatrix1x1 Rinv(1/R(0,0)) ;
    
    // these are the derivatives of the constraint to the track momenta
    std::vector< ROOT::Math::SMatrix<double,1,3> > matrixH( dim ) ;
    for( size_t index = 0 ; index<dim ; ++index)
      matrixH[index] = dM2dP4 * dP4dMom[index] ;
    
    // first compute the total p4 and store all the
    // jacobians. together with the jacobian from p4 to mass2, this
    // gives all matrices H in the equation above.
    
    // compute the new position: deltapos = sum_tracks C_posmom_i * H_i^T * W * r ;
    // compute the new momentum: deltamom_j = sum_tracks C_mommom_ji * H_i^T * W * r ;
    ROOT::Math::SVector<double,3> deltapos ;
    std::vector<ROOT::Math::SVector<double,3> > deltamom(dim) ;
    for( size_t index = 0 ; index<dim ; ++index) {
      ROOT::Math::SVector<double,3> projres = ROOT::Math::Transpose(matrixH[index]) * Rinv * r ;
      deltapos += ROOT::Math::Transpose(m_tracks[index]->momposcov()) * projres ;
      for( size_t jndex = 0 ; jndex<dim ; ++jndex) 
	deltamom[jndex] += momMomCovMatrix(jndex,index) * projres ;
    }
    
    // now compute the change in the covariance matrices. we will not
    // exploit the symmetry of the diagonal elements here, just to
    // simplify things. note that we can speed this up a lot by
    // caching some stuff. maybe later.

    //std::vector<Gaudi::SymMatrix3x3 > deltamomcov(dim) ;
    Gaudi::SymMatrix3x3 deltaposcov ; // C * H^T * W * H * C
    std::vector< Gaudi::Matrix3x3 > deltamomposcov( dim ) ; 
    std::vector< Gaudi::Matrix3x3 > deltamommomcov( dim * (dim+1) / 2 ) ;
    Gaudi::SymMatrix3x3 stmp3x3 ;
    Gaudi::Matrix3x3 tmp3x3 ;
    
    // note that the symmetry appears because of the double entries. except for diagonal. need to be very careful here ...
    for( size_t index = 0 ; index<dim ; ++index) {
      // first the position
      deltaposcov += ROOT::Math::Similarity( ROOT::Math::Transpose(matrixH[index] * m_tracks[index]->momposcov()), Rinv ) ;
      for( size_t jndex = 0 ; jndex<index ; ++jndex) {
	tmp3x3 = 
	  ROOT::Math::Transpose( matrixH[index] * m_tracks[index]->momposcov()) * Rinv 
	  * matrixH[jndex] * m_tracks[jndex]->momposcov() ;
	ROOT::Math::AssignSym::Evaluate(stmp3x3, tmp3x3 + ROOT::Math::Transpose(tmp3x3) ) ;
	deltaposcov += stmp3x3 ;
      }
      // now the momentum.
      for( size_t kndex = 0 ; kndex<dim ; ++kndex) {
	for( size_t lndex = 0 ; lndex<dim ; ++lndex) {
	  for( size_t jndex = 0 ; jndex<=index ; ++jndex) {
	    deltamommomcov[symIndex(index,jndex)]
	      += momMomCovMatrix(index,kndex) * ROOT::Math::Transpose(matrixH[kndex]) * Rinv 
	      * matrixH[lndex] * momMomCovMatrix(lndex,jndex) ;
	  }
	}
      }
      // now mom-pos
      for( size_t kndex = 0 ; kndex<dim ; ++kndex) {
	for( size_t lndex = 0 ; lndex<dim ; ++lndex) {
	  deltamomposcov[index] 
	    += momMomCovMatrix(index,kndex) * ROOT::Math::Transpose(matrixH[kndex]) * Rinv 
	    * matrixH[lndex] * m_tracks[lndex]->momposcov() ;
	}
      }
    }

    // now we need to copy everything back
    m_pos    -= deltapos ;
    m_poscov -= deltaposcov ;
    for( size_t index = 0 ; index<dim ; ++index) {
      m_tracks[index]->mom() -= deltamom[index] ;
      ROOT::Math::AssignSym::Evaluate(stmp3x3, deltamommomcov[symIndex(index,index)] ) ;
      m_tracks[index]->momcov()    -= stmp3x3 ;
      m_tracks[index]->momposcov() -= deltamomposcov[index] ;
      for( size_t jndex = 0 ; jndex<=index ; ++jndex) 
	m_mommomcov[symIndex(index,jndex)] -= deltamommomcov[symIndex(index,jndex)] ;
    }
    m_chi2 += r(0) * Rinv(0,0) * r(0) ;
    return m_fitStatus ;
  }
}
