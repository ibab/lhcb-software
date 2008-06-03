#ifndef OTDAQ_GOL_H
#define OTDAQ_GOL_H

#include "OTDAQ/RawHit.h"
#include "OTDAQ/GolHeader.h"
#include <vector>

namespace OTDAQ
{

  class Gol
  {
  public:
    typedef std::vector<RawHit> RawHitContainer ;
    Gol() {}
    Gol( const GolHeader& header ) : m_header(header) {}
    const GolHeader& header() const { return m_header ; }
    const RawHitContainer& hits() const { return m_hits ; }
    RawHitContainer& hits() { return m_hits ; }
    //   public:
    //     void push_back( const RawHit& hit ) { m_hits.push_back ; }
    //     template<class iterator> 
    //     void insert( iterator begin, iterator end ) { m_hits.insert(begin,end) ; }
  private:
    GolHeader m_header ;
    RawHitContainer m_hits ;
  } ;
  
}

#endif
