#ifndef CLEANING_H
#define CLEANING_H

/*  class Cleaning Cleaning L0MuonKernel/Cleaning.h 
    
    Class to eliminate seeds located in adjacent pads of M3

    author  Luisanna Tocco
    date  4 July 2003

*/

#include <vector>
#include <utility>  // For std::pair
#include <boost/dynamic_bitset.hpp>

namespace L0Muon {
  
class Cleaning {

 public:
  Cleaning();
  ~Cleaning();
    
  typedef std::pair<int,int> HitIndex;
  typedef std::vector< HitIndex > vec; 
  typedef std::vector< boost::dynamic_bitset<> > SeedsMap ; 

  vec cleanSeed(vec&);
  std::vector<boost::dynamic_bitset<> > cleanSeed(SeedsMap & map);
  

 private:
    
  HitIndex m_seedsi, m_seedsj;
  SeedsMap m_seedsmap;
  
 
};

};

#endif

