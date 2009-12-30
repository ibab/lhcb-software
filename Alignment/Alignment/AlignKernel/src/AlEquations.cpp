#include "AlignKernel/AlEquations.h"
#include "AlignKernel/AlFileIO.h"

#include <sstream>
#include <fstream>
#include <iostream>

namespace Al
{   

  std::ostream& operator<<(std::ostream& file, const Al::ElementData& data)
  {
    using namespace FileIO ;
    file  << data.m_dChi2DAlpha 
	  << data.m_d2Chi2DAlpha2
	  << data.m_d2Chi2DAlphaDBeta
	  << data.m_dStateDAlpha
	  << data.m_dVertexDAlpha
	  << data.m_numHits
	  << data.m_numOutliers
	  << data.m_numTracks
	  << data.m_weightV 
	  << data.m_weightR ;
    return file ;
  }
  
  inline
  std::ifstream& operator>>(std::ifstream& file, Al::ElementData& data)
  {
    using namespace FileIO ;
    file  >> data.m_dChi2DAlpha 
	  >> data.m_d2Chi2DAlpha2
	  >> data.m_d2Chi2DAlphaDBeta
      	  >> data.m_dStateDAlpha
      	  >> data.m_dVertexDAlpha
	  >> data.m_numHits
	  >> data.m_numOutliers
	  >> data.m_numTracks
	  >> data.m_weightV 
	  >> data.m_weightR ;
    return file ;
  }

  ElementData::ElementData()
    : m_numHits(0), m_numOutliers(0), m_numTracks(0), m_weightV(0), m_weightR(0) 
  {}
  
  void ElementData::add( const ElementData& rhs ) 
  {
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
      m_initTime(initTime)
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

  void Equations::writeToBuffer(std::ostream& buffer) const
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
	   << m_initTime ;
  }

  void Equations::readFromBuffer(std::istream& buffer)
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
	   >> m_initTime ;
  }

  void Equations::writeToFile(const char* filename) const 
  {
    std::ofstream file(filename,std::ios::out | std::ios::binary);
    writeToBuffer(file) ;
    // std::cout << "Equations::wroteToFile wrote " << file.tellg() << " bytes to file" << std::endl ;
    file.close() ;
  }
  
  void Equations::readFromFile(const char* filename)
  {
    std::ifstream file(filename,std::ios::in | std::ios::binary);
    readFromBuffer(file) ;
    // std::cout << "Equations::readFromFile read " << file.tellg() << " bytes from file" << std::endl ;
    file.close() ;
  }
  
  void Equations::add(const Equations& rhs)
  {
    if( m_elements.empty() ) m_elements.resize( rhs.m_elements.size() ) ;
    assert( m_elements.size() == rhs.m_elements.size() ) ;

    ElementContainer::iterator it = m_elements.begin() ;
    for( ElementContainer::const_iterator rhsit = rhs.m_elements.begin() ;
	 rhsit != rhs.m_elements.end(); ++rhsit, ++it) (*it).add( *rhsit ) ;
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
    if( m_initTime != Gaudi::Time(Gaudi::Time::epoch()) ) {
      if( m_initTime != rhs.m_initTime ) {
	std::cout << "Equations::add: adding up Equations with different initTime. Asserting."
		  << std::endl ;
	assert(0) ;
      }
    } else {
      m_initTime = rhs.m_initTime ;
    }
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
