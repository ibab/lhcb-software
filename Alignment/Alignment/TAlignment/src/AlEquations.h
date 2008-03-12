#include <vector>
#include <map>
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"

namespace Al
{
  struct ElementData
  {
    ElementData() : m_numHits(0), m_weight(0) {}
    void add(const ElementData& rhs) ;
    typedef std::map<int, Gaudi::Matrix6x6> OffdiagonalContainer ;
    Gaudi::Vector6       m_v;
    OffdiagonalContainer m_m;
    size_t               m_numHits ;
    double               m_weight ;
  } ;
  
  class Equations {
  public:
    Equations(size_t nElem)
      : m_elements(nElem),
	m_numTracks(0u), 
	m_totalChiSquare(0.0), 
	m_totalNumDofs(0u), 
	m_numExternalHits(0u)
    {}

    void clear() ;
    size_t nElem() const { return m_elements.size() ; }
    Gaudi::Vector6&       V(int i) { return m_elements[i].m_v ; }
    const Gaudi::Vector6& V(int i) const { return m_elements[i].m_v ; }
    //FIXME: return proxy that fixes i<j...
    Gaudi::Matrix6x6& M(int i, int j) { assert(i<=j); return m_elements[i].m_m[j] ; }
    const Gaudi::Matrix6x6& M(int i, int j) const { return const_cast<Equations*>(this)->M(i,j) ; }
    
    size_t numHits(int i) const { return m_elements[i].m_numHits ; }
    double weight(int i) const { return m_elements[i].m_weight ; }
    
    void addTrackSummary( double chisq, size_t ndof, size_t nexternal) {
      m_totalChiSquare +=chisq;
      m_totalNumDofs   +=ndof ;
      m_numExternalHits+=nexternal ;
      ++m_numTracks;
    }
    
    void addHitSummary(int index, double sigma) {
      m_elements[index].m_numHits += 1 ;
      m_elements[index].m_weight  += 1/(sigma*sigma) ;
    }
    
    size_t numTracks() const { return m_numTracks ; }
    double totalChiSquare() const { return m_totalChiSquare ; }
    size_t totalNumDofs() const { return m_totalNumDofs ; }
    size_t numExternalHits() const { return m_numExternalHits ; }
    
    void writeToFile(const char* filename) const ;
    void readFromFile(const char* filename) ;
    void add(const Al::Equations&) ;
  private:
    typedef std::vector<ElementData> ElementContainer  ;
    ElementContainer m_elements ;
    size_t           m_numTracks ;
    double           m_totalChiSquare ;
    size_t           m_totalNumDofs ;
    size_t           m_numExternalHits ;
  };
  
} ;
