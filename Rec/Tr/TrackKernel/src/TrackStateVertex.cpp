#include "TrackKernel/TrackStateVertex.h"
#include "Event/TrackFunctor.h"
#include "LHCbMath/MatrixTransforms.h"
#include "TrackKernel/TrackVertexUtils.h"

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
      enum Status { IsOK, InitFailure, ProjFailure } ;
      
      VertexTrack( const LHCb::State& state, const PositionCovariance& m_poscov ) ;

      /// set the state
      void setState( const LHCb::State& state ) {
        m_state = state ; 
        m_G = state.covariance() ;
        m_G.InvertChol();
      }
      
      /// compute chisq derivatives. returns 1 if fine.
      Status project( const ROOT::Math::SVector<double,3>& position,
		      ROOT::Math::SVector<double,3>& halfDChisqDX,
		      Gaudi::SymMatrix3x3&           halfD2ChisqDX2) ;
      /// same as above but exploiting that A and B are almost empty
      Status projectFast( const ROOT::Math::SVector<double,3>& position,
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
      
      /// chisquare (ignores the weight)
      double chisq(const ROOT::Math::SVector<double,3>& pos) const {
	return ROOT::Math::Similarity(residual(pos),m_G) ;
      }

      /// set the weight
      void setweight(double w) { m_weight = w ; }

      /// retrieve the weight
      double weight() const { return m_weight ; }

      /// retrieve the status
      Status status() const { return m_status ; }
 
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
      LHCb::State m_state ;
      const PositionCovariance& m_poscov ;
      double m_weight ;

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
      Status m_status ;
    } ;
  
    VertexTrack::VertexTrack(const LHCb::State& state, const PositionCovariance& poscov)
      : m_state(state), m_poscov(poscov), m_weight(1.0), m_status(IsOK)
    {
      // reset cache of variables that we calculate only on demand
      m_WBGA.first = m_momcov.first = m_momposcov.first = false ;

      // compute the weight matrix (inverse of V)
      m_G = state.covariance() ;
      bool OK = m_G.InvertChol();
      if( !OK ) {
	//std::cout << "TrackStateVertex::Vertextrack constructor: cannot invert cov matrix "
	//<< state.covariance() << std::endl ;
	m_status = InitFailure ;
      }
      // set the initial momentum
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
    
    VertexTrack::Status
    VertexTrack::project( const ROOT::Math::SVector<double,3>& pos,
			  ROOT::Math::SVector<double,3>& halfDChisqDX,
			  Gaudi::SymMatrix3x3&           halfD2ChisqDX2)
    {
      if( m_status == InitFailure ) return m_status ;

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
      bool OK = m_W.InvertChol();
      if( !OK ) m_status = ProjFailure ;

      // now we need to be careful with the track weight. in
      // principle, we just reweight m_G. However, if the weight is
      // close to zero, then the matrix inversion of m_W fails. The
      // weight has no effect on m_WBG. In the end, by ignoring the
      // weight until the derivatives, all we do wrong are the track
      // momenta (which would get infinite error for zero weight).
      
      // momentum gain matrix
      m_WBG  = m_W * Transpose(m_B) * m_G ;
      
      // Weight matrix after minimization to momentum
      Gaudi::SymMatrix5x5 GB = m_G - ROOT::Math::Similarity( m_G*m_B, m_W) ;
      
      // Add to chisquare plus derivatives
      halfDChisqDX += - m_weight * ROOT::Math::Transpose(m_A) * GB * residual(pos) ;
      halfD2ChisqDX2 += m_weight * ROOT::Math::Similarity(ROOT::Math::Transpose(m_A),GB) ;

      // reset cache of variables that we calculate only on demand
      m_WBGA.first = m_momcov.first = m_momposcov.first = false ;
      return m_status ;
    }
    
    VertexTrack::Status
    VertexTrack::projectFast( const ROOT::Math::SVector<double,3>& pos,
			      ROOT::Math::SVector<double,3>& halfDChisqDX,
			      Gaudi::SymMatrix3x3&           halfD2ChisqDX2)
    {
      if( m_status == InitFailure ) return m_status ;

      // projection matrix for vertex position
      m_A(0,0) = m_A(1,1) = 1 ;
      m_A(0,2) = -m_q(0) ;
      m_A(1,2) = -m_q(1) ;
      
      // projection matrix for momentum (tx,ty,qop)
      double dz = m_state.z() - pos(2) ;
      m_B(2,0) = m_B(3,1) = m_B(4,2) = 1 ;
      m_B(0,0) = m_B(1,1) = dz ;
      
      // Matrix W (Fruhwirth)
      // let's see if we can optimize this a bit
      //m_W = ROOT::Math::Similarity( Transpose(m_B), m_G ) ;
      double dz2 = dz*dz ;
      m_W(0,0) = dz2*m_G(0,0) + 2*dz*m_G(2,0) + m_G(2,2) ;
      m_W(1,0) = dz2*m_G(1,0) + dz*( m_G(2,1) + m_G(3,0) ) + m_G(3,2) ;
      m_W(2,0) = dz*m_G(4,0) + m_G(4,2) ;
      m_W(1,1) = dz2*m_G(1,1) + 2*dz*m_G(3,1) + m_G(3,3) ;
      m_W(2,1) = dz*m_G(4,1) + m_G(4,3) ;
      m_W(2,2) = m_G(4,4) ;
      
      bool OK = m_W.InvertChol();
      if( !OK ) m_status = ProjFailure ;

      // now we need to be careful with the track weight. in
      // principle, we just reweight m_G. However, if the weight is
      // close to zero, then the matrix inversion of m_W fails. The
      // weight has no effect on m_WBG. In the end, by ignoring the
      // weight until the derivatives, all we do wrong are the track
      // momenta (which would get infinite error for zero weight).
      
      // momentum gain matrix (3x5). w don't actually need it here,
      // but we don't save any time by not cmputing it.
      //m_WBG  = m_W * Transpose(m_B) * m_G ;
      Gaudi::Matrix3x5 WB ;
      WB(0,0) = dz * m_W(0,0) ;
      WB(1,0) = dz * m_W(1,0) ;
      WB(2,0) = dz * m_W(2,0) ;
      WB(0,1) = dz * m_W(0,1) ;
      WB(1,1) = dz * m_W(1,1) ;
      WB(2,1) = dz * m_W(2,1) ;
      WB(0,2) = m_W(0,0) ;
      WB(1,2) = m_W(1,0) ;
      WB(2,2) = m_W(2,0) ;
      WB(0,3) = m_W(0,1) ;
      WB(1,3) = m_W(1,1) ;
      WB(2,3) = m_W(2,1) ;
      WB(0,4) = m_W(0,2) ;
      WB(1,4) = m_W(1,2) ;
      WB(2,4) = m_W(2,2) ;
      m_WBG = WB*m_G ;
      
      // Weight matrix after minimization to momentum
      //Gaudi::SymMatrix5x5 GB = m_G - ROOT::Math::Similarity( m_G*m_B, m_W) ;      
      typedef ROOT::Math::SMatrix<double, 5, 3> Matrix5x3;
      typedef ROOT::Math::SMatrix<double, 3, 5> Matrix3x5;
      typedef ROOT::Math::SMatrix<double, 2, 5> Matrix2x5;
      Matrix5x3 GBtmp ;
      int i,k,l ;
      for(i=0; i<5; ++i) {
	GBtmp(i,0) = dz * m_G(i,0) + m_G(i,2) ;
	GBtmp(i,1) = dz * m_G(i,1) + m_G(i,3) ;
	GBtmp(i,2) = m_G(i,4) ;
      }
      // now we compute the symmetric matrix
      //   Gaudi::SymMatrix5x5 GB = m_G - ROOT::Math::Similarity( GBtmp, m_W) ;
      // we need only two columns, so we write it out. this makes less
      // difference than I had hopes.
      Matrix2x5 GB ;
      GB(0,0) = m_G(0,0) ;
      for(i=1; i<5; ++i) {
	GB(0,i) = m_G(0,i) ;
	GB(1,i) = m_G(1,i) ;
      }
      for(k=0; k<3; ++k) 
	for(l=0; l<3; ++l) {
	  for(i=1; i<5; ++i) {
	    GB(0,i) -= ( GBtmp(0,k) * m_W(k,l) * GBtmp(i,l) ) ;
	    GB(1,i) -= ( GBtmp(1,k) * m_W(k,l) * GBtmp(i,l) ) ;
	  }
	  GB(0,0) -= ( GBtmp(0,k) * m_W(k,l) * GBtmp(0,l) ) ;
	}
      GB(1,0) = GB(0,1) ;
      
      // Compute sub part of A^T G_B that matters
      Matrix3x5 ATGB ; // = ATsub * GBsub ;
      for(i=0; i<5; ++i) {
	ATGB(0,i) = GB(0,i) ;
	ATGB(1,i) = GB(1,i) ;
	ATGB(2,i) = -m_q(0) * GB(0,i) - m_q(1) * GB(1,i) ;
      }
      
      // now add to the derivatives
      halfDChisqDX        -= m_weight * ATGB*residual(pos) ;
      halfD2ChisqDX2(0,0) += m_weight * ATGB(0,0) ;
      halfD2ChisqDX2(0,1) += m_weight * ATGB(0,1) ;
      halfD2ChisqDX2(1,1) += m_weight * ATGB(1,1) ;
      halfD2ChisqDX2(0,2) += m_weight * (- m_q(0) * ATGB(0,0) - m_q(1) * ATGB(0,1)) ;
      halfD2ChisqDX2(1,2) += m_weight * (- m_q(0) * ATGB(1,0) - m_q(1) * ATGB(1,1)) ;
      halfD2ChisqDX2(2,2) += m_weight * (- m_q(0) * ATGB(2,0) - m_q(1) * ATGB(2,1)) ;
      
      // reset cache of variables that we calculate only on demand
      m_WBGA.first = m_momcov.first = m_momposcov.first = false ;
      return m_status ;
    }
  }

  TrackStateVertex::~TrackStateVertex()
  {
    std::for_each(m_tracks.begin(),m_tracks.end(),TrackFunctor::deleteObject()) ; 
  }

  TrackStateVertex&
  TrackStateVertex::operator=(const TrackStateVertex& rhs)
  {
    m_pos       = rhs.m_pos;
    m_poscov    = rhs.m_poscov ;
    //m_posweight = rhs.m_posweight ;
    m_mommomcov = rhs.m_mommomcov ;
    m_fitStatus = rhs.m_fitStatus ;
    m_error     = rhs.m_error ;
    m_chi2      = rhs.m_chi2 ;
    // first delete any existing tracks
    if( ! m_tracks.empty() ) {
      std::for_each(m_tracks.begin(),m_tracks.end(),TrackFunctor::deleteObject()) ; 
      m_tracks.clear() ;
    }
    const size_t N = rhs.m_tracks.size();
    m_tracks.resize( N ) ;
    for(size_t i=0; i<N; ++i) m_tracks[i] = new VertexTrack(*(rhs.m_tracks[i])) ;
    m_refpos    = rhs.m_refpos ;
    m_refweight = rhs.m_refweight ;
    return *this ;
  }

  TrackStateVertex::TrackStateVertex(const TrackStateVertex& rhs)
    : m_pos(rhs.m_pos),
      m_poscov(rhs.m_poscov),
      m_mommomcov(rhs.m_mommomcov),
      m_fitStatus(rhs.m_fitStatus),
      m_error(rhs.m_error),
      m_chi2(rhs.m_chi2),
      m_refpos(rhs.m_refpos),
      m_refweight(rhs.m_refweight)
      //, m_posweight(rhs.m_posweight)
  {
    const size_t N = rhs.m_tracks.size();
    m_tracks.resize( N ) ;
    for(size_t i=0; i<N; ++i) m_tracks[i] = new VertexTrack(*(rhs.m_tracks[i])) ;
  }
  
  TrackStateVertex::TrackStateVertex()
    : m_fitStatus(UnFitted), m_error(NoError), m_chi2(-1)
  {
    for(size_t i=0; i<PositionParameters::kSize; ++i) m_pos(i) = 0 ;
  }
  
  TrackStateVertex::TrackStateVertex( const std::vector<const LHCb::State*>& states,
				      double maxdchisq, size_t maxnumiter)
    : m_fitStatus(UnFitted), m_error(NoError), m_chi2(-1)
  {
    for(size_t i=0; i<PositionParameters::kSize; ++i) m_pos(i) = 0 ;
    for( std::vector<const LHCb::State*>::const_iterator istate = states.begin() ;
	 istate != states.end(); ++istate ) 
      addTrack( **istate ) ;
    fit( maxdchisq, maxnumiter ) ;
  }

  TrackStateVertex::TrackStateVertex( const LHCb::State& stateA,
				      const LHCb::State& stateB,
				      double maxdchisq, size_t maxnumiter)
    : m_fitStatus(UnFitted), m_error(NoError), m_chi2(-1)
  {
    for(size_t i=0; i<PositionParameters::kSize; ++i) m_pos(i) = 0 ;
    addTrack( stateA ) ;
    addTrack( stateB ) ;
    fit( maxdchisq, maxnumiter ) ;
  }

  TrackStateVertex::TrackStateVertex( const Gaudi::XYZPoint& reference,
				      const Gaudi::SymMatrix3x3& refcovariance,
				      bool isweightmatrix )
    : m_poscov(refcovariance), m_fitStatus(UnFitted), m_error(NoError), m_chi2(-1)
  {
    m_refpos(0) =  reference.x() ;
    m_refpos(1) =  reference.y() ;
    m_refpos(2) =  reference.z() ;
    m_refweight = refcovariance ;
    if(!isweightmatrix) m_refweight.InvertChol() ;
    m_pos    = m_refpos ;
  }
  
  void TrackStateVertex::addTrack( const LHCb::State& inputstate,
				   const Gaudi::TrackVector& reference)
  {
    m_tracks.push_back( new VertexTrack( inputstate, m_poscov) ) ;
    if( m_tracks.back()->status() != TrackVertexHelpers::VertexTrack::IsOK ) {
      m_fitStatus = FitFailure ;
      m_error |= BadInputTrack ;
    } else if( m_fitStatus != FitFailure ) {
      // make sure thet fit is flagged as unfitted
      m_fitStatus = UnFitted ;
      // set the reference, used in the _first_ iteration only
      for(int i=0; i<3; ++i) m_tracks.back()->mom()(i) = reference(i+2) ;
    }
  }
  
  void TrackStateVertex::setInputState(size_t i, const LHCb::State& inputstate)
  {
    m_fitStatus = UnFitted ;
    if( i>=m_tracks.size() ) m_tracks.resize(i+1,0) ;
    if( m_tracks[i] == 0 ) {
      m_tracks[i] = new VertexTrack( inputstate, m_poscov ) ;
    } else {
      m_tracks[i]->setState( inputstate ) ;
    }
  }

  void TrackStateVertex::initPos()
  {
    const int N = m_tracks.size() ;
    if( N==2 ) {
      // we gain about one iteration by initializing the position with the doca
      Gaudi::XYZPoint docapoint  ;
      int success = LHCb::TrackVertexUtils::poca(m_tracks[0]->state(), m_tracks[1]->state(), docapoint ) ;
      if( !success ) m_fitStatus = FitFailure ;
      m_pos(0) = docapoint.x();
      m_pos(1) = docapoint.y();
      m_pos(2) = docapoint.z();
    } else {
      // return the average state of the positions. this is what we
      // used to do, and I don't want things to break. it isn't quite
      // optimal though.
      m_pos(0) = m_pos(1) = m_pos(2) = 0 ;
      for(int i=0; i<N; ++i) {
	m_pos(0) += m_tracks[i]->state().x() ;
	m_pos(1) += m_tracks[i]->state().y() ;
	m_pos(2) += m_tracks[i]->state().z() ;
      }
      m_pos(0) /= N ;
      m_pos(1) /= N ;
      m_pos(2) /= N ;
    }
  }
  
  double TrackStateVertex::fitOneStep()
  {
    if(!hasReference() ) {
      // check that we have enough constraints
      if( m_tracks.size() <2 ) {
	m_fitStatus = FitFailure ;
	m_error |= InsufficientTracks ;
      } 
      // initialize the position. FIXME: replace FP comparison with a flag.
      else if( m_pos(0)==0 ) {
	initPos() ;
      }
    }
    
    // This implements the Billoir-Fruhwirth-Regler algorithm.
    // adds the reference position
    Gaudi::SymMatrix3x3 m_posweight = m_refweight ;
    ROOT::Math::SVector<double,3> halfDChisqDX = m_refweight * (m_pos - m_refpos ) ;
    // add all the tracks
    for( VertexTrackContainer::iterator itrack = m_tracks.begin() ;
         itrack != m_tracks.end() && m_fitStatus != FitFailure ; ++itrack ) {
      (*itrack)->projectFast( m_pos, halfDChisqDX, m_posweight ) ;
      if( (*itrack)->status() != TrackVertexHelpers::VertexTrack::IsOK ) {
	m_fitStatus = FitFailure ;
	m_error = ProjectionFailure ;
      }
    }

    if( m_fitStatus == FitFailure ) return -1 ;
    
    // calculate the covariance and the change in the position
    double dchisq = -1 ;
    m_chi2      = -1 ;
    m_poscov = m_posweight ;
    const bool ok = m_poscov.InvertChol();
    if( !ok ) {
      m_fitStatus = FitFailure ;
      m_error = InversionFailure ;
    } else {
      ROOT::Math::SVector<double,3> dpos = - m_poscov * halfDChisqDX ;
      // update the position
      m_pos += dpos ;
      // update the momenta
      for( VertexTrackContainer::const_iterator itrack = m_tracks.begin() ;
	   itrack != m_tracks.end(); ++itrack ) 
	(*itrack)->updateSlopes(m_pos) ;
      // return the delta-chisquare
      dchisq = ROOT::Math::Dot(dpos,halfDChisqDX) ;
      m_fitStatus = FitSuccess ;
    }
    return dchisq ;
  }
  
  double TrackStateVertex::chi2() const
  {
    if( m_chi2 < 0 ) {
      m_chi2 = 0 ;
      for( VertexTrackContainer::const_iterator itrack = m_tracks.begin() ;
	   itrack != m_tracks.end(); ++itrack ) 
	m_chi2 += (*itrack)->chisq(m_pos) ;
      if(hasReference()) 
	m_chi2 += ROOT::Math::Similarity(m_refweight,(m_pos - m_refpos )) ;
    }
    return m_chi2 ;
  }

  double TrackStateVertex::biasedChi2(size_t i) const
  {
    return m_tracks[i]->chisq(m_pos) ;
  }
  
  double TrackStateVertex::weight(size_t i) const
  {
    return m_tracks[i]->weight() ;
  }

  TrackStateVertex::FitStatus TrackStateVertex::fit( double maxdchisq, size_t maxnumiter)
  {
    bool converged(false) ;
    size_t iter(0) ;
    m_fitStatus = FitSuccess ;
    for(; iter<maxnumiter && !converged && m_fitStatus == FitSuccess; ++iter) {
      double dchisq = fitOneStep() ;
      converged = -dchisq < maxdchisq ;
    }
    if( ! converged ) {
      m_fitStatus = FitFailure ;
      m_error = NotConverged ;
      //std::cout << "TrackStateVertex fit failure: not converging" << std::endl ;
    }
    return m_fitStatus ;
  }

  TrackStateVertex::FitStatus TrackStateVertex::fitAdaptive( double maxtrkchi2,
							     double maxdchisq, size_t maxnumiter)
  {
    bool finished(false) ;
    double prevtotalchi2(-1) ;
    size_t N = m_tracks.size() ;
    // since we don't compute unbiased chi2, we need to rescale
    const double scaledmaxtrkchi2 = maxtrkchi2 * (2*N - 3 ) / (2*N) ;
    size_t niter(0) ;
    std::vector<double> trkchi2s(N,0) ;
    while( !finished ) {
      fit( maxdchisq, maxnumiter) ;
      if( m_fitStatus == FitFailure ) {
	finished = true ; 
      } else {
	++niter ; //niter += m_nIter ;
	// compute the total chi2, cache contribution of each track
	m_chi2 = ROOT::Math::Similarity(m_refweight,(m_pos - m_refpos)) ;
	for(size_t i=0; i<N; ++i) {
	  double trkchi2 = m_tracks[i]->chisq(m_pos) ;
	  m_chi2 += m_tracks[i]->weight() * trkchi2 ;
	  trkchi2s[i] = trkchi2 ;
	}
	if( niter>1 && prevtotalchi2 - m_chi2 < maxdchisq ) {
	  finished = true ;
	} else if ( niter >= maxnumiter ) {
	  finished = true ;
	  // We have some convergence problems. Decided not to flag these as failed.
	  //m_fitStatus = FitFailure ;
	  // std::cout << "Adaptive fit failed" << std::endl ;
	  m_error = NotConvergedAdaptive ;
	} else {
	  prevtotalchi2 = m_chi2 ;
	  double sumw(0) ;
	  for(size_t i=0; i<N; ++i) {
	    // this is one way: these are called 'Huber' weights, at least, if you take the sqroot!
	    //double weight = trkchi2s[i] < scaledmaxtrkchi2 ? 1 : scaledmaxtrkchi2 / trkchi2s[i] ;
	    double weight = trkchi2s[i] < scaledmaxtrkchi2 ? 1 : std::sqrt(scaledmaxtrkchi2 / trkchi2s[i]) ;
	    // this is 'tri-weight', more like Tukey
	    //double weight =  trkchi2s[i] < 2 ? 1 
	    // : (trkchi2s[i] >= scaledmaxtrkchi2 ? 0 : (1 - trkchi2s[i] /scaledmaxtrkchi2 ) ) ;
	    m_tracks[i]->setweight(weight) ;
	    sumw += weight ;
	  }
	}
      }
    }
    //m_nIter = niter ;
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
  
  LHCb::State 
  TrackStateVertex::stateAtVertex(size_t i) const
  {
    const VertexTrack& track = *(m_tracks[i]) ;
    LHCb::State state( LHCb::State::Vertex ) ;
    state.setZ( m_pos(2) ) ;
    Gaudi::TrackVector& par = state.stateVector() ;
    Gaudi::SymMatrix5x5& cov = state.covariance() ;
    for(int k=0;k<2; ++k) par(k)   = m_pos(k) ;
    for(int j=0;j<3; ++j) par(j+2) = track.mom()(j) ;
    // now we need to recompute B for this z-position
    const ROOT::Math::SMatrix<double,5,3>& A = track.A() ;
    ROOT::Math::SMatrix<double,5,3> B ;
    B(2,0) = B(3,1) = B(4,2) = 1 ;
    Gaudi::Matrix5x5 tmp = A * ROOT::Math::Transpose(track.momposcov()) * ROOT::Math::Transpose(B) ;
    ROOT::Math::AssignSym::Evaluate( cov, tmp + ROOT::Math::Transpose(tmp) ) ;
    cov += ROOT::Math::Similarity( A, m_poscov ) ;
    cov += ROOT::Math::Similarity( B, track.momcov() ) ;
    return state ;
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
    for( size_t index = 0 ; index<m_tracks.size() ; ++index) 
      if( masshypos[index] >=0 ) {
	Gaudi::Math::geo2LA(m_tracks[index]->mom(),masshypos[index],p4tmp);
	p4sum += p4tmp ;
      }
    return p4sum ;
  }
  
  double TrackStateVertex::mass(const std::vector<double>& masshypos) const 
  {
    return p4(masshypos).M() ;
  }

  Gaudi::SymMatrix4x4 TrackStateVertex::p4CovMatrix(const std::vector<double>& masshypos) const 
  {
    Gaudi::SymMatrix4x4 p4cov ;
    size_t dim = m_tracks.size() ;
    std::vector<Gaudi::Matrix4x3> dP4dMom(dim) ;
    Gaudi::Matrix4x4    tmp4x4 ;
    Gaudi::SymMatrix4x4 stmp4x4 ;
    for( size_t index = 0 ; index<dim ; ++index) 
      if( masshypos[index] >=0 ) {
	Gaudi::Math::JacobdP4dMom(m_tracks[index]->mom(),masshypos[index],dP4dMom[index]);
	p4cov += ROOT::Math::Similarity( dP4dMom[index], m_tracks[index]->momcov() ) ;
	for( size_t jndex = 0 ; jndex<index ; ++jndex) 
	  if( masshypos[jndex] >=0 ) {
	    tmp4x4 = dP4dMom[index] * momMomCovMatrixFast(index,jndex) * ROOT::Math::Transpose(dP4dMom[jndex]);
	    ROOT::Math::AssignSym::Evaluate( stmp4x4, tmp4x4 + ROOT::Math::Transpose( tmp4x4 ) ) ;
	    p4cov += stmp4x4 ;
	  }
      }
    return p4cov ;
  }

  Gaudi::SymMatrix7x7 TrackStateVertex::covMatrix7x7( const std::vector<double>& masshypos ) const
  {
    //< Return the covariance matrix for the V0
    Gaudi::SymMatrix4x4 p4cov ;
    size_t dim = m_tracks.size() ;
    std::vector<Gaudi::Matrix4x3> dP4dMom(dim) ;
    Gaudi::Matrix4x4    tmp4x4 ;
    Gaudi::SymMatrix4x4 stmp4x4 ;
    Gaudi::Matrix4x3 covmompos;
    for( size_t index = 0 ; index<dim ; ++index) 
      if( masshypos[index] >=0 ) {
	// this is something we can still optimize
	Gaudi::Math::JacobdP4dMom(m_tracks[index]->mom(),masshypos[index],dP4dMom[index]);
	p4cov += ROOT::Math::Similarity( dP4dMom[index], m_tracks[index]->momcov() ) ;
	covmompos += dP4dMom[index] * m_tracks[index]->momposcov() ;
	for( size_t jndex = 0 ; jndex<index ; ++jndex) 
	  if( masshypos[jndex] >=0 ) {
	    tmp4x4 = dP4dMom[index] * momMomCovMatrixFast(index,jndex) * ROOT::Math::Transpose(dP4dMom[jndex]);
	    ROOT::Math::AssignSym::Evaluate( stmp4x4, tmp4x4 + ROOT::Math::Transpose( tmp4x4 ) ) ;
	    p4cov += stmp4x4 ;
	  }
      }
    
    // finally copy everything to the 7x7 matrix
    Gaudi::SymMatrix7x7 cov7x7;
    // position part from rec vertex
    cov7x7.Place_at(m_poscov, 0, 0);
    // momentum part
    cov7x7.Place_at(p4cov, 3,3);
    // off diagonal elements
    for(int i = 0 ; i <4; ++i)
      for(int j =0;j<3;++j)
        cov7x7(i+3,j) = covmompos(i,j);
    
    return cov7x7 ;
  }
  
  double TrackStateVertex::massErr(const std::vector<double>& masshypos) const 
  {  
    Gaudi::SymMatrix4x4 p4cov = p4CovMatrix( masshypos ) ;
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
                                                              double constrainedmass, double naturalwidth)
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
    for( size_t index = 0 ; index< dim ; ++index) 
      if( masshypos[index] >= 0 ) {
	Gaudi::Math::geo2LA(m_tracks[index]->mom(),masshypos[index],p4[index]);
	p4sum += p4[index] ;
	Gaudi::Math::JacobdP4dMom(m_tracks[index]->mom(),masshypos[index],dP4dMom[index]);
	p4cov += ROOT::Math::Similarity( dP4dMom[index], m_tracks[index]->momcov() ) ;
	for( size_t jndex = 0 ; jndex<index ; ++jndex) 
	    if( masshypos[jndex] >= 0 ) {
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
    // now add the natural width
    R(0,0) += 4* constrainedmass2 * naturalwidth * naturalwidth ;
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
