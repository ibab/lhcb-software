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
    : m_nbDifferent(0) 
  {
    m_planeList.resize( 12, 0 );
  }

  virtual ~PrPlaneCounter( ) {} ///< Destructor

  /// set the counters from the specified range.
  void set( PrHits::const_iterator itBeg, PrHits::const_iterator itEnd )  {
    m_nbDifferent = 0;
    std::fill( m_planeList.begin(), m_planeList.end(), 0 );
    for ( PrHits::const_iterator itH = itBeg; itEnd != itH; ++itH) {
      addHit( *itH );
    }
  }

  /// add a hit to be counted
  int addHit( const PrHit* hit ) {
    if ( hit->isUsed() ) return m_nbDifferent;
    if ( 0 == m_planeList[hit->planeCode()]++ ) ++m_nbDifferent;
    return m_nbDifferent;
  }

  /// remove a hit
  int removeHit( const PrHit* hit ) {
    if ( 0 == --m_planeList[hit->planeCode()] ) --m_nbDifferent;
    return m_nbDifferent;
  }

  /// returns number of different planes
  unsigned int nbDifferent() const { return m_nbDifferent; }

  /// returns number of hits in specified plane
  int nbInPlane( int plane ) const { return m_planeList[plane]; }

private:
  std::vector<int> m_planeList;
  unsigned int m_nbDifferent;
};

#endif // PRPLANECOUNTER_H
