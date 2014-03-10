#ifndef PRPLANECOUNTER_H
#define PRPLANECOUNTER_H 1

// Include files
#include "PrKernel/PrHit.h"

/** @class PrPlaneCounter PrPlaneCounter.h
 *  Small class to count how many different planes are in a list.
 *
 *  @author Olivier Callot
 *  @date   2012-03-23
 */

class PrPlaneCounter {
public:

  /// Standard constructor
  PrPlaneCounter( )
    : m_nbVals( {0,0} ),
      m_planeList( {0,0,0,0,0,0,0,0,0,0,0,0 })
                    
  {
  }

  virtual ~PrPlaneCounter( ) {} ///< Destructor

  /// set the counters from the specified range.
  void set( PrHits::const_iterator itBeg, PrHits::const_iterator itEnd, const bool fill = true)  {
    
    if(fill){
      m_planeList.fill(0);
      m_nbVals.fill(0);
    }
    
    for ( PrHits::const_iterator itH = itBeg; itEnd != itH; ++itH) {
      if ( (*itH)->isUsed() ) continue;
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

  /// add a hit to be counted
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

  /// remove a hit
  int removeHit( const PrHit* hit ) {
    if ( hit->isUsed() ) return m_nbVals[0];
    if ( 0 == --m_planeList[hit->planeCode()] ){
      --m_nbVals[0];
      ++m_nbVals[1];
    }else{
      if( 0 == m_planeList[hit->planeCode()]){
        --m_nbVals[1];
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
  

private:
  
  /// array for: number of different plane and number of planes with single hit 
  std::array<unsigned int,2> m_nbVals;
  std::array<int,12> m_planeList;
};

#endif // PRPLANECOUNTER_H
