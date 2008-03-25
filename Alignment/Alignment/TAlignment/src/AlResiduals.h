// $Id: AlResiduals.h,v 1.1 2008-03-25 22:02:09 wouter Exp $
#ifndef TRACKTOOLS_TrackResiduals_H 
#define TRACKTOOLS_TrackResiduals_H 1

// Include files
// -------------

// from TrackEvent
#include "Event/Track.h"
#include "Kernel/LHCbID.h"

#include <vector>
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/DiagMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

namespace Al
{
  
  class Residuals
  {
  public:
    typedef std::vector<const LHCb::Node*> NodeContainer ;
    Residuals(const NodeContainer&, double chisq, size_t ndof) ;
    Residuals(size_t size = 0)  : m_nodes(size), m_r(size), m_V(size), m_HCH(size) {}

    size_t size() const { return m_nodes.size() ; }
    double r(size_t i) const { return m_r(i+1) ; }
    double V(size_t i) const { return m_V(i+1) ; }
    double V(size_t i, size_t j) const { return i==j ? V(i) : 0 ; }
    double R(size_t i, size_t j) const { return V(i,j) - HCH(i,j) ; }
    double HCH(size_t i, size_t j) const { return m_HCH(i+1,j+1) ; }
    double HCH_norm(size_t i, size_t j) const { return m_HCH(i+1,j+1)/sqrt( V(i) * V(j) ) ; }
	
    const LHCb::Node& node(size_t i) const { return *(m_nodes[i]) ; }
    size_t index(const LHCb::Node& node) const ;
//     double HCH_norm(const LHCb::Node& inode, const LHCb::Node& jnode) const {
//       size_t i(index(inode)), j(index(jnode)) ;
//       return HCH(i,j)/sqrt( V(i) * V(j) ) ;
//     }
    const NodeContainer& nodes() const { return m_nodes ; }
    double chi2() const { return m_chi2 ; }
    int nDoF() const { return m_nDoF ; }

  private:
    friend class TrackResidualTool ;
    friend class VertexResidualTool ;
    NodeContainer m_nodes ;
    CLHEP::HepVector     m_r ;   // vector of residuals
    CLHEP::HepVector     m_V ;   // measurement cov matrix
    CLHEP::HepSymMatrix  m_HCH ; // matrix HCH
    double m_chi2 ;
    int m_nDoF ;
  };

  class TrackResiduals : public Residuals
  {
  public:
    // typedefs, enums
    typedef std::vector< Gaudi::TrackProjectionMatrix > ProjectionMatrix ;
    
    /// Standard constructor
    TrackResiduals(const LHCb::Track& track,
		   const NodeContainer& nodes) ;
    const LHCb::State& state() const { return m_state ; }
    const ProjectionMatrix& residualStateCov() const { return m_residualStateCov ; }
  private:
    friend class TrackResidualTool ;
    LHCb::State m_state ;                  // most upstream state
    //CLHEP::HepMatrix m_residualStateCov  ; // correlation matrix between that state and the residuals
    ProjectionMatrix m_residualStateCov ;
  };
  
  class MultiTrackResiduals : public Residuals
  {
  public:
    /// Standard constructor
    MultiTrackResiduals(const NodeContainer& nodes,
			double chi2, int ndof,
			double vertexchi2, int vertexndof) 
      : Residuals(nodes,chi2,ndof),m_vertexchi2(vertexchi2),m_vertexndof(vertexndof) {}
    ~MultiTrackResiduals( ) {}; ///< Destructor
  private:
    friend class VertexResidualTool ;
    double m_vertexchi2 ;
    int m_vertexndof ;
  };
  
}

#endif
