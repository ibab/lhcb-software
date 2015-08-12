#ifndef ALIGNSOLVTOOLS_ALEQUATIONS_H
#define ALIGNSOLVTOOLS_ALEQUATIONS_H

#include <vector>
#include <map>
#include <ostream>
#include <istream>
#include <fstream>
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/Time.h"
  
namespace Al
{
  class OffDiagonalData
  {
  public:
    Gaudi::Matrix6x6 m_matrix ;
    size_t m_numTracks ;
    size_t m_numHits ;
  public:
    OffDiagonalData() : m_matrix(), m_numTracks(0), m_numHits(0) {}
    Gaudi::Matrix6x6& matrix() { return m_matrix ; }
    const Gaudi::Matrix6x6& matrix() const { return m_matrix ; }
    void addTrack() { ++m_numTracks ; }
    size_t numTracks() const { return m_numTracks ; }
    size_t numHits() const { return m_numHits ; }
    template<class T>
    void add( const T& m) {
      m_matrix += m ;
      ++m_numHits ;
    }
    OffDiagonalData& operator +=( const OffDiagonalData& rhs )
    {
      m_matrix += rhs.m_matrix ;
      m_numTracks += rhs.m_numTracks ;
      m_numHits += rhs.m_numHits ;
      return *this ;
    }
  } ;

  class ElementData
  {
  public:
    typedef std::map<size_t, OffDiagonalData> OffDiagonalContainer ;
    typedef ROOT::Math::SMatrix<double,6,5> TrackDerivatives ;
    typedef ROOT::Math::SMatrix<double,6,3> VertexDerivatives ;
    
    ElementData() ;
    void add(const ElementData& rhs) ;
    void transform( const Gaudi::Matrix6x6& jacobian ) ;
    const Gaudi::Vector6& dChi2DAlpha() const { return m_dChi2DAlpha ; }
    const Gaudi::SymMatrix6x6& d2Chi2DAlpha2() const { return m_d2Chi2DAlpha2 ; }
    const OffDiagonalContainer& d2Chi2DAlphaDBeta() const { return m_d2Chi2DAlphaDBeta ; }
    const TrackDerivatives& dStateDAlpha() const { return m_dStateDAlpha ; }
    const VertexDerivatives& dVertexDAlpha() const { return m_dVertexDAlpha ; }
    const Gaudi::Vector6& alpha() const { return m_alpha ; }
    void setAlpha( const Gaudi::Vector6& alpha ) { m_alpha = alpha ; m_alphaIsSet=true ; }
    void setAlignFrame( const Gaudi::Vector6& alignframe ) { m_alignframe = alignframe ; }
    const Gaudi::Vector6& alignFrame() const { return m_alignframe ; }
    
    double fracNonOutlier() const { return 1 - (m_numHits >0 ? m_numOutliers/double(m_numHits): 0 ) ; }
    double weightV() const { return m_weightV ; }
    double weightR() const { return m_weightR ; }
    size_t numHits() const { return m_numHits ; }
    size_t numOutliers() const { return m_numOutliers ; }
    size_t numTracks() const { return m_numTracks ; }
    void addHitSummary(double V, double R, const Gaudi::XYZPoint& hitpos) {
      m_numHits += 1 ;
      m_weightV += 1/V ;
      m_weightR += R/(V*V) ;
      m_sumX += hitpos.x() ;
      m_sumY += hitpos.y() ;
      m_sumZ += hitpos.z() ;
    }
    Gaudi::XYZPoint averageHitPosition() const {
      return m_numHits>0 ? Gaudi::XYZPoint( m_sumX/m_numHits, m_sumY/m_numHits,m_sumZ/m_numHits) : Gaudi::XYZPoint() ;
    }
    void addTrack() { ++m_numTracks ; }

    Gaudi::Vector6       m_alignframe ;       // parameters of the alignment frame
    Gaudi::Vector6       m_alpha ;            // the set of parameters at which the derivatives are computed
    Gaudi::Vector6       m_dChi2DAlpha ;      // (half) 1st derivative
    Gaudi::SymMatrix6x6  m_d2Chi2DAlpha2;     // (half) 2nd derivative diagonal ('this-module')
    OffDiagonalContainer m_d2Chi2DAlphaDBeta; // (half) 2nd derivative off-diagonal ('module-to-module')
    TrackDerivatives     m_dStateDAlpha ;     // derivative of track parameter to alpha
    VertexDerivatives    m_dVertexDAlpha ;    // derivative of vertex position to alpha
    size_t               m_numHits ;
    size_t               m_numOutliers ;
    size_t               m_numTracks ;
    double               m_weightV ; // sum V^{-1}          --> weight of 1st derivative
    double               m_weightR ; // sum V^{-1} R V^{-1} --> weight of 2nd derivative
    bool                 m_alphaIsSet ;
    double               m_sumX ;
    double               m_sumY ;
    double               m_sumZ ;
  } ;
  
  class Equations {
  public:
    typedef std::vector<ElementData> ElementContainer  ;
    Equations(size_t nElem=0, Gaudi::Time time = Gaudi::Time::epoch()) ;
    void clear() ;
    size_t nElem() const { return m_elements.size() ; }
    ElementData& element(size_t i) { return m_elements[i] ; }
    const ElementData& element(size_t i) const { return m_elements[i] ; }

     void addChi2Summary( double chisq, size_t ndof, size_t nexternal) {
      m_totalChiSquare +=chisq;
      m_totalNumDofs   +=ndof ;
      m_numExternalHits+=nexternal ;
    }
    
    void addVertexChi2Summary( double chisq, size_t ndof ) {
      m_totalVertexChiSquare +=chisq;
      m_totalVertexNumDofs   +=ndof ;
    }
    
    void addEventSummary( size_t numtracks, 
			  size_t numvertices,
			  size_t numdimuons,
			  Gaudi::Time time,
			  size_t runnr ) {
      ++m_numEvents ;
      m_numTracks += numtracks ;
      m_numVertices += numvertices ;
      m_numDiMuons += numdimuons ;
      if( m_firstTime.ns() > time.ns() ) m_firstTime = time ;
      if( m_lastTime.ns() < time.ns()  ) m_lastTime  = time ;
      if( m_firstRun > runnr ) m_firstRun = runnr ;
      if( m_lastRun < runnr  ) m_lastRun  = runnr ;
    }

    size_t numTracks() const { return m_numTracks ; }
    size_t numVertices() const { return m_numVertices ; }
    size_t numDiMuons() const { return m_numDiMuons ; }
    size_t numEvents() const { return m_numEvents ; }
    Gaudi::Time firstTime() const { return m_firstTime ; }
    Gaudi::Time lastTime() const { return m_lastTime ; }
    Gaudi::Time initTime() const { return m_initTime ; }
    size_t firstRun() const { return m_firstRun ; }
    size_t lastRun() const { return m_lastRun ; }
    size_t numHits() const ;
    double totalChiSquare() const { return m_totalChiSquare ; }
    size_t totalNumDofs() const { return m_totalNumDofs ; }
    double totalVertexChiSquare() const { return m_totalVertexChiSquare ; }
    size_t totalVertexNumDofs() const { return m_totalVertexNumDofs ; }
    double totalTrackChiSquare() const { return totalChiSquare() - m_totalVertexChiSquare ; }
    size_t totalTrackNumDofs() const { return totalNumDofs() - m_totalVertexNumDofs ; }
    size_t numExternalHits() const { return m_numExternalHits ; }
    
    void writeToFile(const char* filename) const ;
    void readFromFile(const char* filename) ;
    void writeToBuffer(std::ofstream& buffer) const ;
    void readFromBuffer(std::ifstream& buffer) ;
    void add(const Al::Equations&) ;

    std::ostream& fillStream(std::ostream& os) const ;

    const ElementContainer& elements() const { return m_elements ; }
  private:
    ElementContainer m_elements ;
    size_t           m_numEvents ;
    size_t           m_numTracks ;
    size_t           m_numVertices ;
    size_t           m_numDiMuons ;
    double           m_totalChiSquare ;
    size_t           m_totalNumDofs ;
    size_t           m_numExternalHits ;
    double           m_totalVertexChiSquare ;
    size_t           m_totalVertexNumDofs ;
    Gaudi::Time      m_firstTime ;
    Gaudi::Time      m_lastTime ;
    Gaudi::Time      m_initTime ;
    size_t           m_firstRun ;
    size_t           m_lastRun ;
  };
  
}
#endif // ALIGNSOLVTOOLS_ALEQUATIONS_H
