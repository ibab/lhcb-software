#include "AlignKernel/AlEquations.h"
#include "AlignKernel/AlFileIO.h"

#include <sstream>
#include <fstream>
#include <iostream>

namespace Al
{

  namespace FileIO {

    std::ofstream& operator<<(std::ofstream& file, const Al::ElementData& data)
    {
      file  << data.m_alpha
      << data.m_dChi2DAlpha
      << data.m_d2Chi2DAlpha2
      << data.m_d2Chi2DAlphaDBeta
      << data.m_dStateDAlpha
      << data.m_dVertexDAlpha
      << data.m_numHits
      << data.m_numOutliers
      << data.m_numTracks
      << data.m_weightV
      << data.m_weightR
      << data.m_alphaIsSet
      << data.m_sumX
      << data.m_sumY
      << data.m_sumZ ;
      return file ;
    }

    inline
    std::ifstream& operator>>(std::ifstream& file, Al::ElementData& data)
    {
      file  >> data.m_alpha
      >> data.m_dChi2DAlpha
      >> data.m_d2Chi2DAlpha2
      >> data.m_d2Chi2DAlphaDBeta
      >> data.m_dStateDAlpha
      >> data.m_dVertexDAlpha
      >> data.m_numHits
      >> data.m_numOutliers
      >> data.m_numTracks
      >> data.m_weightV
      >> data.m_weightR
      >> data.m_alphaIsSet
      >> data.m_sumX
      >> data.m_sumY
      >> data.m_sumZ ;
      return file ;
    }
  }

  ElementData::ElementData()
    : m_numHits(0), m_numOutliers(0), m_numTracks(0), m_weightV(0), m_weightR(0), m_alphaIsSet(false),
      m_sumX(0), m_sumY(0), m_sumZ(0)
  {}

  void ElementData::add( const ElementData& rhs )
  {
    if( !m_alphaIsSet && rhs.m_alphaIsSet ) {
      // if this is the first one and the parameters are not set, copy the parameters
      m_alpha = rhs.m_alpha ;
      m_alphaIsSet = rhs.m_alphaIsSet ;
    }
    m_dChi2DAlpha += rhs.m_dChi2DAlpha ;
    m_d2Chi2DAlpha2 += rhs.m_d2Chi2DAlpha2 ;
    m_numHits += rhs.m_numHits ;
    m_numOutliers += rhs.m_numOutliers ;
    m_numTracks += rhs.m_numTracks ;
    m_weightV += rhs.m_weightV ;
    m_weightR += rhs.m_weightR ;
    m_dStateDAlpha += rhs.m_dStateDAlpha ;
    m_dVertexDAlpha += rhs.m_dVertexDAlpha ;
    for( OffDiagonalContainer::const_iterator rhsit = rhs.m_d2Chi2DAlphaDBeta.begin() ;
  rhsit != rhs.m_d2Chi2DAlphaDBeta.end() ; ++ rhsit) {
      OffDiagonalContainer::iterator it = m_d2Chi2DAlphaDBeta.find(rhsit->first) ;
      if( it==m_d2Chi2DAlphaDBeta.end() )
  m_d2Chi2DAlphaDBeta.insert( *rhsit ) ;
      else
  it->second += rhsit->second ;
    }
    m_sumX += rhs.m_sumX ;
    m_sumY += rhs.m_sumY ;
    m_sumZ += rhs.m_sumZ ;
  }

  void ElementData::transform( const Gaudi::Matrix6x6& jacobian )
  {
    ElementData tmp = *this ;
    m_dChi2DAlpha = jacobian * tmp.m_dChi2DAlpha ;
    m_d2Chi2DAlpha2 = ROOT::Math::Similarity( jacobian, tmp.m_d2Chi2DAlpha2 ) ;
    m_dStateDAlpha = jacobian * tmp.m_dStateDAlpha ;
    m_dVertexDAlpha = jacobian * tmp.m_dVertexDAlpha ;
    m_d2Chi2DAlphaDBeta.clear() ;
    for( OffDiagonalContainer::const_iterator it = tmp.m_d2Chi2DAlphaDBeta.begin() ;
  it != tmp.m_d2Chi2DAlphaDBeta.end() ; ++ it)
      m_d2Chi2DAlphaDBeta[it->first].matrix() = jacobian * it->second.matrix() ;
  }

  Equations::Equations(size_t nElem, Gaudi::Time initTime)
    : m_elements(nElem),
      m_numEvents(0u),
      m_numTracks(0u),
      m_numVertices(0u),
      m_numDiMuons(0u),
      m_totalChiSquare(0.0),
      m_totalNumDofs(0u),
      m_numExternalHits(0u),
      m_totalVertexChiSquare(0.0),
      m_totalVertexNumDofs(0u),
      m_firstTime(Gaudi::Time::max()),
      m_lastTime(Gaudi::Time::epoch()),
      m_initTime(initTime),
      m_firstRun(size_t(-1)),
      m_lastRun(0)
  {
    assert( nElem == m_elements.size() ) ;
  }

  void Equations::clear()
  {
    size_t nelem = m_elements.size() ;
    m_elements.clear() ;
    m_elements.resize(nelem) ;
    m_numEvents       = 0u ;
    m_numTracks       = 0u ;
    m_numVertices     = 0u ;
    m_numDiMuons      = 0u ;
    m_totalChiSquare  = 0.0 ;
    m_totalNumDofs    = 0u ;
    m_numExternalHits = 0u ;
    m_totalVertexChiSquare  = 0.0 ;
    m_totalVertexNumDofs    = 0u ;
    m_firstTime = Gaudi::Time(Gaudi::Time::max()) ;
    m_lastTime = Gaudi::Time(Gaudi::Time::epoch()) ;
  }

  void Equations::writeToBuffer(std::ofstream& buffer) const
  {
    using namespace FileIO ;
    buffer << m_elements
    << m_numEvents
    << m_numTracks
    << m_numVertices
    << m_numDiMuons
    << m_totalChiSquare
    << m_totalNumDofs
    << m_numExternalHits
    << m_totalVertexChiSquare
    << m_totalVertexNumDofs
    << m_firstTime
    << m_lastTime
    << m_initTime
    << m_firstRun
    << m_lastRun ;
  }

  void Equations::readFromBuffer(std::ifstream& buffer)
  {
    using namespace FileIO ;
    buffer >> m_elements
    >> m_numEvents
    >> m_numTracks
    >> m_numVertices
    >> m_numDiMuons
    >> m_totalChiSquare
    >> m_totalNumDofs
    >> m_numExternalHits
    >> m_totalVertexChiSquare
    >> m_totalVertexNumDofs
    >> m_firstTime
    >> m_lastTime
    >> m_initTime
    >> m_firstRun
    >> m_lastRun ;
  }

  void Equations::writeToFile(const char* filename) const
  {
    std::ofstream file(filename,std::ios::out | std::ios::binary);
    writeToBuffer(file) ;
    std::cout << "Al::Equations::writeToFile: INFO:: wrote " << m_numEvents << " events in " <<  file.tellp() << " bytes to file" << std::endl ;
    file.close() ;
  }

  void Equations::readFromFile(const char* filename)
  {
    std::ifstream file(filename,std::ios::in | std::ios::binary);
    if( file.is_open() ) {
      readFromBuffer(file) ;
      std::cout << "Al::Equations::readFromFile: INFO:: read " << m_numEvents << " events in " << file.tellg() << " bytes from file "<<filename << std::endl ;
      file.close() ;
    } else {
      printf("Cannot open input file: \'%s\'",filename) ;
    }
  }

  void Equations::add(const Equations& rhs)
  {
    // if this if the first one, we just copy
    if( m_elements.empty() || m_numEvents==0 ) {
      m_elements = rhs.m_elements ;
      m_initTime = rhs.m_initTime ;
    } else {
      if( m_elements.size() != rhs.m_elements.size() ) {
	std::cout << "Al::Equations::add: INFO:: adding up derivatives with different sizes: This:"
		  <<  m_elements.size() <<" RHS:" << rhs.m_elements.size()<< std::endl ;
//	assert(0) ;
	return;
      }
      if( m_initTime != rhs.m_initTime )
	std::cout << "Al::Equations::add: INFO:: adding up Equations with different initTime: "
		<< m_initTime << " and " << rhs.m_initTime << ". Make sure that you know what you are doing."
		<< std::endl ;

      // this is a tricky one. if we add up derivatievs that were
      // actually computed around a different point, then we need to
      // update all first derivatives. we correct the rhs before doing
      // the addition. Note the signs. I think that they are correct now.
      Equations ncrhs = rhs ;
      size_t index(0) ;
      for( ElementContainer::iterator rhsit = ncrhs.m_elements.begin() ;
          rhsit != ncrhs.m_elements.end(); ++rhsit, ++index) {
        // compute delta-alpha (usign the 'old' rhs)
        Gaudi::Vector6 deltaalpha = rhs.m_elements[index].m_alpha - m_elements[index].m_alpha ;
  if( ROOT::Math::Dot(deltaalpha,deltaalpha) > 1e-12 )
    std::cout << "Al::Equations::add: INFO:: Correcting for shifted alignment: "
	      << deltaalpha << std::endl ;
  // first the diagonal
        rhsit->m_dChi2DAlpha += rhsit->m_d2Chi2DAlpha2 * deltaalpha ;
        // now all the correlated elements
        for( ElementData::OffDiagonalContainer::const_iterator offdiagit = rhsit->m_d2Chi2DAlphaDBeta.begin() ;
            offdiagit != rhsit->m_d2Chi2DAlphaDBeta.end() ; ++offdiagit) {
          // the upper diagonal
          ncrhs.m_elements[ offdiagit->first ].m_dChi2DAlpha += ROOT::Math::Transpose(offdiagit->second.matrix()) * deltaalpha ;
          // but also the lower diagonal. for this we need the delta-alpha of the other element
          Gaudi::Vector6 deltaalphaj = rhs.m_elements[ offdiagit->first ].m_alpha - m_elements[ offdiagit->first ].m_alpha ;
          rhsit->m_dChi2DAlpha += offdiagit->second.matrix() * deltaalphaj ;
        }
        // update alpha itself
        rhsit->m_alpha = m_elements[index].m_alpha ;
      }

      // only now add the right-hand-side
      ElementContainer::iterator it = m_elements.begin() ;
      for( ElementContainer::const_iterator rhsit = ncrhs.m_elements.begin() ;
    rhsit != ncrhs.m_elements.end(); ++rhsit, ++it) (*it).add( *rhsit ) ;
    }

    m_numEvents       += rhs.m_numEvents ;
    m_numTracks       += rhs.m_numTracks ;
    m_numVertices     += rhs.m_numVertices ;
    m_numDiMuons      += rhs.m_numDiMuons ;
    m_totalChiSquare  += rhs.m_totalChiSquare ;
    m_totalNumDofs    += rhs.m_totalNumDofs ;
    m_numExternalHits += rhs.m_numExternalHits ;
    m_totalVertexChiSquare  += rhs.m_totalVertexChiSquare ;
    m_totalVertexNumDofs    += rhs.m_totalVertexNumDofs ;
    if( m_firstTime.ns() > rhs.m_firstTime.ns() ) m_firstTime = rhs.m_firstTime ;
    if( m_lastTime.ns()  < rhs.m_lastTime.ns()  ) m_lastTime  = rhs.m_lastTime ;
    if( m_firstRun > rhs.m_firstRun ) m_firstRun = rhs.m_firstRun ;
    if( m_lastRun  < rhs.m_lastRun  ) m_lastRun  = rhs.m_lastRun ;
    //std::cout << "Total number of events is now: "
    //<<  m_numEvents << " (added " << rhs.m_numEvents << ")" << std::endl ;
  }

  size_t Equations::numHits() const
  {
    size_t rc(0) ;
    for( ElementContainer::const_iterator it = m_elements.begin() ;
  it != m_elements.end(); ++it )
      rc += it->m_numHits ;
    return rc ;
  }

  std::ostream& Equations::fillStream(std::ostream& os) const
  {
    std::stringstream stream ;
    stream << "NumEvents = " << numEvents() << std::endl ;
    return os << stream.str() ;
  }

}
