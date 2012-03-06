#ifndef TALIGNMENT_ALDOFMASK_H
#define TALIGNMENT_ALDOFMASK_H

//#include <boost/array.hpp>
#include <vector>
#include <algorithm>

class AlDofMask
{
public:
  typedef std::vector<bool> MaskType ;

  /** Constructors */
  AlDofMask() {}
  AlDofMask(size_t size, bool isactive = true) ;
  AlDofMask(const MaskType& mask) ;
  //template<class Container> AlDofMask(const Container& mask) ;
  
  // number of active (unmasked) parameters
  size_t nActive() const { return m_activetoallmap.size() ; }
  // transform from an 'active' parameter to an 'all' parameter
  int parIndex(size_t iactivepar) const { 
    return iactivepar<m_activetoallmap.size() ? m_activetoallmap[iactivepar] : -1 ; }
  // transform from an 'all' parameter to an 'active' parameter
  int activeParIndex(size_t ipar) const { 
    return ipar<m_mask.size() ? m_alltoactivemap[ipar] : -1 ; }
  // 
  bool isActive(size_t ipar) const { 
    return ipar<m_mask.size() ? m_mask[ipar] : false ; }
  //
  void setActive(size_t ipar, bool b) { 
    if( b != isActive(ipar) ) { m_mask[ipar] = b ; fillIndexMaps() ; } }
  //
  typedef MaskType::const_iterator const_iterator ;
  const_iterator begin() const { return m_mask.begin() ; }
  const_iterator end()   const { return m_mask.end() ; }
  size_t size() const { return m_mask.size() ; }
  void push_back( bool mask ) { m_mask.push_back(mask) ; fillIndexMaps() ; }
  void push_back( const AlDofMask& mask ) ;
  
private:
  // fill the maps that transform betweem 'active' and 'all'
  void   fillIndexMaps() ;
  
private:
  MaskType                   m_mask;
  std::vector<int>           m_alltoactivemap ;
  std::vector<unsigned char> m_activetoallmap; 
} ;

inline AlDofMask::AlDofMask(size_t size, bool active) 
  : m_mask(size,active)
{
  fillIndexMaps() ;
}

inline AlDofMask::AlDofMask(const MaskType& mask) 
  : m_mask(mask)
{
  fillIndexMaps() ; 
}

inline void AlDofMask::fillIndexMaps() 
{
  m_activetoallmap.clear() ;
  m_alltoactivemap.clear() ;
  if( m_mask.size()>0) {
    for(unsigned int ipar=0; ipar<m_mask.size(); ++ipar) 
      if( m_mask[ipar] ) m_activetoallmap.push_back(ipar) ;
    m_alltoactivemap.resize(size(),-1) ;
    for(unsigned int iactive=0; iactive<m_activetoallmap.size(); ++iactive)
      m_alltoactivemap[m_activetoallmap[iactive]] = iactive ;
  }
}


inline void AlDofMask::push_back( const AlDofMask& rhs ) 
{
  m_mask.insert(m_mask.end(), rhs.m_mask.begin(), rhs.m_mask.end() ) ;
  fillIndexMaps() ;
}

#endif
