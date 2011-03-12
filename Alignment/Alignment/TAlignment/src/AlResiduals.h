// $Id: AlResiduals.h,v 1.7 2010-05-03 12:43:38 wouter Exp $
#ifndef TRACKTOOLS_TrackResiduals_H 
#define TRACKTOOLS_TrackResiduals_H 1

// Include files
// -------------

// from TrackEvent
#include "Event/Track.h"
#include "Event/Node.h"
#include "Kernel/LHCbID.h"
#include <vector>

// forward declarations
namespace LHCb {
  class Track ;
  class Node ;
}
class AlignmentElement ;

namespace Al
{
  class Residual1D
  {
  public:
    typedef Gaudi::TrackProjectionMatrix TrackProjectionMatrix ;
    typedef ROOT::Math::SMatrix<double,1,3> ResidualVertexMatrix ;
    Residual1D(const LHCb::Node& n, const AlignmentElement& e ) ;
    const LHCb::Node& node() const { return *m_node ; }
    const AlignmentElement& element() const { return *m_element ; }
    double r() const { return m_r ; }
    double V() const { return m_V ; }
    double HCH() const { return m_HCH ; }
    double R() const { return V() - HCH() ; }
    double chi2() const { return m_r*m_r/R() ; }
    void setHCH( double hch ) { m_HCH = hch ; }
    void setR( double r ) { m_r = r ; }
    const TrackProjectionMatrix& H() const { return node().projectionMatrix() ; }
    //TrackProjectionMatrix& residualStateCov() { return m_residualStateCov; }
    const TrackProjectionMatrix& residualStateCov() const { return m_residualStateCov; }
    void setResidualStateCov(const TrackProjectionMatrix& c) { m_residualStateCov = c ; }
    
    //ResidualVertexMatrix& residualVertexCov() { return m_residualVertexCov; }
    const ResidualVertexMatrix& residualVertexCov() const { return m_residualVertexCov; }    
    void setResidualVertexCov(const ResidualVertexMatrix& c ) { m_residualVertexCov  = c ; }

  private:
    const LHCb::Node* m_node ;
    const AlignmentElement* m_element ;
    double m_r ;
    double m_V ;
    double m_HCH ;
    TrackProjectionMatrix m_residualStateCov ;  // correlation between residual and reference state
    ResidualVertexMatrix  m_residualVertexCov ; // correlation between residual and vertex
  } ;
  
  struct CovarianceElement
  {
    CovarianceElement(size_t r, size_t c, double v) : row(r),col(c),val(v) {}
    size_t row ;
    size_t col ;
    double val ;
    double operator*(double x) const { return val * x ; }
    double operator+=(double x) { return val += x ; }
  } ;
  
  class Residuals
  {
  public:
    typedef std::vector<Residual1D>        ResidualContainer ;
    typedef std::vector<CovarianceElement> CovarianceContainer ;

    Residuals(double chisq, size_t ndof, size_t numexthits=0) ;
    
    size_t size() const { return m_residuals.size() ; }
    double chi2() const { return m_chi2 ; }
    int nDoF() const { return m_nDoF ; }
    size_t nExternalHits() const { return m_nExternalHits ; }
    const CovarianceContainer& HCHElements() const { return m_HCHElements ; }
    const CovarianceContainer& hch() const { return m_HCHElements ; }
    const ResidualContainer& residuals() const { return m_residuals ; }
    const Residual1D& residual(size_t i) const { return m_residuals[i] ; }
    bool testHCH( std::string& message ) const ;
    size_t nAlignables() const ;
  private:
    friend class TrackResidualTool ;
    friend class VertexResidualTool ;
    ResidualContainer   m_residuals ;
    CovarianceContainer m_HCHElements ; // off-diagonal elements of matrix HCH
    double m_chi2 ;
    int m_nDoF ;
    size_t m_nExternalHits ;
  };

  class TrackResiduals : public Residuals
  {
  public:
    // typedefs, enums
    typedef std::vector< Gaudi::TrackProjectionMatrix > ProjectionMatrix ;
    
    /// Standard constructor
    TrackResiduals(const LHCb::Track& track) : Residuals(track.chi2(), track.nDoF()), m_track(&track) {}
    const LHCb::State& state() const { return m_state ; }
    const LHCb::Track& track() const { return *m_track ; }
  private:
    friend class TrackResidualTool ;
    LHCb::State m_state ;                  // most upstream state
    const LHCb::Track* m_track ;
  };
  
  class MultiTrackResiduals : public Residuals
  {
  public:
    /// Standard constructor
    MultiTrackResiduals(double chi2, int ndof, size_t numexthits,
			std::vector<const TrackResiduals*>& tracks,
			double vertexchi2, int vertexndof) 
      : Residuals(chi2,ndof,numexthits),m_tracks(tracks),m_vertexchi2(vertexchi2),m_vertexndof(vertexndof) {}
    ~MultiTrackResiduals( ) {}; ///< Destructor
    double vertexChi2() const { return m_vertexchi2 ; }
    int    vertexNDoF() const { return m_vertexndof ; }
    size_t numTracks() const { return m_tracks.size() ; }
  private:
    friend class VertexResidualTool ;
    std::vector<const TrackResiduals*> m_tracks ;
    double m_vertexchi2 ;
    int    m_vertexndof ;
  };
  
}

#endif
