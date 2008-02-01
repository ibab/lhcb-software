#ifndef TALIGNMENT_ALDOFMASK_H
#define TALIGNMENT_ALDOFMASK_H

#include <boost/array.hpp>
#include <vector>
#include <algorithm>

class AlDofMask
{
public:
  /** Enums, typedefs */
  enum { NumPars = 6 } ;
  typedef boost::array<bool,NumPars> MaskType ;
  
  /** Constructors */
  AlDofMask() ;
  AlDofMask(const std::vector<bool>& mask) ;
  AlDofMask(const MaskType& mask) ;
  
  // number of active (unmasked) parameters
  size_t nActive() const { return m_activetoallmap.size() ; }
  // transform from an 'active' parameter to an 'all' parameter
  int parIndex(size_t iactivepar) const { assert(iactivepar<m_activetoallmap.size()) ; return m_activetoallmap[iactivepar] ; }
  // transform from an 'all' parameter to an 'active' parameter
  int activeParIndex(size_t ipar) const { assert(ipar<m_mask.size()); return m_alltoactivemap[ipar] ; }
  // 
  bool isActive(size_t ipar) const { assert(ipar<m_mask.size()) ; return m_mask[ipar] ; }
  //
  void setActive(size_t ipar, bool b) { if( b != isActive(ipar) ) { m_mask[ipar] = b ; fillIndexMaps() ; } }
  //
  bool operator[](size_t ipar) const { return isActive(ipar) ; }
  //
  bool& operator[](size_t ipar) { assert(ipar<m_mask.size()) ; return m_mask[ipar] ; }
  //
  typedef MaskType::const_iterator const_iterator ;
  const_iterator begin() const { return m_mask.begin() ; }
  const_iterator end()   const { return m_mask.end() ; }

private:
  // fill the maps that transform betweem 'active' and 'all'
  void   fillIndexMaps() ;
  
private:
  MaskType                   m_mask;
  boost::array<int,NumPars>  m_alltoactivemap ;
  std::vector<unsigned char> m_activetoallmap; 
} ;

inline AlDofMask::AlDofMask()
{
  for(MaskType::iterator it = m_mask.begin(); it != m_mask.end(); ++it) *it = true ;
  fillIndexMaps() ;
}

inline AlDofMask::AlDofMask(const std::vector<bool>& mask)
{
  assert(mask.size()==m_mask.size()) ;
  for(size_t i=0; i<m_mask.size(); ++i) m_mask[i] = mask[i] ;
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
  for(unsigned int ipar=0; ipar<m_mask.size(); ++ipar) {
    if( m_mask[ipar] ) m_activetoallmap.push_back(ipar) ;
    m_alltoactivemap[ipar] = -1 ; 
  }
  for(unsigned int iactive=0; iactive<m_activetoallmap.size(); ++iactive)
    m_alltoactivemap[m_activetoallmap[iactive]] = iactive ;
}

#endif
