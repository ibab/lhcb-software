 // $Id: TrackVertexer.cpp,v 1.1 2007-12-03 12:56:58 wouter Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackVertexer.h"

// from Event
#include "Event/TwoProngVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackVertexer
//
// 2007-11-07 : Wouter HULSBERGEN
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackVertexer );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackVertexer::TrackVertexer( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_maxNumIter(5),
    m_maxDChisq(0.05),
    m_computeMomCov(true)
{
  declareInterface<ITrackVertexer>(this);
  declareProperty("MaxNumberOfIterations",m_maxNumIter) ;
  declareProperty("MaxDeltaChisqForConvergence",m_maxDChisq) ;
  declareProperty("ComputeMomentumCovariance",m_computeMomCov) ;
}
//=============================================================================
// Destructor
//=============================================================================
TrackVertexer::~TrackVertexer() {} 

namespace TrackVertexerHelpers
{

  class VertexTrack
  {
  public:
    VertexTrack( const LHCb::State& state ) ;
    
    void project( const ROOT::Math::SVector<double,3>& position,
                  //double& chisq,
                  ROOT::Math::SVector<double,3>& halfDChisqDX,
                  Gaudi::SymMatrix3x3&           halfD2ChisqDX2) ;
    
    /// derivative of momentum to position (excluding q/p)
    const ROOT::Math::SMatrix<double,3,3>& WBGA() const { return m_WBGA ;}
    
    /// momentum (tx,ty) covariance 
    Gaudi::SymMatrix3x3 D(const Gaudi::SymMatrix3x3& poscov) const {
      return m_W + ROOT::Math::Similarity(WBGA(),poscov) ;
    }

    /// momentum-position covariance
    ROOT::Math::SMatrix<double, 3, 3> E(const Gaudi::SymMatrix3x3& poscov) const { 
      return - WBGA() * poscov ;
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
    ROOT::Math::SVector<double,5> residual(const ROOT::Math::SVector<double,3>& pos) const ;
    
    /// full state after fitting
    void momentum(const Gaudi::SymMatrix3x3& poscov,
		  ROOT::Math::SVector<double,3>& mom,
		  Gaudi::SymMatrix3x3& momcov,
		  ROOT::Math::SMatrix<double,3,3>& posmomcov) ; 
  private:
    const LHCb::State& m_state ;
    
    // In fruhwirth's notation
    Gaudi::SymMatrix5x5 m_G ;               // weight matrix of state p=(x,y,tz,ty)
    ROOT::Math::SMatrix<double,5,3> m_A ;   // projection matrix for vertex position
    ROOT::Math::SMatrix<double,5,3> m_B ;   // projection matrix for slopes (tx,ty, qop)
    ROOT::Math::SVector<double,3>   m_q ;   // predicted/fitted slope (tx,ty)
    Gaudi::SymMatrix3x3 m_W ;               // cov matrix of slopes for fixed position
    ROOT::Math::SMatrix<double, 3, 5> m_WBG  ; // derivative dmom/dresidual  
    ROOT::Math::SMatrix<double, 3, 3> m_WBGA ; // be areful with caching here ...
  } ;
  
  VertexTrack::VertexTrack(const LHCb::State& state)
    : m_state(state)
  {
    // compute the weight matrix (inverse of V)
    m_G = state.covariance() ;
    m_G.Invert() ;
    m_q(0) = state.tx() ;
    m_q(1) = state.ty() ;
    m_q(2) = state.qOverP() ;
  }
  
  inline ROOT::Math::SVector<double,5>
  VertexTrack::residual(const ROOT::Math::SVector<double,3>& pos) const
  {
    double dz = m_state.z() - pos(2) ;
    ROOT::Math::SVector<double,5> res ;
    res(0) = m_state.x() - ( pos(0) + dz * m_q(0)) ;
    res(1) = m_state.y() - ( pos(1) + dz * m_q(1)) ;
    res(2) = m_state.tx() - m_q(0) ;
    res(3) = m_state.ty() - m_q(1) ;
    res(4) = m_state.qOverP() - m_q(2) ;
    return res ;
  }
 
  void VertexTrack::project( const ROOT::Math::SVector<double,3>& pos,
                                     //double& chisq,
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
  }

  /// full state after fitting
  void VertexTrack::momentum(const Gaudi::SymMatrix3x3& poscov,
                             ROOT::Math::SVector<double,3>& mom,
                             Gaudi::SymMatrix3x3& momcov,
                             ROOT::Math::SMatrix<double,3,3>& momposcov)
  {
    m_WBGA = m_WBG * m_A ;
    mom = m_q ;
    momposcov = - m_WBGA * poscov ;
    ROOT::Math::AssignSym::Evaluate(momcov , -momposcov * ROOT::Math::Transpose(m_WBGA) ) ;
    momcov += m_W ;
  }
}

LHCb::TwoProngVertex*
TrackVertexer::fit(const LHCb::State& stateA, const LHCb::State& stateB) const
{
  // This implements the Billoir-Fruhwirth-Regler algorithm. This algorithms works just as well for more-than-two prongs, but
  // there is no usecase for this at the moment (and no data structure to store the result).
  
  
  // position and covariance of the vertex. initalize position with average of states.
  ROOT::Math::SVector<double,3> pos ;
  pos(0) = 0.5*(stateA.x() + stateB.x()) ;
  pos(1) = 0.5*(stateA.y() + stateB.y()) ;
  pos(2) = 0.5*(stateA.z() + stateB.z()) ;
  Gaudi::SymMatrix3x3 C ;
  
  // track contributions (keep track of correlations to momenta)
  TrackVertexerHelpers::VertexTrack trackentryA(stateA) ;
  TrackVertexerHelpers::VertexTrack trackentryB(stateB) ;
  typedef std::vector<TrackVertexerHelpers::VertexTrack*> VertexTrackContainer ;
  VertexTrackContainer trackentries ;
  trackentries.reserve(2) ;
  trackentries.push_back( &trackentryA ) ;
  trackentries.push_back( &trackentryB ) ;
    
  bool converged(false) ;
  size_t iter(0) ;
  for(; iter<m_maxNumIter &&!converged; ++iter) {
    Gaudi::SymMatrix3x3 halfD2ChisqDX2 ;
    ROOT::Math::SVector<double,3> halfDChisqDX ;
    // add all the tracks
    for( VertexTrackContainer::iterator itrack = trackentries.begin() ;
         itrack != trackentries.end(); ++itrack ) {
      //chisqprev += (*itrack)->chisq(pos) ;
      (*itrack)->project( pos, halfDChisqDX, halfD2ChisqDX2) ;
    }
    // calculate the covariance and the change in the position
    C = halfD2ChisqDX2 ;
    C.Invert() ;
    ROOT::Math::SVector<double,3> dpos = - C * halfDChisqDX ;
    // update the position
    pos += dpos ;
    // update the chisquare
    double dchisq = ROOT::Math::Dot(dpos,halfDChisqDX) ;
    converged = -dchisq < m_maxDChisq ;
    // update the momenta
    for( VertexTrackContainer::const_iterator itrack = trackentries.begin() ;
         itrack != trackentries.end(); ++itrack ) 
      (*itrack)->updateSlopes(pos) ;
    }
  //std::cout << "Number of iterations: " << iter << std::endl ;
  
  // calculate the chisquare
  double chisq(0) ;
  for( VertexTrackContainer::const_iterator itrack = trackentries.begin() ;
       itrack != trackentries.end(); ++itrack ) 
    chisq += (*itrack)->chisq(pos) ;
  
  // copy everything to the RecVertex
  LHCb::TwoProngVertex* vertex = new LHCb::TwoProngVertex(Gaudi::XYZPoint(pos(0),pos(1),pos(2))) ;
  vertex->setChi2(chisq) ;
  vertex->setCovMatrix( C ) ;
  vertex->setNDoF(trackentries.size()*2-3) ;
  if(m_computeMomCov) {
    trackentryA.momentum(C,vertex->momA(),vertex->momcovA(),vertex->momposcovA()) ;
    trackentryB.momentum(C,vertex->momB(),vertex->momcovB(),vertex->momposcovB()) ;
    vertex->mommomcov() = trackentryA.WBGA() * C * ROOT::Math::Transpose(trackentryB.WBGA()) ;
  }
  return vertex ; 
}
