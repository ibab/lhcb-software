#ifndef TRACKKERNEL_TrackStateVertex_H
#define TRACKKERNEL_TrackStateVertex_H 1

// Include files
#include "Event/State.h"
#include "GaudiKernel/Vector4DTypes.h"

namespace LHCb 
{

  // Forward declarations
  class TwoProngVertex ;
  namespace TrackVertexHelpers {
    class VertexTrack ;
  }

  /** @class TrackStateVertex TrackStateVertex.h
   *
   * Vertex(ing) class for multitrack vertices
   *
   * @author Wouter Hulsbergen
   * created Wed Dec 12 14:58:51 2007
   * 
   */

  class TrackStateVertex
  {
  public:
    ///
    enum FitStatus { FitSuccess, FitFailure, UnFitted } ;

    /// Sets minimal data content for useable vertex. The rest we do with setters.
    TrackStateVertex() ;    

    /// Construct vertex from set of states. also fits the vertex.
    TrackStateVertex(const std::vector<const LHCb::State*>& states, double maxdchisq=0.01, size_t maxiterations=10) ;

    /// Construct from a reference vertex. Then add track states
    /// later. If you use inverse of cov matrix, specify 'isweight=true'.
    TrackStateVertex(const Gaudi::XYZPoint& refposition, const Gaudi::SymMatrix3x3& refcovariance, bool isweight=false) ;
 
    /// Copy constructor
    TrackStateVertex(const TrackStateVertex& vertex) ;

    /// Destructor
    ~TrackStateVertex() ;

    /// assignment
    TrackStateVertex& operator=(const TrackStateVertex& vertex) ;

    /// add a track. invalidates any existing fit. (call 'fitOneStep' afterwards)
    void addTrack( const LHCb::State& state, const Gaudi::TrackVector& reference ) ;
    
    /// add a track. invalidates any existing fit. (call 'fit' afterwards)
    void addTrack( const LHCb::State& state ) { addTrack(state,state.stateVector()) ; }
    
    /// Set the input state for track i. invalidates any existing fit. (call 'fit' afterwards)
    void setTrack( size_t i, const LHCb::State& state ) { setInputState(i,state) ; }

    /// fit a single iteration. returns the delta-chisquare.
    double fitOneStep() ;

    /// fit until converged
    FitStatus fit( double maxdchisq=0.01, size_t maxiterations=10) ;

    /// adapative fit. downweight tracks with chi2 contribution larger than maxtrkchi2
    FitStatus fitAdaptive( double maxtrkchi2, double maxdchisq=0.01, size_t maxiterations=10) ;

    /// return the fit status
    FitStatus fitStatus() const { return m_fitStatus ; }

    /// calculate the chisquare of the vertex fit
    double chi2() const ;

    /// number of dofs in vertex fit
    int nDoF() const { return nTracks()*2 - 3 + ( hasReference() ? 3 : 0 ) ; }

    /// number of tracks in vertex
    size_t nTracks() const { return m_tracks.size() ; }

    /// Fitted state vector of track i
    Gaudi::TrackVector stateVector(size_t i) const ;
    /// Fitted covariance of track i
    Gaudi::TrackSymMatrix stateCovariance(size_t i) const ;
    /// Correlation matrix between state i and j
    Gaudi::Matrix5x5 stateCovariance(size_t i, size_t j) const ;
    /// Fitted state for track i
    LHCb::State state(size_t i) const ;
    /// Fitted state at vertex z position for track i
    LHCb::State stateAtVertex(size_t i) const ;
    /// Input state for track i
    const LHCb::State& inputState(size_t i) const ;
    /// Set the input state for track i
    void setInputState( size_t i, const LHCb::State& state ) ;
    /// Position of the vertex
    Gaudi::XYZPoint position() const { return Gaudi::XYZPoint(m_pos(0),m_pos(1),m_pos(2)) ; }
    /// Position covariance of the vertex
    const Gaudi::SymMatrix3x3& covMatrix() const { return m_poscov ; }
    /// Momentum (tx,ty,q/pc) of track i
    const ROOT::Math::SVector<double,3>& mom( size_t i) const ;
    /// Momentum (tx,ty,q/pc) covariance matrix of track i
    const Gaudi::SymMatrix3x3& momCovMatrix( size_t i ) const ;
    /// Covariance matrix for momentum i and position
    const ROOT::Math::SMatrix<double,3,3>& momPosCovMatrix(size_t i) const ;
    /// Covariance matrix for momentum i and momentum j (j<=i)
    const Gaudi::Matrix3x3& momMomCovMatrixFast(size_t i, size_t j) const {
      return m_mommomcov.empty() ? computeMomMomCov(i,j) : m_mommomcov[symIndex(i,j)] ; }
    /// Covariance matrix for momentum i and momentum j
    Gaudi::Matrix3x3 momMomCovMatrix(size_t i, size_t j) const {
      return j<=i ? momMomCovMatrixFast(i,j) : ROOT::Math::Transpose( momMomCovMatrixFast(j,i) ) ; }  
    /// Get the total p4 given the mass hypotheses
    Gaudi::LorentzVector p4(const std::vector<double>& masshypos) const ;
    /// Get the total p4 given the mass hypotheses
    Gaudi::SymMatrix4x4 p4CovMatrix(const std::vector<double>& masshypos) const ;
    /// Get the cov7x7
    Gaudi::SymMatrix7x7 covMatrix7x7( const std::vector<double>& masshypos ) const ;
    /// Get the mass given a set of mass hypotheses
    double mass(const std::vector<double>& masshypos) const ;
    /// Get the error on the mass given a set of mass hypotheses
    double massErr(const std::vector<double>& masshypos) const ;
    /// Add a mass constraint
    FitStatus constrainMass(const std::vector<double>& masshypos,double constrainedmass, double naturalwidth=0.) ;

    /// Projection matrix for state i to vertex position (matrix A in Fruhwirth)
    const ROOT::Math::SMatrix<double,5,3>& matrixA(size_t i) const ;
    /// Projection matrix for state i to momentum i (matrix B in Fruhwirth)
    const ROOT::Math::SMatrix<double,5,3>& matrixB(size_t i) const ;

    /// the biased chisquare contribution of a single track. (the
    /// unbiased one I still need to compute)
    double biasedChi2( size_t i ) const ;

    /// get the weight of a track after the adaptive fit
    double weight( size_t i ) const ;

  private:
    size_t symIndex( size_t i, size_t j ) const { return i*(i+1)/2 + j ; }
    const Gaudi::Matrix3x3& computeMomMomCov(size_t i, size_t j) const ;
    bool hasReference() const { return (m_refweight(0,0)+m_refweight(1,1)+m_refweight(2,2))>0 ; }
  private:
    typedef TrackVertexHelpers::VertexTrack VertexTrack ;
    typedef std::vector< TrackVertexHelpers::VertexTrack* > VertexTrackContainer ;
    typedef ROOT::Math::SVector<double,3> MomentumParameters ;
    typedef ROOT::Math::SVector<double,3> PositionParameters ;
    typedef Gaudi::SymMatrix3x3           PositionCovariance ;
    typedef Gaudi::SymMatrix3x3           MomentumCovariance ;
    VertexTrackContainer m_tracks ;
    PositionParameters m_pos ;
    PositionCovariance m_poscov ;
    mutable std::vector< Gaudi::Matrix3x3 > m_mommomcov ;
    FitStatus m_fitStatus ;
    mutable double m_chi2 ;
    PositionParameters m_refpos ;    // position of reference position
    PositionCovariance m_refweight ; // weight (inverse cov) of reference position
  } ;
  
} // namespace LHCb;

#endif
