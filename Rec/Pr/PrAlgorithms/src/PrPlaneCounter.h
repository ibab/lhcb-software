#ifndef PRPLANECOUNTER_H
#define PRPLANECOUNTER_H 1

// Include files
#include "PrKernel/PrHit.h"

/** @class PrPlaneCounter PrPlaneCounter.h
 *  Small class to count how many different planes are in a list and how many planes with a single hit fired
 *  
 *  @author Olivier Callot
 *  @date   2012-03-23
 *  @author Michel De Cian
 *  @date   2014-03-12 Added number of planes which only have a single hit
 */

class PrPlaneCounter {
 public:
  
  /// Standard constructor
 PrPlaneCounter( )
   : m_nbVals( {{0,0}} ),
    m_planeList( {{0,0,0,0,0,0,0,0,0,0,0,0}} ),
    m_first(),
    m_last()
      {
      }
  
  virtual ~PrPlaneCounter( ) {} ///< Destructor
  
  /** Set values (fired planes, single planes, different planes) for a given range of iterators
   *  @brief Set values for a given range of iterators
   *  @param itBeg First iterator, begin of range
   *  @param itEnd Last iterator, end of range
   *  @param fill Should the values be reset?
   */
  void set( PrHits::const_iterator itBeg, PrHits::const_iterator itEnd, const bool fill = true)  {
    
    m_first = itBeg;
    m_last  = itEnd;
    if(fill){
      m_planeList = {{0,0,0,0,0,0,0,0,0,0,0,0}};
      m_nbVals    = {{0,0}};
    }
    
    for ( PrHits::const_iterator itH = itBeg; itEnd != itH; ++itH) {
      if ( (*itH)->isUsed()  ) continue;
      if ( 0 == m_planeList[(*itH)->planeCode()]++ ){
          ++m_nbVals[0];
          ++m_nbVals[1];
      }else{
        if ( 2 ==  m_planeList[(*itH)->planeCode()]){
          --m_nbVals[1];
        }
      }
    }
  }
  
  PrHits::const_iterator first() const{ return m_first;}
  PrHits::const_iterator last()  const{ return m_last; }

  /** Update values for additional hit
   *  @param hit Hit to be added
   *  @return int number of different planes
   */
  int addHit( const PrHit* hit ) {
    if ( hit->isUsed() ) return m_nbVals[0];
    if ( 0 == m_planeList[hit->planeCode()]++ ){
      ++m_nbVals[0];
      ++m_nbVals[1];
    }else{
      if ( 2 ==  m_planeList[hit->planeCode()]){
        --m_nbVals[1];
      }    
    }
    return m_nbVals[0];
  }

  /** Update values for removed hit
   *  @param hit Hit to be removed
   *  @return int number of different planes
   */
  int removeHit( const PrHit* hit ) {
    if ( hit->isUsed() ) return m_nbVals[0];
    if ( 0 == --m_planeList[hit->planeCode()] ){
      --m_nbVals[0];
      --m_nbVals[1];
    }else{
      if( 1 == m_planeList[hit->planeCode()]){
        ++m_nbVals[1];
      }
    }
    return m_nbVals[0];
  }

  /// returns number of different planes
  unsigned int nbDifferent() const { return m_nbVals[0]; }

  /// returns number of hits in specified plane
  int nbInPlane( const int plane ) const { return m_planeList[plane]; }

  /// returns number of single planes
  unsigned int nbSingle() const { return m_nbVals[1];}
  
  /// clear list with hits in planes and number of different planes / single-hit planes 
  void clear(){
    m_planeList.fill(0);
    m_nbVals.fill(0);
  }
  
  struct LowerBySize{ //Before higher number of hits and better chi2
    bool operator() ( const PrPlaneCounter& lhs, const PrPlaneCounter& rhs) const{
      if(lhs.nbDifferent() == rhs.nbDifferent()){
        float diff_lhs= std::fabs( (*lhs.last())->coord()-(*lhs.first())->coord());
        float diff_rhs= std::fabs( (*rhs.last())->coord()-(*rhs.first())->coord());
	return diff_lhs < diff_rhs;
      }
      return lhs.nbDifferent() > rhs.nbDifferent();
    }
  };
  

private:
  
  /// array for: number of different plane (0) and number of planes with single hit (1) 
  std::array<unsigned int,2> m_nbVals;
  std::array<int,12> m_planeList;
  PrHits::const_iterator m_first;
  PrHits::const_iterator m_last;
};

#endif // PRPLANECOUNTER_H
