#ifndef CLEANING_H
#define CLEANING_H

/**  @class Cleaning Cleaning.h L0MuonKernel/Cleaning.h 
    
    Class for cleaning adjacents pads in M3. 
    If two pads are adjacent, only the closest to the beam 
    pipe is kept

    @author  Luisanna Tocco
    @date  4 July 2003

*/

#include <vector>
#include <utility>  // For std::pair
#include <boost/dynamic_bitset.hpp>

namespace L0Muon {
  
class Cleaning {

 public:
  
  /// Constructor 
  Cleaning();

  /// Destructor
  ~Cleaning();
    
  typedef std::pair<int,int> HitIndex;
  typedef std::vector< HitIndex > vec; 
  typedef std::vector< boost::dynamic_bitset<> > SeedsMap ; 

  vec cleanSeed(vec&);

  /// Clean the seed map
  std::vector<boost::dynamic_bitset<> > cleanSeed(SeedsMap & map);
  

 private:
    
  HitIndex m_seedsi, m_seedsj;
  SeedsMap m_seedsmap;
  
 
};

};

#endif

