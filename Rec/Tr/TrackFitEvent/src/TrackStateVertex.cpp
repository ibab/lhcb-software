#include "Event/TrackStateVertex.h"
#include "Event/TrackFunctor.h"
#include "Event/TwoProngVertex.h"

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
      
      /// momentum-position covariance ('E' in Fruhwirth)
      const ROOT::Math::SMatrix<double, 3, 3>& momposcov() const {
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
      const ROOT::Math::SMatrix<double,3,3>& computeWBGA() const {
	m_WBGA.first = true ; 
	return m_WBGA.second = m_WBG * m_A ; 
      }
      
      const Gaudi::SymMatrix3x3& computeMomCov() const {
	m_momcov.first = true ;
	ROOT::Math::AssignSym::Evaluate(m_momcov.second , -momposcov() * ROOT::Math::Transpose(WBGA())) ;
	m_momcov.second += m_W ;
      	return m_momcov.second ;
      }
      
      const ROOT::Math::SMatrix<double, 3, 3>& computeMomPosCov() const {
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
      m_G.Invert() ;
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
      m_W.Invert() ;
      
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
    m_tracks.push_back( new VertexTrack( inputstate, m_poscov ) ) ;
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
    m_poscov.Invert() ;
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
    
    Gaudi::Matrix3x3 mommomcov = track1.WBGA() * m_poscov * ROOT::Math::Transpose(track2.WBGA()) ;
    
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

  LHCb::TwoProngVertex* TrackStateVertex::createTwoProngVertex(bool computemomcov) const 
  {
    LHCb::TwoProngVertex* vertex = 
      new LHCb::TwoProngVertex(Gaudi::XYZPoint(m_pos(0),m_pos(1),m_pos(2))) ;
    vertex->setChi2(chi2()) ;
    vertex->setCovMatrix( m_poscov ) ;
    vertex->setNDoF(m_tracks.size()*2-3) ;
    vertex->momA() = m_tracks.front()->mom() ;
    vertex->momB() = m_tracks.back()->mom() ;
    if( computemomcov ) {
      vertex->momcovA()    = m_tracks.front()->momcov() ;
      vertex->momposcovA() = m_tracks.front()->momposcov() ;
      vertex->momcovB()    = m_tracks.back()->momcov() ;
      vertex->momposcovB() = m_tracks.back()->momposcov() ;
      vertex->mommomcov()  = m_tracks.front()->WBGA() * m_poscov * ROOT::Math::Transpose(m_tracks.back()->WBGA()) ;
    }
    return vertex ;
  }
}
