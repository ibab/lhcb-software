#include "TAlignment/AlEquations.h"
#include "TAlignment/AlFileIO.h"

namespace Al
{   

  using namespace FileIO ;

  std::ofstream& operator<<(std::ofstream& file, const Al::ElementData& data)
  {
    file  << data.m_v
	  << data.m_m 
	  << data.m_numHits
	  << data.m_weight ;
    return file ;
  }
  
  inline
  std::ifstream& operator>>(std::ifstream& file, Al::ElementData& data)
  {
    file  >> data.m_v
	  >> data.m_m 
	  >> data.m_numHits
	  >> data.m_weight ;
    return file ;
  }

  void ElementData::add( const ElementData& rhs ) 
  {
    m_v       += rhs.m_v ;
    m_numHits += rhs.m_numHits ;
    m_weight  += rhs.m_weight ;
    for( OffdiagonalContainer::const_iterator rhsit = rhs.m_m.begin() ;
	 rhsit != rhs.m_m.end() ; ++ rhsit) {
      OffdiagonalContainer::iterator it = m_m.find(rhsit->first) ;
      if( it==m_m.end() ) 
	m_m.insert( *rhsit ) ;
      else
	it->second += rhsit->second ;
    }
  }
  
  void Equations::clear() 
  {
    size_t nelem = m_elements.size() ;
    m_elements.clear() ;
    m_elements.resize(nelem) ;
    m_totalChiSquare  = 0.0 ;
    m_numTracks       = 0u ;
    m_totalNumDofs    = 0u ;
    m_numExternalHits = 0u ;
  }

  void Equations::writeToFile(const char* filename) const 
  {
    std::ofstream file(filename,std::ios::out | std::ios::binary);
    file << m_elements
	 << m_numTracks
	 << m_totalChiSquare
	 << m_totalNumDofs
	 << m_numExternalHits ;
    file.close() ;
  }
  
  void Equations::readFromFile(const char* filename)
  {
    std::ifstream file(filename,std::ios::in | std::ios::binary);
    file >> m_elements
	 >> m_numTracks
	 >> m_totalChiSquare
	 >> m_totalNumDofs
	 >> m_numExternalHits ;
    file.close() ;
  }
  
  void Equations::add(const Equations& rhs)
  {
    assert( m_elements.size() == rhs.m_elements.size() ) ;
    ElementContainer::iterator it = m_elements.begin() ;
    for( ElementContainer::const_iterator rhsit = rhs.m_elements.begin() ;
	 rhsit != rhs.m_elements.end(); ++rhsit, ++it) (*it).add( *rhsit ) ;
    m_numTracks       += rhs.m_numTracks ;
    m_totalChiSquare  += m_totalChiSquare ;
    m_totalNumDofs    += m_totalNumDofs ;
    m_numExternalHits += m_numExternalHits ;
  }
  

}
